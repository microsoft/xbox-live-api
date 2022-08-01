// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"
#include "xbox_live_context_internal.h"
#include "real_time_activity_manager.h"

using namespace xbox::services::system;
using namespace xbox::services::real_time_activity;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

PresenceService::PresenceService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
) noexcept
    : m_user{ std::move(user) },
    m_xboxLiveContextSettings{ xboxLiveContextSettings },
    m_rtaManager{ rtaManager },
    m_titleId{ AppConfig::Instance()->TitleId() }
{
    // Track title presence changes for this title by default
    m_trackedTitles.insert(std::pair<uint32_t, size_t>{ m_titleId, 1 });
}

PresenceService::~PresenceService() noexcept
{
    for (auto& xuidPair : m_trackedXuids)
    {
        if (!m_devicePresenceChangedHandlers.empty())
        {
            m_rtaManager->RemoveSubscription(m_user, xuidPair.second.devicePresenceChangedSub);
        }
        if (!m_titlePresenceChangedHandlers.empty())
        {
            for (auto& titlePair : xuidPair.second.titlePresenceChangedSubscriptions)
            {
                m_rtaManager->RemoveSubscription(m_user, titlePair.second);
            }
        }
    }
}

XblFunctionContext PresenceService::AddTitlePresenceChangedHandler(
    TitlePresenceChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    // Add subs to RTA manager if needed
    if (m_titlePresenceChangedHandlers.empty())
    {
        for (auto& xuidPair : m_trackedXuids)
        {
            for (auto& titlePair : m_trackedTitles)
            {
                auto sub{ MakeShared<TitlePresenceChangeSubscription>(xuidPair.first, titlePair.first, shared_from_this()) };
                xuidPair.second.titlePresenceChangedSubscriptions[titlePair.first] = sub;
                m_rtaManager->AddSubscription(m_user, sub);
            }
        }
    }

    m_titlePresenceChangedHandlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

void PresenceService::RemoveTitlePresenceChangedHandler(
    _In_ XblFunctionContext context
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto removed{ m_titlePresenceChangedHandlers.erase(context) };
    if (removed && m_titlePresenceChangedHandlers.empty())
    {
        for (auto& xuidPair : m_trackedXuids)
        {
            for (auto& titlePair : xuidPair.second.titlePresenceChangedSubscriptions)
            {
                m_rtaManager->RemoveSubscription(m_user, titlePair.second);
            }
        }
    }
}

XblFunctionContext PresenceService::AddDevicePresenceChangedHandler(
    DevicePresenceChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    // Add subs to RTA manager if needed
    if (m_devicePresenceChangedHandlers.empty())
    {
        for (auto& pair : m_trackedXuids)
        {
            m_rtaManager->AddSubscription(m_user, pair.second.devicePresenceChangedSub);
        }
    }

    m_devicePresenceChangedHandlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

void PresenceService::RemoveDevicePresenceChangedHandler(
    _In_ XblFunctionContext context
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto removed{ m_devicePresenceChangedHandlers.erase(context) };
    if (removed && m_devicePresenceChangedHandlers.empty())
    {
        for (auto& pair : m_trackedXuids)
        {
            m_rtaManager->RemoveSubscription(m_user, pair.second.devicePresenceChangedSub);
        }
    }
}

HRESULT PresenceService::TrackUsers(
    const Vector<uint64_t>& xuids
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    for (auto& xuid : xuids)
    {
        // If we don't have them already, create RTA subscriptions for the new user
        auto iter{ m_trackedXuids.find(xuid) };
        if (iter == m_trackedXuids.end())
        {
            TrackedXuidSubscriptions newSubs{};
            newSubs.refCount = 1;
            newSubs.devicePresenceChangedSub = MakeShared<DevicePresenceChangeSubscription>(xuid, shared_from_this());
            for (auto& pair : m_trackedTitles)
            {
                auto& title{ pair.first };
                newSubs.titlePresenceChangedSubscriptions[title] = MakeShared<TitlePresenceChangeSubscription>(xuid, title, shared_from_this());
            }

            // If there are existing handlers, add the new subs to RTA managers
            if (!m_devicePresenceChangedHandlers.empty())
            {
                RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(m_user, newSubs.devicePresenceChangedSub));
            }
            if (!m_titlePresenceChangedHandlers.empty())
            {
                for (auto& pair : newSubs.titlePresenceChangedSubscriptions) 
                {
                    RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(m_user, pair.second));
                }
            }
            m_trackedXuids[xuid] = std::move(newSubs);
        }
        else
        {
            ++(iter->second.refCount);
        }
    }
    return S_OK;
}

HRESULT PresenceService::StopTrackingUsers(
    const Vector<uint64_t>& xuids
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    for (auto& xuid : xuids)
    {
        auto iter{ m_trackedXuids.find(xuid) };
        if (iter != m_trackedXuids.end() && --(iter->second.refCount) == 0)
        {
            // Remove the subs from RTA manager as necessary
            if (!m_devicePresenceChangedHandlers.empty())
            {
                RETURN_HR_IF_FAILED(m_rtaManager->RemoveSubscription(m_user, iter->second.devicePresenceChangedSub));
            }
            if (!m_titlePresenceChangedHandlers.empty())
            {
                for (auto& pair : iter->second.titlePresenceChangedSubscriptions)
                {
                    RETURN_HR_IF_FAILED(m_rtaManager->RemoveSubscription(m_user, pair.second));
                }
            }
            m_trackedXuids.erase(iter);
        }
    }
    return S_OK;
}

HRESULT PresenceService::TrackAdditionalTitles(
    const Vector<uint32_t>& titleIds
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    for (auto& titleId : titleIds)
    {
        auto iter{ m_trackedTitles.find(titleId) };
        if (iter == m_trackedTitles.end())
        {
            m_trackedTitles[titleId] = 1;

            // If its a new title, create the appropriate subscriptions
            for (auto& pair : m_trackedXuids)
            {
                auto sub{ MakeShared<TitlePresenceChangeSubscription>(pair.first, titleId, shared_from_this()) };
                pair.second.titlePresenceChangedSubscriptions[titleId] = sub;

                // Add new subs to RTA manager if we have handlers
                if (!m_titlePresenceChangedHandlers.empty())
                {
                    RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(m_user, sub));
                }
            }
        }
        else
        {
            ++(iter->second);
        }
    }

    return S_OK;
}

HRESULT PresenceService::StopTrackingAdditionalTitles(
    const Vector<uint32_t>& titleIds
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    List<uint32_t> removedTitles{};
    for (auto& titleId : titleIds)
    {
        // Don't allow removal of the current title
        if (titleId == m_titleId)
        {
            continue;
        }

        auto iter{ m_trackedTitles.find(titleId) };
        if (iter != m_trackedTitles.end() && --(iter->second) == 0)
        {
            for (auto& pair : m_trackedXuids)
            {
                auto& subs{ pair.second.titlePresenceChangedSubscriptions };
                auto subIter{ subs.find(titleId) };
                assert(subIter != subs.end());

                // Remove subs from RTA manager as necessary
                if (!m_titlePresenceChangedHandlers.empty())
                {
                    RETURN_HR_IF_FAILED(m_rtaManager->RemoveSubscription(m_user, subIter->second));
                }
                subs.erase(subIter);
            }
            m_trackedTitles.erase(iter);
        }
    }
    return S_OK;
}

HRESULT PresenceService::SetPresence(
    _In_ TitleRequest&& request,
    _In_ AsyncContext<HRESULT> async
) const noexcept
{

    Stringstream subpath;
    subpath << "/users/xuid(" << m_user.Xuid() << ")/devices/current/titles/current";

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("userpresence", subpath.str()),
        xbox_live_api::set_presence_helper
    ));

    JsonDocument titleRequestJson(rapidjson::kObjectType);
    request.Serialize(titleRequestJson, titleRequestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(titleRequestJson));
    RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTRACT_VERSION_HEADER, "3"));

    return httpCall->Perform({
        async.Queue(),
        [
            async
        ]
    (HttpResult result)
    {
        HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
        if (SUCCEEDED(hr))
        {
            uint32_t heartbeatDelayInMins{ 0 };

            auto heartbeatAfterHeader = result.Payload()->GetResponseHeader("X-Heartbeat-After");
            if (!heartbeatAfterHeader.empty())
            {
                heartbeatDelayInMins = utils::internal_string_to_uint32(heartbeatAfterHeader) / 60;
            }
            else
            {
                heartbeatDelayInMins = 5;
            }
        }
        async.Complete(hr);
    } });
}

HRESULT PresenceService::GetPresence(
    _In_ uint64_t xuid,
    _In_ AsyncContext<Result<std::shared_ptr<XblPresenceRecord>>> async
) const noexcept
{
    Stringstream subpath;
    subpath << "/users/xuid(" << xuid << ")?level=all";

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("userpresence", subpath.str()),
        xbox_live_api::get_presence
    ));

    httpCall->SetHeader(CONTRACT_VERSION_HEADER, "3");

    return httpCall->Perform({
        async.Queue(),
        [
            async
        ]
    (HttpResult result)
    {
        HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
        if (SUCCEEDED(hr))
        {
            return async.Complete(XblPresenceRecord::Deserialize(result.Payload()->GetResponseBodyJson()));
        }
        return async.Complete(hr);
    } });
}

HRESULT PresenceService::GetBatchPresence(
    _In_ UserBatchRequest&& batchRequest,
    _In_ AsyncContext<Result<xsapi_internal_vector<std::shared_ptr<XblPresenceRecord>>>> async
) const noexcept
{
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("userpresence", "/users/batch"),
        xbox_live_api::get_presence_for_multiple_users
    ));

    JsonDocument batchRequestJson(rapidjson::kObjectType);
    batchRequest.Serialize(batchRequestJson, batchRequestJson.GetAllocator());
    httpCall->SetRequestBody(batchRequestJson);
    httpCall->SetHeader(CONTRACT_VERSION_HEADER, "3");

    return httpCall->Perform({
        async.Queue().GetHandle(),
        [
            async
        ]
    (HttpResult result)
    {
        HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
        if (SUCCEEDED(hr))
        {
            return async.Complete(DeserializeBatchPresenceRecordsResponse(result.Payload()->GetResponseBodyJson()));
        }
        return async.Complete(hr);
    } });
}

void PresenceService::HandleDevicePresenceChanged(
    _In_ uint64_t xuid,
    _In_ XblPresenceDeviceType deviceType,
    _In_ bool isUserLoggedOnDevice
) const noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    auto handlers{ m_devicePresenceChangedHandlers };
    lock.unlock();

    for (auto& pair : handlers)
    {
        pair.second(xuid, deviceType, isUserLoggedOnDevice);
    }
}

void PresenceService::HandleTitlePresenceChanged(
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _In_ XblPresenceTitleState state
) const noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    auto handlers{ m_titlePresenceChangedHandlers };
    lock.unlock();

    for (auto& pair : handlers)
    {
        pair.second(xuid, titleId, state);
    }
}

Result<Vector<std::shared_ptr<XblPresenceRecord>>> PresenceService::DeserializeBatchPresenceRecordsResponse(
    const JsonValue& json
) noexcept
{
    Vector<std::shared_ptr<XblPresenceRecord>> presenceRecords;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<std::shared_ptr<XblPresenceRecord>>(
        XblPresenceRecord::Deserialize,
        json,
        presenceRecords
    ));

    return presenceRecords;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END

