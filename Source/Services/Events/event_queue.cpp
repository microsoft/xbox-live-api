// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_xsapi.h"
#include "local_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

EventQueue::EventQueue(
    User&& user,
    std::shared_ptr<cll::CllTenantSettings> tenantSettings
) :
    m_user{ std::move(user) },
    m_tenantSettings{ tenantSettings }
{
    auto state{ GlobalState::Get() };
    assert(state);
    m_localStorage = state->LocalStorage();

    Stringstream ss;
    ss << m_filenamePrefix << m_user.Xuid() << ".dir";
    m_directoryFilename = ss.str();
}

void EventQueue::Initialize()
{
    // Initialize in memory directory. Treat as write through cache, don't read from directory file on disk after this.
    m_localStorage->ReadAsync(m_user, m_directoryFilename,
        [
            weakThis = std::weak_ptr<EventQueue>{ shared_from_this() }
        ]
    (Result<xsapi_internal_vector<uint8_t>> result)
    {
        auto sharedThis{ weakThis.lock() };
        if (sharedThis && Succeeded(result))
        {
            auto& bytes = result.Payload();
            auto fileMetadata = utils::string_split_internal(xsapi_internal_string{ bytes.begin(), bytes.end() }, '\n');

            std::lock_guard<std::mutex> lock{ sharedThis->m_mutex };
            for (auto& metadata : fileMetadata)
            {
                xsapi_internal_stringstream ss{ metadata };
                xsapi_internal_string filename;
                uint64_t fileSize{ 0 };
                ss >> filename >> fileSize;

                sharedThis->m_fileMetadata[filename] = fileSize;
                sharedThis->m_totalFilesSize += fileSize;
            }

            sharedThis->Populate();
        }
    });
}

void EventQueue::Cleanup()
{
    // Doing this in a cleanup method rather than the destructor so that we can ensure queue lifetime
    // during the asynchronous flush. If we are already in the destructor, we can't take a shared reference
    // to the queue.
    std::lock_guard<std::mutex> lock{ m_mutex };

    // Make sure the failed payload gets flushed as well
    if (m_failedPayload)
    {
        m_queue.push_back(std::move(m_failedPayload));
    }
    Flush();
}

HRESULT EventQueue::AddEvent(Event&& event)
{
    return AddEvents(xsapi_internal_vector<Event>{ 1, std::move(event) });
}

HRESULT EventQueue::AddEvents(xsapi_internal_vector<Event>&& events)
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    std::shared_ptr<EventUploadPayload> payload;
    if (m_queue.empty())
    {
        auto copyUserResult = m_user.Copy();
        RETURN_HR_IF_FAILED(copyUserResult.Hresult());
        payload = MakeShared<EventUploadPayload>(copyUserResult.ExtractPayload(), m_tenantSettings);
        m_queue.push_back(payload);
    }
    else
    {
        payload = m_queue.back();
    }

    for (auto& event : events)
    {
        auto hr = payload->AddEvent(event);
        if (FAILED(hr))
        {
            auto copyUserResult = m_user.Copy();
            RETURN_HR_IF_FAILED(copyUserResult.Hresult());
            payload = MakeShared<EventUploadPayload>(copyUserResult.ExtractPayload(), m_tenantSettings);
            m_queue.push_back(payload);
            hr = payload->AddEvent(event);
            RETURN_HR_IF_FAILED(hr);
        }
    }

    if (m_mode == Mode::Offline)
    {
        return Flush();
    }

    return S_OK;
}

std::shared_ptr<EventUploadPayload> EventQueue::GetNextPayload(size_t minimumEventCount)
{
    std::lock_guard<std::mutex> guard{ m_mutex };

    std::shared_ptr<EventUploadPayload> nextPayload{ nullptr };

    if (m_failedPayload)
    {
        std::swap(m_failedPayload, nextPayload);
    }
    else if (!m_queue.empty() && m_queue.front()->EventCount() >= minimumEventCount)
    {
        nextPayload = m_queue.front();
        m_queue.pop_front();
    }

    return nextPayload;
}

void EventQueue::SetMode(Mode mode)
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    if (m_mode == mode)
    {
        return;
    }

    m_mode = mode;
    switch (mode)
    {
        case Mode::Offline:
        {
            assert(m_failedPayload);
            Flush();
            break;
        }
        case Mode::Normal:
        {
            Populate();
            break;
        }
    }
}

void EventQueue::RequeueFailedPayload(std::shared_ptr<EventUploadPayload> failedPayload)
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_failedPayload = std::move(failedPayload);
}

HRESULT EventQueue::Populate()
{
    // Ensure that m_mutex is locked when calling this helper function

    for (auto& metadata : m_fileMetadata)
    {
        HRESULT hr = m_localStorage->ReadAsync(
            m_user,
            metadata.first,
            [
                metadata = std::pair<xsapi_internal_string, uint64_t>{ metadata },
                weakThis = std::weak_ptr<EventQueue>{ shared_from_this() }
            ]
        (Result<Vector<uint8_t>> readResult)
        {
            auto sharedThis{ weakThis.lock() };
            if (sharedThis && Succeeded(readResult) && sharedThis->m_mode == Mode::Normal)
            {
                auto& bytes = readResult.Payload();
                auto serializedEvents = utils::string_split_internal(xsapi_internal_string{ bytes.begin(), bytes.end() }, '\n');

                for (const auto& serializedEvent : serializedEvents)
                {
                    auto deserializationResult = Event::Deserialize(serializedEvent);
                    if (Succeeded(deserializationResult))
                    {
                        sharedThis->AddEvent(deserializationResult.ExtractPayload());
                    }
                }

                {
                    std::lock_guard<std::mutex> lock{ sharedThis->m_mutex };
                    sharedThis->m_totalFilesSize -= metadata.second;
                    sharedThis->m_fileMetadata.erase(metadata.first);
                    sharedThis->WriteDirectoryFile();
                }

                auto clearAsyncHR = sharedThis->m_localStorage->ClearAsync(sharedThis->m_user, metadata.first, nullptr);
                if (FAILED(clearAsyncHR))
                {
                    // Log failure but don't let when failure prevent populating the rest of the files.
                    LOGS_WARN << "Failed to read events file due to user being available. HR = " << clearAsyncHR;
                }
            }
        });

        if (FAILED(hr))
        {
            // Log failure but don't let when failure prevent populating the rest of the files.
            LOGS_WARN << "Failed to read events file " << metadata.first << " that appeared in " << m_directoryFilename;
        }
    }

    return S_OK;
}

HRESULT EventQueue::Flush()
{
    // Ensure that m_mutex is locked when calling this helper function

    if (m_flushInProgress)
    {
        return S_OK;
    }

    // If there is an existing non-full file, continue writing to that
    std::pair<String, uint64_t> fileMetadata{ "", 0 };
    if (!m_fileMetadata.empty() && m_fileMetadata.rbegin()->second < m_maxFileSize)
    {
        fileMetadata = *m_fileMetadata.rbegin();
    }

    xsapi_internal_vector<uint8_t> flushData;
    auto targetDataSize = static_cast<size_t>(m_maxFileSize - fileMetadata.second);
    flushData.reserve(targetDataSize + static_cast<size_t>(m_tenantSettings->getMaxEventSizeInBytes()));
    xbox::services::datetime earliestTimestamp{ xbox::services::datetime::utc_now() };

    for (auto iter = m_queue.begin(); iter != m_queue.end(); iter = m_queue.erase(iter))
    {
        auto payload{ *iter };
        auto eventsRemainingInPayload = payload->ExtractEventsAndSerialize(flushData, targetDataSize, earliestTimestamp);

        if (eventsRemainingInPayload)
        {
            // The only reason events can be remaining after ExtractEventsAndSerialize is if the flushData was full.
            // Break early so the partial payload doesn't get erased from queue.
            break;
        }
    }

    if (flushData.empty())
    {
        return S_OK;
    }

    // Construct filename if we don't already have one
    if (fileMetadata.first.empty())
    {
        Stringstream ss;
        ss << m_filenamePrefix << std::hex << std::uppercase << earliestTimestamp.to_interval() << ".json";
        fileMetadata.first = ss.str();

        assert(m_fileMetadata.empty() || fileMetadata.first > m_fileMetadata.rend()->first);
    }

    m_flushInProgress = true;

    // To avoid client event data from being lost, we never want to cleanup before
    // we finish flushing unuploaded events
    auto holdCleanup{ GlobalState::Get() };
    assert(holdCleanup);

    HRESULT hr = m_localStorage->WriteAsync(
        m_user,
        XblLocalStorageWriteMode::Append,
        fileMetadata.first,
        std::move(flushData),
        [
            sharedThis{ shared_from_this() },
            filename{ fileMetadata.first },
            holdCleanup
        ]
    (Result<size_t> result)
    {
        std::lock_guard<std::mutex> lock{ sharedThis->m_mutex };
        assert(sharedThis->m_flushInProgress);
        sharedThis->m_flushInProgress = false;

        if (Succeeded(result))
        {
            auto previousFileSize{ sharedThis->m_fileMetadata[filename] };
            auto newFileSize{ result.ExtractPayload() };
            sharedThis->m_totalFilesSize += (newFileSize - previousFileSize);
            sharedThis->m_fileMetadata[filename] = newFileSize;

            while (sharedThis->m_totalFilesSize > sharedThis->m_storageAllotment)
            {
                LOGS_INFO << "Offline events files have exceeded the configured storage allotment.";
                LOGS_INFO << "The oldest events file will be deleted and offline event data will be permanently lost.";

                auto metadataToDelete{ sharedThis->m_fileMetadata.begin() };
                sharedThis->m_totalFilesSize -= metadataToDelete->second;
                
                auto clearAsyncHR = sharedThis->m_localStorage->ClearAsync(sharedThis->m_user, metadataToDelete->first, nullptr);
                if (SUCCEEDED(clearAsyncHR))
                {
                    sharedThis->m_fileMetadata.erase(metadataToDelete);
                }
                else 
                {
                    LOGS_INFO << "The user is currently unavailable. Failed to clear oldest event.";
                    return; //todo: What's the proper handling here?
                }
            }

            sharedThis->WriteDirectoryFile();
            sharedThis->Flush();
        }
    });

    return hr;
}

HRESULT EventQueue::WriteDirectoryFile()
{
    // Ensure that m_mutex is locked when calling this helper function

    xsapi_internal_vector<uint8_t> data;
    for (auto& metadata : m_fileMetadata)
    {
        data.insert(data.end(), metadata.first.begin(), metadata.first.end());
        data.push_back('\t');

        auto sizeString{ utils::uint64_to_internal_string(metadata.second) };
        data.insert(data.end(), sizeString.begin(), sizeString.end());
        data.push_back('\n');
    }

    auto holdCleanup{ GlobalState::Get() };
    assert(holdCleanup);

    return m_localStorage->WriteAsync(
        m_user,
        XblLocalStorageWriteMode::Truncate,
        m_directoryFilename,
        std::move(data),
        [
            holdCleanup
        ]
    (Result<size_t> result)
    {
        if (Failed(result))
        {
            LOGS_ERROR << "Failed to write events directory file. Offline event data may be lost!";
        }
    });
}

HRESULT EventQueue::SetMaxFileSize(uint64_t fileSizeInBytes)
{
    if (fileSizeInBytes < 1024)
    {
        LOGS_ERROR << "Max file size must be at least 1kb";
        return E_INVALIDARG;
    }

    m_maxFileSize = fileSizeInBytes;
    return S_OK;
}

HRESULT EventQueue::SetStorageAllotment(uint64_t storageAllotmentInBytes)
{
    if (storageAllotmentInBytes < m_maxFileSize)
    {
        LOGS_ERROR << "Storage allotment must be greater than the maximum file size.";
        return E_INVALIDARG;
    }

    m_storageAllotment = storageAllotmentInBytes;
    return S_OK;
}

uint64_t EventQueue::m_maxFileSize{ 128000 }; // default file size of 128k
uint64_t EventQueue::m_storageAllotment{ m_maxFileSize * 150 }; // default storage allotment of 150 files (~20MB)

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END
