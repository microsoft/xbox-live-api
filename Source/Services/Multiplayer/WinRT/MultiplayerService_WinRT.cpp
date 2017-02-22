// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerService_WinRT.h"
#include "Utils_WinRT.h"
#include "user_context.h"
#include "utils.h"
#include "MultiplayerSessionWriteMode_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::multiplayer;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerService::MultiplayerService(
    _In_ multiplayer_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_sessionChangeEventBind = std::make_shared<MultiplayerSessionChangeEventBind>(Platform::WeakReference(this), m_cppObj);
    m_subscriptionLostEventBind = std::make_shared<MultiplayerSubscriptionLostEventBind>(Platform::WeakReference(this), m_cppObj);

    m_sessionChangeEventBind->AddMultiplayerSessionChangeEvent();
    m_subscriptionLostEventBind->AddMultiplayerSubscriptionLostEvent();
}

MultiplayerService::~MultiplayerService()
{
    m_sessionChangeEventBind->RemoveMultiplayerSessionChangeEvent(m_cppObj);
    m_subscriptionLostEventBind->RemoveMultiplayerSubscriptionLostEvent(m_cppObj);
}

MultiplayerSessionChangeEventBind::MultiplayerSessionChangeEventBind(
    _In_ Platform::WeakReference setting,
    _In_ multiplayer_service& cppObj
    ) : 
    m_setting(setting),
    m_cppObj(cppObj),
    m_functionContext(0)
{
}

void
MultiplayerSessionChangeEventBind::RemoveMultiplayerSessionChangeEvent(
    _In_ xbox::services::multiplayer::multiplayer_service& cppObj
    )
{
    cppObj.remove_multiplayer_session_changed_handler(m_functionContext);
}

void
MultiplayerSessionChangeEventBind::MultiplayerSessionChangeRouter(
    _In_ const multiplayer_session_change_event_args& sessionChangeEventArgs
    )
{
    MultiplayerService^ multiplayerService = m_setting.Resolve<MultiplayerService>();

    if (multiplayerService != nullptr)
    {
#if !TV_API
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([multiplayerService, sessionChangeEventArgs]()
            {
                multiplayerService->RaiseMultiplayerSessionChange(ref new MultiplayerSessionChangeEventArgs(sessionChangeEventArgs));
            }));
        }
        else
#endif
        {
            multiplayerService->RaiseMultiplayerSessionChange(ref new MultiplayerSessionChangeEventArgs(sessionChangeEventArgs));
        }
    }
}

void MultiplayerSessionChangeEventBind::AddMultiplayerSessionChangeEvent()
{
    std::weak_ptr<MultiplayerSessionChangeEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_multiplayer_session_changed_handler([thisWeakPtr](_In_ const multiplayer_session_change_event_args& sessionChangeEventArgs)
    {
        std::shared_ptr<MultiplayerSessionChangeEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->MultiplayerSessionChangeRouter(sessionChangeEventArgs);
        }
    });
}

MultiplayerSubscriptionLostEventBind::MultiplayerSubscriptionLostEventBind(
    _In_ Platform::WeakReference setting,
    _In_ multiplayer_service& cppObj
    ) : 
    m_setting(setting),
    m_cppObj(cppObj),
    m_functionContext(0)
{
}

void
MultiplayerSubscriptionLostEventBind::RemoveMultiplayerSubscriptionLostEvent(
    _In_ xbox::services::multiplayer::multiplayer_service& cppObj
    )
{
    cppObj.remove_multiplayer_subscription_lost_handler(m_functionContext);
}

void
MultiplayerSubscriptionLostEventBind::MultiplayerSubscriptionLostRouter()
{
    MultiplayerService^ multiplayerService = m_setting.Resolve<MultiplayerService>();
    if (multiplayerService != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([multiplayerService]()
            {
                multiplayerService->RaiseMultiplayerSubscriptionLost(ref new MultiplayerSubscriptionLostEventArgs());
            }));
        }
        else
        {
            multiplayerService->RaiseMultiplayerSubscriptionLost(ref new MultiplayerSubscriptionLostEventArgs());
        }
    }
}

void MultiplayerSubscriptionLostEventBind::AddMultiplayerSubscriptionLostEvent()
{
    std::weak_ptr<MultiplayerSubscriptionLostEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_multiplayer_subscription_lost_handler([thisWeakPtr](void)
    {
        std::shared_ptr<MultiplayerSubscriptionLostEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->MultiplayerSubscriptionLostRouter();
        }
    });
}

void
MultiplayerService::RaiseMultiplayerSessionChange(
    _In_ MultiplayerSessionChangeEventArgs^ args
    )
{
    MultiplayerSessionChanged(this, args);
}

void
MultiplayerService::RaiseMultiplayerSubscriptionLost(
    _In_ MultiplayerSubscriptionLostEventArgs^ args
    )
{
    MultiplayerSubscriptionLost(this, args);
}

IAsyncOperation<MultiplayerSession^>^ 
MultiplayerService::WriteSessionAsync(
    _In_ MultiplayerSession^ multiplayerSession,
    _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(multiplayerSession);

    auto task = m_cppObj.write_session(
        multiplayerSession->GetCppObj(),
        static_cast<multiplayer_session_write_mode>(multiplayerSessionWriteMode)
        )
    .then([](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        THROW_IF_ERR(multiplayerSession);

        MultiplayerSession^ session = nullptr;
        if (multiplayerSession.payload() != nullptr)
        {
            session = ref new MultiplayerSession(multiplayerSession.payload());
        }
        return session;
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<WriteSessionResult^>^
MultiplayerService::TryWriteSessionAsync(
    _In_ MultiplayerSession^ multiplayerSession,
    _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode
)
{
    THROW_INVALIDARGUMENT_IF_NULL(multiplayerSession);

    auto task = m_cppObj.write_session(
        multiplayerSession->GetCppObj(),
        static_cast<multiplayer_session_write_mode>(multiplayerSessionWriteMode)
        )
    .then([this](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        return TryWriteSessionHelper(multiplayerSession);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<MultiplayerSession^>^ 
MultiplayerService::WriteSessionByHandleAsync(
    _In_ MultiplayerSession^ multiplayerSession,
    _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode,
    _In_ String^ handleId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(multiplayerSession);
    THROW_INVALIDARGUMENT_IF_NULL(handleId);

    auto task = m_cppObj.write_session_by_handle(
        multiplayerSession->GetCppObj(),
        static_cast<multiplayer_session_write_mode>(multiplayerSessionWriteMode),
        STRING_T_FROM_PLATFORM_STRING(handleId)
        )
    .then([](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        THROW_IF_ERR(multiplayerSession);
        MultiplayerSession^ session = nullptr;
        if (multiplayerSession.payload() != nullptr)
        {
            session = ref new MultiplayerSession(multiplayerSession.payload());
        }
        return session;
    });
    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<WriteSessionResult^>^
MultiplayerService::TryWriteSessionByHandleAsync(
    _In_ MultiplayerSession^ multiplayerSession,
    _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode,
    _In_ Platform::String^ handleId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(multiplayerSession);
    THROW_INVALIDARGUMENT_IF_NULL(handleId);

    auto task = m_cppObj.write_session_by_handle(
        multiplayerSession->GetCppObj(),
        static_cast<multiplayer_session_write_mode>(multiplayerSessionWriteMode),
        STRING_T_FROM_PLATFORM_STRING(handleId)
        )
        .then([this](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        return TryWriteSessionHelper(multiplayerSession);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<MultiplayerSession^>^ 
MultiplayerService::GetCurrentSessionAsync(
    _In_ MultiplayerSessionReference^ sessionReference
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);
    auto task = m_cppObj.get_current_session(sessionReference->GetCppObj())
    .then([](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        THROW_IF_ERR(multiplayerSession);
        MultiplayerSession^ session = nullptr;
        if (multiplayerSession.payload() != nullptr)
        {
            session = ref new MultiplayerSession(multiplayerSession.payload());
        }
        return session;
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<MultiplayerSession^>^ 
MultiplayerService::GetCurrentSessionByHandleAsync(
    _In_ String^ handleId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(handleId);
    auto task = m_cppObj.get_current_session_by_handle(STRING_T_FROM_PLATFORM_STRING(handleId))
    .then([](xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSession)
    {
        THROW_IF_ERR(multiplayerSession);
        MultiplayerSession^ session = nullptr;
        if (multiplayerSession.payload() != nullptr)
        {
            session = ref new MultiplayerSession(multiplayerSession.payload());
        }
        return session;
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<MultiplayerSessionStates^>^>^ 
MultiplayerService::GetSessionsAsync(
    _In_ MultiplayerGetSessionsRequest^ getSessionsRequest
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(getSessionsRequest);

    auto task = m_cppObj.get_sessions(getSessionsRequest->GetCppObj())
    .then([](xbox_live_result<std::vector<multiplayer_session_states>> states)
    {
        THROW_IF_ERR(states);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSessionStates, multiplayer_session_states>(states.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<MultiplayerSessionStates^>^>^
MultiplayerService::GetSessionsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_opt_ Platform::String^ sessionTemplateNameFilter,
    _In_opt_ Platform::String^ xboxUserIdFilter,
    _In_opt_ Platform::String^ keywordFilter,
    _In_ MultiplayerSessionVisibility visibilityFilter,
    _In_ uint32 contractVersionFilter,
    _In_ bool includePrivateSessions,
    _In_ bool includeReservations,
    _In_ bool includeInactiveSessions,
    _In_ uint32 maxItems
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);

    return GetSessionsHelper(
        serviceConfigurationId,
        sessionTemplateNameFilter,
        xboxUserIdFilter,
        nullptr,
        keywordFilter,
        visibilityFilter,
        contractVersionFilter,
        includePrivateSessions,
        includeReservations,
        includeInactiveSessions,
        maxItems
        );
}

IAsyncOperation<IVectorView<MultiplayerSessionStates^>^>^
MultiplayerService::GetSessionsForUsersFilterAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_opt_ Platform::String^ sessionTemplateNameFilter,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdsFilter,
    _In_opt_ Platform::String^ keywordFilter,
    _In_ MultiplayerSessionVisibility visibilityFilter,
    _In_ uint32 contractVersionFilter,
    _In_ bool includePrivateSessions,
    _In_ bool includeReservations,
    _In_ bool includeInactiveSessions,
    _In_ uint32 maxItems
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);

    return GetSessionsHelper(
        serviceConfigurationId,
        sessionTemplateNameFilter,
        nullptr,
        xboxUserIdsFilter,
        keywordFilter,
        visibilityFilter,
        contractVersionFilter,
        includePrivateSessions,
        includeReservations,
        includeInactiveSessions,
        maxItems
        );
}

IAsyncAction^ 
MultiplayerService::SetActivityAsync(
    _In_ MultiplayerSessionReference^ sessionReference
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);
    auto task = m_cppObj.set_activity(sessionReference->GetCppObj())
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<Platform::String^>^
MultiplayerService::SetTransferHandleAsync(
    _In_ MultiplayerSessionReference^ targetSessionReference,
    _In_ MultiplayerSessionReference^ originSessionReference
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(targetSessionReference);
    THROW_INVALIDARGUMENT_IF_NULL(originSessionReference);

    auto task = m_cppObj.set_transfer_handle(
        targetSessionReference->GetCppObj(),
        originSessionReference->GetCppObj()
        )
    .then([](xbox_live_result<string_t> result)
    {
        THROW_IF_ERR(result);
        return ref new Platform::String(result.payload().c_str());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
MultiplayerService::SetSearchHandleAsync(
    _In_ MultiplayerSearchHandleRequest^ searchHandleRequest
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(searchHandleRequest);

    auto task = m_cppObj.set_search_handle(searchHandleRequest->GetCppObj())
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncAction^ 
MultiplayerService::ClearActivityAsync(
    _In_ String^ serviceConfigurationId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    auto task = m_cppObj.clear_activity(STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId))
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncAction^
MultiplayerService::ClearSearchHandleAsync(
    _In_ String^ handleId
)
{
    THROW_INVALIDARGUMENT_IF_NULL(handleId);
    auto task = m_cppObj.clear_search_handle(STRING_T_FROM_PLATFORM_STRING(handleId))
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<Platform::String^>^>^ 
MultiplayerService::SendInvitesAsync(
    _In_ MultiplayerSessionReference^ sessionReference,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
    _In_ uint32 titleId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);

    auto task = m_cppObj.send_invites(
        sessionReference->GetCppObj(),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds),
        titleId)
    .then([](xbox_live_result<std::vector<string_t>> invites)
    {
        THROW_IF_ERR(invites);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(invites.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<String^>^>^ 
MultiplayerService::SendInvitesAsync(
    _In_ MultiplayerSessionReference^ sessionReference,
    _In_ IVectorView<String^>^ xboxUserIds,
    _In_ uint32 titleId,
    _In_ Platform::String^ contextStringId,
    _In_ Platform::String^ activationContext
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);

    auto task = m_cppObj.send_invites(
        sessionReference->GetCppObj(),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds),
        titleId,
        STRING_T_FROM_PLATFORM_STRING(contextStringId),
        STRING_T_FROM_PLATFORM_STRING(activationContext)
        )
    .then([](xbox_live_result<std::vector<string_t>> invites)
    {
        THROW_IF_ERR(invites);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(invites.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<MultiplayerActivityDetails^>^>^ 
MultiplayerService::GetActivitiesForSocialGroupAsync(
    _In_ String^ serviceConfigurationId,
    _In_ String^ socialGroupOwnerXboxUserId,
    _In_ String^ socialGroup
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    THROW_INVALIDARGUMENT_IF_NULL(socialGroupOwnerXboxUserId);
    THROW_INVALIDARGUMENT_IF_NULL(socialGroup);

    auto task = m_cppObj.get_activities_for_social_group(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(socialGroupOwnerXboxUserId),
        STRING_T_FROM_PLATFORM_STRING(socialGroup)
        )
    .then([](xbox_live_result<std::vector<multiplayer_activity_details>> activities)
    {
        THROW_IF_ERR(activities);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerActivityDetails, multiplayer_activity_details>(activities.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<MultiplayerActivityDetails^>^>^ 
MultiplayerService::GetActivitiesForUsersAsync(
    _In_ String^ serviceConfigurationId,
    _In_ IVectorView<String^>^ xboxUserIds
    )
{    
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);

    auto task = m_cppObj.get_activities_for_users(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds)
        )
    .then([](xbox_live_result<std::vector<multiplayer_activity_details>> activities)
    {
        THROW_IF_ERR(activities);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerActivityDetails, multiplayer_activity_details>(activities.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<MultiplayerSearchHandleDetails^>^>^
MultiplayerService::GetSearchHandlesAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ sessionTemplateName,
    _In_ Platform::String^ orderBy,
    _In_ bool orderAscending,
    _In_ Platform::String^ searchFilter
    )
{
    auto searchHandleRequest = ref new MultiplayerQuerySearchHandleRequest(serviceConfigurationId, sessionTemplateName);
    searchHandleRequest->OrderBy = orderBy;
    searchHandleRequest->OrderAscending = orderAscending;
    searchHandleRequest->SearchFilter = searchFilter;

    return GetSearchHandlesAsync(searchHandleRequest);
}

IAsyncOperation<IVectorView<MultiplayerSearchHandleDetails^>^>^
MultiplayerService::GetSearchHandlesAsync(
    _In_ MultiplayerQuerySearchHandleRequest^ searchHandleRequest
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(searchHandleRequest);
    THROW_INVALIDARGUMENT_IF_NULL(searchHandleRequest->ServiceConfigurationId);
    THROW_INVALIDARGUMENT_IF_NULL(searchHandleRequest->SessionTemplateName);

    auto task = m_cppObj.get_search_handles(searchHandleRequest->GetCppObj())
    .then([](xbox_live_result<std::vector<multiplayer_search_handle_details>> activities)
    {
        THROW_IF_ERR(activities);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSearchHandleDetails, multiplayer_search_handle_details>(activities.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

void
MultiplayerService::EnableMultiplayerSubscriptions()
{
    auto result = m_cppObj.enable_multiplayer_subscriptions();
    THROW_ON_ERR_CODE(result);
}

void 
MultiplayerService::DisableMultiplayerSubscriptions()
{
    CONVERT_STD_EXCEPTION(
        m_cppObj.disable_multiplayer_subscriptions();
        );
}

WriteSessionResult^
MultiplayerService::TryWriteSessionHelper(
    _In_ xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionResult
    )
{
    std::shared_ptr<multiplayer_session> multiplayerSession = multiplayerSessionResult.payload();
    WriteSessionStatus writeSessionStatus = WriteSessionStatus::Unknown;
    std::error_code errorCode = multiplayerSessionResult.err();
    
    if (multiplayerSession == nullptr && errorCode == xbox_live_error_code::no_error)
    {
        writeSessionStatus = WriteSessionStatus::SessionDeleted;
    }
    else if (errorCode == xbox_live_error_code::http_status_401_unauthorized)
    {
        writeSessionStatus = WriteSessionStatus::AccessDenied;
    }
    else if (errorCode == xbox_live_error_code::http_status_404_not_found)
    {
        writeSessionStatus = WriteSessionStatus::HandleNotFound;
    }
    else if (errorCode == xbox_live_error_code::http_status_409_conflict)
    {
        writeSessionStatus = WriteSessionStatus::Conflict;
    }
    else if (errorCode == xbox_live_error_code::http_status_412_precondition_failed)
    {
        writeSessionStatus = WriteSessionStatus::OutOfSync;
    }
    else
    {
        if (multiplayerSession != nullptr)
        {
            writeSessionStatus = static_cast<WriteSessionStatus>(multiplayerSession->write_status());
            if (writeSessionStatus == WriteSessionStatus::Unknown && multiplayerSessionResult.err())
            {
                THROW_ON_ERR_CODE(multiplayerSessionResult.err());
            }
        }
        else
        {
            THROW_ON_ERR_CODE(multiplayerSessionResult.err());
        }
    }

    bool didSucceed = (writeSessionStatus != WriteSessionStatus::AccessDenied &&
        writeSessionStatus != WriteSessionStatus::Conflict &&
        writeSessionStatus != WriteSessionStatus::OutOfSync &&
        writeSessionStatus != WriteSessionStatus::HandleNotFound);

    return ref new WriteSessionResult(
        didSucceed,
        multiplayerSession != nullptr ? ref new MultiplayerSession(multiplayerSession) : nullptr,
        writeSessionStatus
        );
}

IAsyncOperation<IVectorView<MultiplayerSessionStates^>^>^
MultiplayerService::GetSessionsHelper(
    _In_ Platform::String^ serviceConfigurationId,
    _In_opt_ Platform::String^ sessionTemplateNameFilter,
    _In_opt_ Platform::String^ xboxUserIdFilter,
    _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdsFilter,
    _In_opt_ Platform::String^ keywordFilter,
    _In_ MultiplayerSessionVisibility visibilityFilter,
    _In_ uint32 contractVersionFilter,
    _In_ bool includePrivateSessions,
    _In_ bool includeReservations,
    _In_ bool includeInactiveSessions,
    _In_ uint32 maxItems
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);

    auto sessionsRequest = multiplayer_get_sessions_request(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        maxItems
        );

    sessionsRequest.set_session_template_name_filter(
        STRING_T_FROM_PLATFORM_STRING(sessionTemplateNameFilter)
        );

    if (xboxUserIdFilter != nullptr)
    {
        sessionsRequest.set_xbox_user_id_filter(
            STRING_T_FROM_PLATFORM_STRING(xboxUserIdFilter)
            );
    }
    else
    {
        sessionsRequest.set_xbox_user_ids_filter(
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIdsFilter)
            );
    }

    sessionsRequest.set_keyword_filter(
        STRING_T_FROM_PLATFORM_STRING(keywordFilter)
        );

    sessionsRequest.set_visibility_filter(
        static_cast<multiplayer_session_visibility>(visibilityFilter)
        );

    sessionsRequest.set_contract_version_filter(
        contractVersionFilter
        );

    sessionsRequest.set_include_private_sessions(
        includePrivateSessions
        );

    sessionsRequest.set_include_reservations(
        includeReservations
        );

    sessionsRequest.set_include_inactive_sessions(
        includeInactiveSessions
        );

    auto task = m_cppObj.get_sessions(sessionsRequest)
    .then([](xbox_live_result<std::vector<multiplayer_session_states>> states)
    {
        THROW_IF_ERR(states);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSessionStates, multiplayer_session_states>(states.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END