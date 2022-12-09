// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/presence_c.h"
#include "real_time_activity_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

class UserBatchRequest
{
public:
    UserBatchRequest(
        _In_ uint64_t* xuids,
        _In_ size_t xuidsCount,
        _In_opt_ XblPresenceQueryFilters* filters
    ) noexcept;

    UserBatchRequest(
        _In_ String&& socialGroup,
        _In_opt_ uint64_t* socialGroupOwnerXuid,
        _In_opt_ XblPresenceQueryFilters* filters
    ) noexcept;

    static String StringFromDetailLevel(
        _In_ XblPresenceDetailLevel level
    );

    void Serialize(_Out_ JsonValue& serializedObject, _In_ JsonDocument::AllocatorType& allocator) const;

private:
    UserBatchRequest(_In_opt_ XblPresenceQueryFilters* filters) noexcept;

    Vector<String> m_xuids;
    String m_socialGroup;
    String m_socialGroupOwnerXuid;
    Vector<String> m_deviceTypes;
    Vector<String> m_titleIds;
    XblPresenceDetailLevel m_presenceDetailLevel{ XblPresenceDetailLevel::Default };
    bool m_onlineOnly{ false };
    bool m_broadcastingOnly{ false };
};

class TitleRequest
{
public:
    TitleRequest(
        _In_ bool isUserActive,
        _In_opt_ const XblPresenceRichPresenceIds* richPresenceIds
    );

    void Serialize(_Out_ JsonValue& serializedObject, _In_ JsonDocument::AllocatorType& allocator);

private:
    bool m_isUserActive;
    String m_scid;
    String m_presenceId;
    Vector<String> m_presenceTokenIds;
};

class PresenceService;

class TitlePresenceChangeSubscription : public real_time_activity::Subscription, public std::enable_shared_from_this<TitlePresenceChangeSubscription>
{
public:
    TitlePresenceChangeSubscription(
        _In_ uint64_t xuid,
        _In_ uint32_t titleId,
        _In_ std::shared_ptr<PresenceService> presenceService
    ) noexcept;

protected:
    void OnSubscribe(const JsonValue& data) noexcept override;
    void OnEvent(const JsonValue& event) noexcept override;

private:
    uint64_t m_xuid;
    uint32_t m_titleId;
    std::weak_ptr<PresenceService> m_presenceService;
};

class DevicePresenceChangeSubscription : public real_time_activity::Subscription, public std::enable_shared_from_this<DevicePresenceChangeSubscription>
{
public:
    DevicePresenceChangeSubscription(
        _In_ uint64_t xuid,
        _In_ std::shared_ptr<PresenceService> presenceService
    ) noexcept;

protected:
    void OnSubscribe(const JsonValue& data) noexcept override;
    void OnEvent(_In_ const JsonValue& event) noexcept override;

private:
    uint64_t m_xuid;
    std::weak_ptr<class PresenceService> m_presenceService;
};

class DeviceRecord
{
public:
    DeviceRecord() = default;
    DeviceRecord(const DeviceRecord& other);
    DeviceRecord& operator=(DeviceRecord other);
    ~DeviceRecord();

    XblPresenceDeviceType DeviceType() const;

    const Vector<XblPresenceTitleRecord>& TitleRecords() const;

    static Result<std::shared_ptr<DeviceRecord>> Deserialize(_In_ const JsonValue& json);

    static String DeviceTypeAsString(_In_ XblPresenceDeviceType deviceType);
    static XblPresenceDeviceType DeviceTypeFromString(_In_ const String& value);

private:
    static Result<XblPresenceTitleRecord> DeserializeTitleRecord(_In_ const JsonValue& json);
    static Result<XblPresenceBroadcastRecord> DeserializeBroadcastRecord(_In_ const JsonValue& json);

    static XblPresenceTitleViewState TitleViewStateFromString(_In_ const String& viewState);
    static XblPresenceBroadcastProvider BroadcastProviderFromString(_In_ const String& provider);

    XblPresenceDeviceType m_deviceType{ XblPresenceDeviceType::Unknown };
    Vector<XblPresenceTitleRecord> m_titleRecords;
};

namespace legacy
{
    // To support legacy RTA path, subscriptions require some extra state to be tracked
    struct Subscription;
    struct TitleSubscription;
}

class PresenceService : public std::enable_shared_from_this<PresenceService>
{
public:
    PresenceService(
        _In_ User&& user,
        _In_ const TaskQueue& backgroundQueue,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    ~PresenceService() noexcept;

    typedef Function<void(uint64_t xuid, uint32_t titleId, XblPresenceTitleState state)> TitlePresenceChangedHandler;

    XblFunctionContext AddTitlePresenceChangedHandler(
        TitlePresenceChangedHandler handler
    ) noexcept;

    void RemoveTitlePresenceChangedHandler(
        _In_ XblFunctionContext context
    ) noexcept;

    typedef Function<void(uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)> DevicePresenceChangedHandler;

    XblFunctionContext AddDevicePresenceChangedHandler(
        DevicePresenceChangedHandler handler
    ) noexcept;

    void RemoveDevicePresenceChangedHandler(
        _In_ XblFunctionContext context
    ) noexcept;

    HRESULT TrackUsers(
        const Vector<uint64_t>& xuids
    ) noexcept;

    HRESULT StopTrackingUsers(
        const Vector<uint64_t>& xuids
    ) noexcept;

    HRESULT TrackAdditionalTitles(
        const Vector<uint32_t>& titleIds
    ) noexcept;

    HRESULT StopTrackingAdditionalTitles(
        const Vector<uint32_t>& titleIds
    ) noexcept;

    HRESULT SetPresence(
        _In_ TitleRequest&& titleRequest,
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

    HRESULT GetPresence(
        _In_ uint64_t xuid,
        _In_ AsyncContext<Result<std::shared_ptr<XblPresenceRecord>>> async
    ) const noexcept;

    HRESULT GetBatchPresence(
        _In_ UserBatchRequest&& batchRequest,
        _In_ AsyncContext<Result<Vector<std::shared_ptr<XblPresenceRecord>>>> async
    ) const noexcept;

private:
    void HandleDevicePresenceChanged(
        _In_ uint64_t xuid,
        _In_ XblPresenceDeviceType deviceType,
        _In_ bool isUserLoggedOnDevice
    ) const noexcept;

    void HandleTitlePresenceChanged(
        _In_ uint64_t xuid,
        _In_ uint32_t titleId,
        _In_ XblPresenceTitleState state
    ) const noexcept;

    void HandleRTAResync();

    static Result<Vector<std::shared_ptr<XblPresenceRecord>>> DeserializeBatchPresenceRecordsResponse(
        const JsonValue& json
    ) noexcept;

    User m_user;
    TaskQueue m_queue;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;

    XblFunctionContext m_resyncHandlerToken{ 0 };
    Map<XblFunctionContext, DevicePresenceChangedHandler> m_devicePresenceChangedHandlers;
    Map<XblFunctionContext, TitlePresenceChangedHandler> m_titlePresenceChangedHandlers;
    XblFunctionContext m_nextHandlerToken{ 1 };

    struct TrackedXuidSubscriptions
    {
        size_t refCount{ 0 };
        std::shared_ptr<DevicePresenceChangeSubscription> devicePresenceChangedSub;
        Map<uint32_t, std::shared_ptr<TitlePresenceChangeSubscription>> titlePresenceChangedSubscriptions;
    };

    Map<uint64_t, TrackedXuidSubscriptions> m_trackedXuids;
    Map<uint32_t, size_t> m_trackedTitles;
    uint32_t const m_titleId;

    mutable std::recursive_mutex m_mutex;

    friend class DevicePresenceChangeSubscription;
    friend class TitlePresenceChangeSubscription;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END

struct XblPresenceRecord : public xbox::services::RefCounter, public std::enable_shared_from_this<XblPresenceRecord>
{
public:
    XblPresenceRecord() = default;

    uint64_t Xuid() const;

    XblPresenceUserState UserState() const;

    const Vector<XblPresenceDeviceRecord>& DeviceRecords() const;

    bool IsUserPlayingTitle(_In_ uint32_t titleId) const;

    static xbox::services::Result<std::shared_ptr<XblPresenceRecord>> Deserialize(_In_ const JsonValue& json);

    static XblPresenceUserState UserStateFromString(_In_ const xbox::services::String& value);

protected:
    // RefCounter overrides
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;

private:
    uint64_t m_xuid{};
    XblPresenceUserState m_userState{ XblPresenceUserState::Unknown };

    Vector<XblPresenceDeviceRecord> m_deviceRecords;
    Vector<std::shared_ptr<xbox::services::presence::DeviceRecord>> m_deviceRecordsInternal;
};