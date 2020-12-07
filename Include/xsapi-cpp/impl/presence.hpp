// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_data::presence_data(
    _In_ string_t serviceConfigurationId,
    _In_ string_t presenceId
) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_presenceId(std::move(presenceId))
{
}

presence_data::presence_data(
    _In_ string_t serviceConfigurationId,
    _In_ string_t presenceId,
    _In_ std::vector<string_t> presenceTokenIds
) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_presenceId(std::move(presenceId)),
    m_presenceTokenIds(std::move(presenceTokenIds))
{
}

const string_t& presence_data::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t& presence_data::presence_id() const
{
    return m_presenceId;
}

const std::vector<string_t>& presence_data::presence_token_ids() const
{
    return m_presenceTokenIds;
}

presence_broadcast_record::presence_broadcast_record(
    _In_ XblPresenceRecordHandle handle,
    _In_ const XblPresenceBroadcastRecord* broadcastRecord
) :
    m_broadcastRecord{ broadcastRecord }
{
    XblPresenceRecordDuplicateHandle(handle, &m_handle);
}

presence_broadcast_record::presence_broadcast_record(
    const presence_broadcast_record& other
) :
    m_broadcastRecord{ other.m_broadcastRecord }
{
    XblPresenceRecordDuplicateHandle(other.m_handle, &m_handle);
}

presence_broadcast_record& presence_broadcast_record::operator=(
    presence_broadcast_record other
)
{
    std::swap(m_handle, other.m_handle);
    m_broadcastRecord = other.m_broadcastRecord;
    return *this;
}

presence_broadcast_record::~presence_broadcast_record()
{
    if (m_handle)
    {
        XblPresenceRecordCloseHandle(m_handle);
    }
}

string_t presence_broadcast_record::broadcast_id() const
{
    if (m_broadcastRecord)
    {
        return Utils::StringTFromUtf8(m_broadcastRecord->broadcastId);
    }
    return string_t();
}

string_t presence_broadcast_record::session() const
{
    if (m_broadcastRecord)
    {
        return Utils::StringTFromUtf8(m_broadcastRecord->session);
    }
    return string_t();
}

string_t presence_broadcast_record::provider() const
{
    if (m_broadcastRecord)
    {
        switch (m_broadcastRecord->provider)
        {
        case XblPresenceBroadcastProvider::Twitch:
            return _T("twitch");
        default:
            return _T("unknown");
        }
    }
    return _T("unknown");
}

uint32_t presence_broadcast_record::viewer_count() const
{
    if (m_broadcastRecord)
    {
        return m_broadcastRecord->viewerCount;
    }
    return 0;
}

utility::datetime presence_broadcast_record::start_time() const
{
    if (m_broadcastRecord)
    {
        return Utils::DatetimeFromTimeT(m_broadcastRecord->startTime);
    }
    return utility::datetime();
}

presence_title_record::presence_title_record(
    _In_ XblPresenceRecordHandle handle,
    _In_ const XblPresenceTitleRecord* titleRecord
) :
    m_titleRecord(titleRecord)
{
    XblPresenceRecordDuplicateHandle(handle, &m_handle);
}

presence_title_record::presence_title_record(
    _In_ const presence_title_record& other
) :
    m_titleRecord(other.m_titleRecord)
{
    XblPresenceRecordDuplicateHandle(other.m_handle, &m_handle);
}

presence_title_record& presence_title_record::operator=(
    _In_ presence_title_record other
)
{
    std::swap(m_handle, other.m_handle);
    m_titleRecord = other.m_titleRecord;
    return *this;
}

presence_title_record::~presence_title_record()
{
    XblPresenceRecordCloseHandle(m_handle);
}

uint32_t presence_title_record::title_id() const
{
    return m_titleRecord->titleId;
}

string_t presence_title_record::title_name() const
{
    return Utils::StringTFromUtf8(m_titleRecord->titleName);
}

utility::datetime presence_title_record::last_modified_date() const
{
    return Utils::DatetimeFromTimeT(m_titleRecord->lastModified);
}

bool presence_title_record::is_title_active() const
{
    return m_titleRecord->titleActive;
}

string_t presence_title_record::presence() const
{
    return Utils::StringTFromUtf8(m_titleRecord->richPresenceString);
}

presence_title_view_state presence_title_record::presence_title_view() const
{
    return static_cast<presence_title_view_state>(m_titleRecord->viewState);
}

presence_broadcast_record presence_title_record::broadcast_record() const
{
    if (m_titleRecord->broadcastRecord)
    {
        return presence_broadcast_record(m_handle, m_titleRecord->broadcastRecord);
    }
    else
    {
        return presence_broadcast_record();
    }
}

presence_device_record::presence_device_record(
    _In_ XblPresenceRecordHandle handle,
    _In_ const XblPresenceDeviceRecord* deviceRecord
) :
    m_deviceRecord(deviceRecord)
{
    XblPresenceRecordDuplicateHandle(handle, &m_handle);
}

presence_device_record::presence_device_record(
    _In_ const presence_device_record& other
) :
    m_deviceRecord(other.m_deviceRecord)
{
    XblPresenceRecordDuplicateHandle(other.m_handle, &m_handle);
}

presence_device_record& presence_device_record::operator=(
    _In_ presence_device_record other
)
{
    std::swap(m_handle, other.m_handle);
    m_deviceRecord = other.m_deviceRecord;
    return *this;
}

presence_device_record::~presence_device_record()
{
    XblPresenceRecordCloseHandle(m_handle);
}

presence_device_type presence_device_record::device_type() const
{
    return static_cast<presence_device_type>(m_deviceRecord->deviceType);
}

std::vector<presence_title_record> presence_device_record::presence_title_records() const
{
    return Utils::Transform<presence_title_record>(m_deviceRecord->titleRecords, m_deviceRecord->titleRecordsCount, 
        [this](const XblPresenceTitleRecord& titleRecord)
    {
        return presence_title_record(m_handle, &titleRecord);
    });
}

presence_record::presence_record(
    _In_ XblPresenceRecordHandle handle
)
{
    XblPresenceRecordDuplicateHandle(handle, &m_handle);
}

presence_record::presence_record(
    _In_ const presence_record& other
)
{
    XblPresenceRecordDuplicateHandle(other.m_handle, &m_handle);
}

presence_record& presence_record::operator=(
    presence_record other
)
{
    std::swap(m_handle, other.m_handle);
    return *this;
}

presence_record::~presence_record()
{
    if (m_handle)
    {
        XblPresenceRecordCloseHandle(m_handle);
    }
}

string_t presence_record::xbox_user_id() const
{
    uint64_t xuid;
    XblPresenceRecordGetXuid(m_handle, &xuid);
    return Utils::StringTFromUint64(xuid);
}

user_presence_state presence_record::user_state() const
{
    XblPresenceUserState state;
    XblPresenceRecordGetUserState(m_handle, &state);
    return static_cast<user_presence_state>(state);
}

std::vector<presence_device_record> presence_record::presence_device_records() const
{
    const XblPresenceDeviceRecord* deviceRecords;
    size_t deviceRecordsCount;
    XblPresenceRecordGetDeviceRecords(m_handle, &deviceRecords, &deviceRecordsCount);

    return Utils::Transform<presence_device_record>(deviceRecords, deviceRecordsCount, 
        [this](const XblPresenceDeviceRecord& deviceRecord)
    {
        return presence_device_record(m_handle, &deviceRecord);
    });
}

bool presence_record::is_user_playing_title(_In_ uint32_t titleId) const
{
    auto userState = user_state();
    if (userState == user_presence_state::offline || userState == user_presence_state::unknown)
    {
        return false;
    }

    auto deviceRecords = presence_device_records();
    for (const auto& deviceRecord : deviceRecords)
    {
        auto titleRecords = deviceRecord.presence_title_records();
        for (const auto& titleRecord : titleRecords)
        {
            if (titleRecord.title_id() == titleId)
            {
                return titleRecord.is_title_active();
            }
        }
    }
    return false;
}

device_presence_change_event_args::device_presence_change_event_args(
    _In_ uint64_t xuid,
    _In_ XblPresenceDeviceType deviceType,
    _In_ bool isUserLoggedOn
) :
    m_deviceType(deviceType),
    m_isUserLoggedOn(isUserLoggedOn)
{
    m_xuid = Utils::StringTFromUint64(xuid);
}

const string_t& device_presence_change_event_args::xbox_user_id() const
{
    return m_xuid;
}

presence_device_type device_presence_change_event_args::device_type() const
{
    return static_cast<presence_device_type>(m_deviceType);
}

bool device_presence_change_event_args::is_user_logged_on_device() const
{
    return m_isUserLoggedOn;
}

device_presence_change_subscription::device_presence_change_subscription(
    _In_ XblRealTimeActivitySubscriptionHandle handle,
    _In_ const string_t& xuid
) :
    real_time_activity_subscription(handle),
    m_xuid(xuid)
{
    stringstream_t uri;
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << m_xuid << _T(")/devices");
    m_resourceUri = uri.str();
}

const string_t& device_presence_change_subscription::xbox_user_id() const
{
    return m_xuid;
}

title_presence_change_event_args::title_presence_change_event_args(
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _In_ XblPresenceTitleState titleState
) :
    m_titleId(titleId),
    m_titleState(titleState)
{
    m_xuid = Utils::StringTFromUint64(xuid);
}

const string_t& title_presence_change_event_args::xbox_user_id() const
{
    return m_xuid;
}

uint32_t title_presence_change_event_args::title_id() const
{
    return m_titleId;
}

title_presence_state title_presence_change_event_args::title_state() const
{
    return static_cast<title_presence_state>(m_titleState);
}

title_presence_change_subscription::title_presence_change_subscription(
    _In_ XblRealTimeActivitySubscriptionHandle handle,
    _In_ const string_t& xuid,
    _In_ uint32_t titleId
) :
    real_time_activity_subscription(handle),
    m_xuid(xuid),
    m_titleId(titleId)
{
    stringstream_t uri;
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << m_xuid << _T(")/titles/") << m_titleId;
    m_resourceUri = uri.str();
}

const string_t& title_presence_change_subscription::xbox_user_id() const
{
    return m_xuid;
}

uint32_t title_presence_change_subscription::title_id() const
{
    return m_titleId;
}

pplx::task<xbox_live_result<void>> presence_service::set_presence(
    _In_ bool isUserActiveInTitle
)
{
    auto asyncWrapper = new AsyncWrapper<void>();

    auto hr = XblPresenceSetPresenceAsync(
        m_xblContextHandle,
        isUserActiveInTitle,
        nullptr,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<void>> presence_service::set_presence(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data presenceData
)
{
    auto asyncWrapper = new AsyncWrapper<void>();

    XblPresenceRichPresenceIds ids{};
    Utils::Utf8FromCharT(presenceData.service_configuration_id().data(), ids.scid, sizeof(ids.scid));

    std::string presenceId = Utils::StringFromStringT(presenceData.presence_id());
    ids.presenceId = presenceId.data();

    UTF8StringArrayRef presenceTokenIds{ presenceData.presence_token_ids() };
    ids.presenceTokenIds = presenceTokenIds.Data();
    ids.presenceTokenIdsCount = presenceTokenIds.Size();


    auto hr = XblPresenceSetPresenceAsync(
        m_xblContextHandle,
        isUserActiveInTitle,
        &ids,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<presence_record>> presence_service::get_presence(
    _In_ const string_t& xboxUserId
)
{
    auto asyncWrapper = new AsyncWrapper<presence_record>(
        [](XAsyncBlock* async, presence_record& presenceRecord)
    {
        XblPresenceRecordHandle handle;
        auto hr = XblPresenceGetPresenceResult(async, &handle);
        if (SUCCEEDED(hr))
        {
            presenceRecord = presence_record(handle);
            XblPresenceRecordCloseHandle(handle);
        }
        return hr;
    });

    auto hr = XblPresenceGetPresenceAsync(
        m_xblContextHandle,
        Utils::Uint64FromStringT(xboxUserId),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<presence_record>>> presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds
)
{
    return get_presence_for_multiple_users(
        xboxUserIds,
        {},
        {},
        presence_detail_level::default_level,
        false,
        false
    );
}

pplx::task<xbox_live_result<std::vector<presence_record>>> presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<presence_record>>(
        [](XAsyncBlock* async, std::vector<presence_record>& presenceRecords)
    {
        size_t resultCount;
        auto hr = XblPresenceGetPresenceForMultipleUsersResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            std::vector<XblPresenceRecordHandle> recordHandles(resultCount);
            hr = XblPresenceGetPresenceForMultipleUsersResult(async, recordHandles.data(), resultCount);

            if (SUCCEEDED(hr))
            {
                presenceRecords = Utils::Transform<presence_record>(recordHandles, [](XblPresenceRecordHandle recordHandle)
                {
                    presence_record record(recordHandle);
                    XblPresenceRecordCloseHandle(recordHandle);
                    return record;
                });
            }
        }
        return hr;
    });

    XblPresenceQueryFilters filters{};
    if (!deviceTypes.empty())
    {
        filters.deviceTypes = (XblPresenceDeviceType*)(deviceTypes.data());
        filters.deviceTypesCount = deviceTypes.size();
    }
    if (!titleIds.empty())
    {
        filters.titleIds = titleIds.data();
        filters.titleIdsCount = titleIds.size();
    }
    filters.detailLevel = static_cast<XblPresenceDetailLevel>(presenceDetailLevel);
    filters.broadcastingOnly = broadcastingOnly;
    filters.onlineOnly = onlineOnly;

    auto xuids = Utils::XuidVectorFromXuidStringVector(xboxUserIds);

    auto hr = XblPresenceGetPresenceForMultipleUsersAsync(
        m_xblContextHandle,
        xuids.data(),
        xuids.size(),
        &filters,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<presence_record>>> presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup
)
{
    return get_presence_for_social_group(
        socialGroup,
        string_t(),
        {},
        {},
        presence_detail_level::default_level,
        false,
        false
    );
}

pplx::task<xbox_live_result<std::vector<presence_record>>> presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level peoplehubDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<presence_record>>(
        [](XAsyncBlock* async, std::vector<presence_record>& presenceRecords)
    {
        size_t resultCount;
        auto hr = XblPresenceGetPresenceForSocialGroupResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            std::vector<XblPresenceRecordHandle> recordHandles(resultCount);
            hr = XblPresenceGetPresenceForSocialGroupResult(async, recordHandles.data(), resultCount);

            if (SUCCEEDED(hr))
            {
                presenceRecords = Utils::Transform<presence_record>(recordHandles, [](XblPresenceRecordHandle recordHandle)
                {
                    presence_record record(recordHandle);
                    XblPresenceRecordCloseHandle(recordHandle);
                    return record;
                });
            }
        }
        return hr;
    });

    XblPresenceQueryFilters filters{};
    if (!deviceTypes.empty())
    {
        filters.deviceTypes = (XblPresenceDeviceType*)(deviceTypes.data());
        filters.deviceTypesCount = deviceTypes.size();
    }
    if (!titleIds.empty())
    {
        filters.titleIds = titleIds.data();
        filters.titleIdsCount = titleIds.size();
    }
    filters.detailLevel = static_cast<XblPresenceDetailLevel>(peoplehubDetailLevel);
    filters.broadcastingOnly = broadcastingOnly;
    filters.onlineOnly = onlineOnly;

    uint64_t groupOwnerXuid = 0;
    if (!socialGroupOwnerXboxUserId.empty())
    {
        groupOwnerXuid = Utils::Uint64FromStringT(socialGroupOwnerXboxUserId);
    }

    auto hr = XblPresenceGetPresenceForSocialGroupAsync(
        m_xblContextHandle,
        Utils::StringFromStringT(socialGroup).data(),
        groupOwnerXuid ? &groupOwnerXuid : nullptr,
        &filters,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

xbox_live_result<std::shared_ptr<device_presence_change_subscription>> presence_service::subscribe_to_device_presence_change(
    _In_ const string_t& xboxUserId
)
{
    XblRealTimeActivitySubscriptionHandle subHandle{};
    auto hr = XblPresenceSubscribeToDevicePresenceChange(
        m_xblContextHandle,
        Utils::Uint64FromStringT(xboxUserId),
        &subHandle
    );

    if (FAILED(hr))
    {
        return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(Utils::ConvertHr(hr));
    }
    return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(std::make_shared<device_presence_change_subscription>(subHandle, xboxUserId));
}

xbox_live_result<void> presence_service::unsubscribe_from_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_subscription> subscription
)
{
    return Utils::ConvertHr(XblPresenceUnsubscribeFromDevicePresenceChange(m_xblContextHandle, subscription->m_handle));
}

xbox_live_result<std::shared_ptr<title_presence_change_subscription>> presence_service::subscribe_to_title_presence_change(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId
)
{
    XblRealTimeActivitySubscriptionHandle subHandle{};
    auto hr = XblPresenceSubscribeToTitlePresenceChange(
        m_xblContextHandle,
        Utils::Uint64FromStringT(xboxUserId),
        titleId,
        &subHandle
    );

    if (FAILED(hr))
    {
        return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(Utils::ConvertHr(hr));
    }
    return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(std::make_shared<title_presence_change_subscription>(subHandle, xboxUserId, titleId));
}

xbox_live_result<void> presence_service::unsubscribe_from_title_presence_change(
    _In_ std::shared_ptr<title_presence_change_subscription> subscription
)
{
    return Utils::ConvertHr(XblPresenceUnsubscribeFromTitlePresenceChange(m_xblContextHandle, subscription->m_handle));
}

struct presence_service::HandlerContext
{
    XblFunctionContext internalContext;
    std::function<void(const device_presence_change_event_args&)> devicePresenceChangedHandler;
    std::function<void(const title_presence_change_event_args&)> titlePresenceChangedHandler;
};

function_context presence_service::add_device_presence_changed_handler(
    _In_ std::function<void(const device_presence_change_event_args&)> handler
)
{
    auto context = new HandlerContext{};
    context->devicePresenceChangedHandler = std::move(handler);

    context->internalContext = XblPresenceAddDevicePresenceChangedHandler(m_xblContextHandle,
        [](void* context, uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->devicePresenceChangedHandler(device_presence_change_event_args{ xuid, deviceType, isUserLoggedOnDevice });
    }, context);

    return context;
}

void presence_service::remove_device_presence_changed_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblPresenceRemoveDevicePresenceChangedHandler(m_xblContextHandle, handlerContext->internalContext);
    delete handlerContext;
}

function_context presence_service::add_title_presence_changed_handler(
    _In_ std::function<void(const title_presence_change_event_args&)> handler
)
{
    auto context = new HandlerContext{};
    context->titlePresenceChangedHandler = std::move(handler);

    context->internalContext = XblPresenceAddTitlePresenceChangedHandler(m_xblContextHandle,
        [](void* context, uint64_t xuid, uint32_t titleId, XblPresenceTitleState titleState)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->titlePresenceChangedHandler(title_presence_change_event_args{ xuid, titleId, titleState });
    }, context);

    return context;
}

void presence_service::remove_title_presence_changed_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblPresenceRemoveTitlePresenceChangedHandler(m_xblContextHandle, handlerContext->internalContext);
    delete handlerContext;
}

presence_service::presence_service(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

presence_service::presence_service(const presence_service& other)
{
    XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
}

presence_service& presence_service::operator=(presence_service other)
{
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    return *this;
}

presence_service::~presence_service()
{
    XblContextCloseHandle(m_xblContextHandle);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END

XBL_WARNING_POP