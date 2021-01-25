// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <cll/CllPartA.h>
#include <cll/CllTenantSettings.h>
#include "events_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

class Event
{
public:
    Event(
        _In_ uint64_t xuid,
        _In_ const xsapi_internal_string& eventName,
        _In_ const JsonValue& dimensions,
        _In_ const JsonValue& measurements,
        _In_ xbox::services::datetime timestamp = xbox::services::datetime::utc_now()
    );

    Event(const Event& other);

    static Result<Event> Deserialize(
        _In_ const xsapi_internal_string& inputData
    );

    const std::string& Data() const;
    const std::string& FullEventName() const;
    const xbox::services::datetime& Timestamp() const;

    xsapi_internal_string Serialize() const;

private:
    Event() = default;

    std::string SerializeFieldValue(
        _In_ const JsonValue& value
    );

    uint64_t m_xuid{ 0 };
    xsapi_internal_string m_eventName;
    std::string m_fullEventName;
    JsonDocument m_dimensions;
    JsonDocument m_measurements;
    xbox::services::datetime m_timestamp;
    std::string m_data;
};

class EventUploadPayload : public std::enable_shared_from_this<EventUploadPayload>
{
public:
    EventUploadPayload(
        _In_ User&& user,
        _In_ std::shared_ptr<cll::CllTenantSettings> tenantSettings
    );

    HRESULT AddEvent(_In_ const Event& event);
    size_t EventCount() const;

    struct RequestData
    {
        xsapi_internal_http_headers headers;
        xsapi_internal_string requestBody;
    };

    HRESULT GetRequestData(_In_ AsyncContext<Result<const RequestData&>> async);

    size_t ExtractEventsAndSerialize(
        _Inout_ xsapi_internal_vector<uint8_t>& serializedData,
        _In_ size_t targetDataSize,
        _Inout_ xbox::services::datetime& timestamp
    );

private:
    HRESULT CreateTicketData(
        _In_ AsyncContext<Result<std::vector<cll::TicketData>>> async
    );

    User m_user;
    std::shared_ptr<cll::CllTenantSettings> m_tenantSettings;
    cll::CllUploadRequestData m_cllRequestData;
    RequestData m_requestData;
    xsapi_internal_list<Event> m_events;
};

enum class Mode
{
    Normal,
    Offline
};

// Class to manage pending events
class EventQueue : public std::enable_shared_from_this<EventQueue>
{
public:
    EventQueue(
        User&& user,
        std::shared_ptr<cll::CllTenantSettings> tenantSettings
    );

    void Initialize();
    void Cleanup();

    HRESULT AddEvent(Event&& event);
    HRESULT AddEvents(xsapi_internal_vector<Event>&& events);
    std::shared_ptr<EventUploadPayload> GetNextPayload(size_t minimumEventCount = 1);
    void SetMode(Mode mode);
    void RequeueFailedPayload(std::shared_ptr<EventUploadPayload> failedPayload);

    static HRESULT SetMaxFileSize(uint64_t fileSizeInBytes);
    static HRESULT SetStorageAllotment(uint64_t storageAllotmentInBytes);

private:
    HRESULT WriteDirectoryFile();
    HRESULT Populate();
    HRESULT Flush();

    std::mutex m_mutex;
    std::atomic<Mode> m_mode{ Mode::Normal };

    User m_user;
    std::shared_ptr<cll::CllTenantSettings> m_tenantSettings;

    xsapi_internal_string const m_filenamePrefix{ "XblEvents" };
    xsapi_internal_string m_directoryFilename;

    xsapi_internal_list<std::shared_ptr<EventUploadPayload>> m_queue;
    std::shared_ptr<EventUploadPayload> m_failedPayload;

    // flush metadata
    Map<String, uint64_t> m_fileMetadata;
    uint64_t m_totalFilesSize{ 0 };
    bool m_flushInProgress{ false };

    std::shared_ptr<system::LocalStorage> m_localStorage;

    static uint64_t m_maxFileSize;
    static uint64_t m_storageAllotment;
};

class EventsService : public IEventsService, public std::enable_shared_from_this<EventsService>
{
public:
    EventsService(
        _In_ User user,
        _In_ const TaskQueue& queue
    );
    ~EventsService();

    HRESULT Initialize();

    HRESULT WriteInGameEvent(
        _In_z_ const char* eventName,
        _In_opt_z_ const char* dimensions,
        _In_opt_z_ const char* measurements
    );

    static const std::string& IKey();

private:
    HRESULT WriteInGameEventHelper(
        _In_ const xsapi_internal_string& eventName,
        _In_ const JsonValue& dimensions,
        _In_ const JsonValue& measurement
    );

    User m_user;
    TaskQueue m_queue;

    void InitializeTenantSettings();

    HRESULT ScheduleUpload() noexcept;
    void UploadAsync(AsyncContext<> async) noexcept;

    HRESULT UploadEventPayload(
        std::shared_ptr<EventUploadPayload> payload,
        AsyncContext<HRESULT> async
    );

    void OnFailedPayload(std::shared_ptr<EventUploadPayload> failedPayload);

    uint64_t m_minimumUploadIntervalInMs{ 1000 };
    uint64_t m_maximumUploadIntervalInMs{ 5000 };
    size_t m_payloadMinimumEventCount{ 1 };
    uint32_t m_numRetryAttempts{ 0 };
    xsapi_internal_string m_eventUploadUrl{ "https://vortex.data.microsoft.com/collect/v1" };
    uint32_t m_uploadTimeoutInSeconds{ 5 };

    chrono_clock_t::time_point m_lastUploadAttempt{ chrono_clock_t::now() };

    std::shared_ptr<EventQueue> m_eventQueue;

    std::shared_ptr<cll::CllTenantSettings> m_tenantSettings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END
