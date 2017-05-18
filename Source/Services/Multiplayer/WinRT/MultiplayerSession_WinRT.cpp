// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerSession_WinRT.h"
#include "Utils_WinRT.h"
#include "XboxLiveContext_WinRT.h"
#include "user_context.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::GameServerPlatform;
using namespace Microsoft::Xbox::Services::Tournaments;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSession::MultiplayerSession(
    _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> cppObj
    ) :
    m_cppObj(cppObj)
{
    THROW_INVALIDARGUMENT_IF_NULL(m_cppObj);

    m_sessionReference = ref new MultiplayerSessionReference(m_cppObj->session_reference());
    m_sessionProperties = ref new MultiplayerSessionProperties(m_cppObj->session_properties());
    m_sessionRoleTypes = ref new MultiplayerSessionRoleTypes(m_cppObj->session_role_types());
    m_sessionConstants = ref new MultiplayerSessionConstants(m_cppObj->session_constants());
    m_matchmakingServer = ref new MultiplayerSessionMatchmakingServer(m_cppObj->matchmaking_server());
    m_tournamentsServer = ref new MultiplayerSessionTournamentsServer(m_cppObj->tournaments_server());
    m_arbitrationServer = ref new MultiplayerSessionArbitrationServer(m_cppObj->arbitration_server());
}

MultiplayerSession::MultiplayerSession(
    _In_ XboxLiveContext^ xboxLiveContext
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    m_cppObj = std::make_shared<xbox::services::multiplayer::multiplayer_session>(
        STRING_T_FROM_PLATFORM_STRING(xboxLiveContext->User->XboxUserId)
        );
}

#if TV_API
MultiplayerSession::MultiplayerSession(
    _In_ XboxLiveContext^ xboxLiveContext,
    _In_ MultiplayerSessionReference^ multiplayerSessionReference,
    _In_ uint32 maxMembersInSession,
    _In_ bool reserved,
    _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
    _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
    _In_opt_ Platform::String^ sessionCustomConstantsJson
    )
{
    UNREFERENCED_PARAMETER(reserved);
    _Init(xboxLiveContext, multiplayerSessionReference, maxMembersInSession, multiplayerSessionVisibility, initiatorXboxUserIds, sessionCustomConstantsJson);
}
#endif

MultiplayerSession::MultiplayerSession(
    _In_ XboxLiveContext^ xboxLiveContext,
    _In_ MultiplayerSessionReference^ multiplayerSessionReference,
    _In_ uint32 maxMembersInSession,
    _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
    _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
    _In_opt_ Platform::String^ sessionCustomConstantsJson
    )
{
    _Init(xboxLiveContext, multiplayerSessionReference, maxMembersInSession, multiplayerSessionVisibility, initiatorXboxUserIds, sessionCustomConstantsJson);
}

void
MultiplayerSession::_Init(
    _In_ XboxLiveContext^ xboxLiveContext,
    _In_ MultiplayerSessionReference^ multiplayerSessionReference,
    _In_ uint32 maxMembersInSession,
    _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
    _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
    _In_opt_ Platform::String^ sessionCustomConstantsJson
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    THROW_INVALIDARGUMENT_IF_NULL(multiplayerSessionReference);

    std::vector<string_t> intitiatorIds;
    if (initiatorXboxUserIds != nullptr)
    {
        intitiatorIds = UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(initiatorXboxUserIds);
    }

    CONVERT_STD_EXCEPTION(
        auto sessionCustomConstantsValueString = UtilsWinRT::JsonValueFromPlatformString(sessionCustomConstantsJson);
        m_cppObj = std::make_shared<xbox::services::multiplayer::multiplayer_session>(
            STRING_T_FROM_PLATFORM_STRING(xboxLiveContext->User->XboxUserId),
            multiplayerSessionReference->GetCppObj(),
            maxMembersInSession,
            static_cast<xbox::services::multiplayer::multiplayer_session_visibility>(multiplayerSessionVisibility),
            intitiatorIds,
            sessionCustomConstantsValueString
            );
        );
}

MultiplayerSession::MultiplayerSession(
    _In_ XboxLiveContext^ xboxLiveContext,
    _In_ MultiplayerSessionReference^ multiplayerSessionReference
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    CONVERT_STD_EXCEPTION(
        m_cppObj = std::make_shared<xbox::services::multiplayer::multiplayer_session>(
            STRING_T_FROM_PLATFORM_STRING(xboxLiveContext->User->XboxUserId),
            multiplayerSessionReference->GetCppObj()
            );
        );
}

std::shared_ptr<xbox::services::multiplayer::multiplayer_session>
MultiplayerSession::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<Platform::String^>^ 
MultiplayerSession::HostCandidates::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->host_candidates())->GetView();
}

MultiplayerSessionReference^
MultiplayerSession::SessionReference::get()
{
    if (m_sessionReference == nullptr)
    {
        m_sessionReference = ref new MultiplayerSessionReference(m_cppObj->session_reference());
    }
    return m_sessionReference;
}

MultiplayerSessionConstants^ 
MultiplayerSession::SessionConstants::get()
{
    if (m_sessionConstants == nullptr)
    {
        m_sessionConstants = ref new MultiplayerSessionConstants(m_cppObj->session_constants());
    }
    return m_sessionConstants;
}

MultiplayerSessionProperties^ 
MultiplayerSession::SessionProperties::get()
{
    if (m_sessionProperties == nullptr)
    {
        m_sessionProperties = ref new MultiplayerSessionProperties(m_cppObj->session_properties());
    }
    return m_sessionProperties;
}

MultiplayerSessionRoleTypes^
MultiplayerSession::SessionRoleTypes::get()
{
    if (m_sessionRoleTypes == nullptr)
    {
        m_sessionRoleTypes = ref new MultiplayerSessionRoleTypes(m_cppObj->session_role_types());
    }
    return m_sessionRoleTypes;
}

IVectorView<MultiplayerSessionMember^>^ 
MultiplayerSession::Members::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSessionMember>(m_cppObj->members())->GetView();
}

MultiplayerSessionMatchmakingServer^
MultiplayerSession::MatchmakingServer::get()
{
    return m_matchmakingServer;
}

MultiplayerSessionTournamentsServer^
MultiplayerSession::TournamentsServer::get()
{
    return m_tournamentsServer;
}

MultiplayerSessionArbitrationServer^
MultiplayerSession::ArbitrationServer::get()
{
    return m_arbitrationServer;
}

MultiplayerSessionMember^ 
MultiplayerSession::CurrentUser::get()
{
    if (m_cppObj->current_user())
    {
        return ref new MultiplayerSessionMember(m_cppObj->current_user());
    }
    else
    {
        return nullptr;
    }
}

void
MultiplayerSession::AddMemberReservation( 
    _In_ Platform::String^ xboxUserId, 
    _In_opt_ Platform::String^ memberCustomConstantsJson
    )
{
    auto memberCustomConstantsJsonString = UtilsWinRT::JsonValueFromPlatformString(memberCustomConstantsJson);
    auto result = m_cppObj->add_member_reservation(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        memberCustomConstantsJsonString
        );
    THROW_ON_ERR_CODE(result);
}

void
MultiplayerSession::AddMemberReservation( 
    _In_ Platform::String^ xboxUserId, 
    _In_opt_ Platform::String^ memberCustomConstantsJson,
    _In_ bool initializeRequested
    )
{
    auto memberCustomConstantsJsonString = UtilsWinRT::JsonValueFromPlatformString(memberCustomConstantsJson);
    auto result = m_cppObj->add_member_reservation(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        memberCustomConstantsJsonString,
        initializeRequested
        );
    THROW_ON_ERR_CODE(result);
}

MultiplayerSessionMember^
MultiplayerSession::Join()
{
    return JoinHelper(nullptr, false, false, false);
}

MultiplayerSessionMember^ 
MultiplayerSession::Join(
    _In_opt_ Platform::String^ memberCustomConstantsJson
    )
{
    return JoinHelper(memberCustomConstantsJson, false, false, false);
}

MultiplayerSessionMember^
MultiplayerSession::Join(
    _In_opt_ Platform::String^ memberCustomConstantsJson,
    _In_ bool initializeRequested
    )
{
    return JoinHelper(memberCustomConstantsJson, initializeRequested, false, true);
}

MultiplayerSessionMember^
MultiplayerSession::Join(
    _In_opt_ Platform::String^ memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus
    )
{
    return JoinHelper(memberCustomConstantsJson, initializeRequested, joinWithActiveStatus, true);
}

MultiplayerSessionMember^
MultiplayerSession::JoinHelper(
    _In_opt_ Platform::String^ memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus,
    _In_ bool addInitializePropertyToRequest
    )
{
    auto memberCustomConstantsJsonString = web::json::value::null();
    if (memberCustomConstantsJson)
    {
        memberCustomConstantsJsonString = UtilsWinRT::JsonValueFromPlatformString(memberCustomConstantsJson);
    }
    auto result = m_cppObj->join(
        memberCustomConstantsJsonString,
        initializeRequested,
        joinWithActiveStatus,
        addInitializePropertyToRequest
        );

    THROW_IF_ERR(result);
    MultiplayerSessionMember^ member = ref new MultiplayerSessionMember(
        result.payload()
        );

    m_currentUser = member;
    return member;
}

void
MultiplayerSession::SetVisibility( 
    _In_ MultiplayerSessionVisibility visibility
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_visibility(
            static_cast<xbox::services::multiplayer::multiplayer_session_visibility>(visibility)
            );
        );
}
  
void
MultiplayerSession::SetMaxMembersInSession( 
    _In_ uint32 maxMembersInSession
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_max_members_in_session(
            maxMembersInSession
            );
        );
}

void
MultiplayerSession::SetMutableRoleSettings(
    _In_ Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ roleTypes
)
{
    auto errc = m_cppObj->set_mutable_role_settings(
        UtilsWinRT::CreateStdMapObjectWithStringKeyFromPlatformMapObj<multiplayer_role_type>(roleTypes)
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetTimeouts( 
    _In_ Windows::Foundation::TimeSpan memberReservedTimeout,
    _In_ Windows::Foundation::TimeSpan memberInactiveTimeout,
    _In_ Windows::Foundation::TimeSpan memberReadyTimeout,
    _In_ Windows::Foundation::TimeSpan sessionEmptyTimeout
    )
{
    auto errc = m_cppObj->set_timeouts(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(memberReservedTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(memberInactiveTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(memberReadyTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(sessionEmptyTimeout)
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetArbitrationTimeouts(
    _In_ Windows::Foundation::TimeSpan arbitrationTimeout,
    _In_ Windows::Foundation::TimeSpan forfeitTimeout
    )
{
    auto errc = m_cppObj->set_arbitration_timeouts(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(arbitrationTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(forfeitTimeout)
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetQualityOfServiceConnectivityMetrics(
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
    )
{
    auto errc = m_cppObj->set_quality_of_service_connectivity_metrics(
        enableLatencyMetric,
        enableBandwidthDownMetric,
        enableBandwidthUpMetric,
        enableCustomMetric
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetManagedInitialization(
    _In_ Windows::Foundation::TimeSpan joinTimeout,
    _In_ Windows::Foundation::TimeSpan measurementTimeout,
    _In_ Windows::Foundation::TimeSpan evaluationTimeout,
    _In_ bool autoEvalute,
    _In_ uint32 membersNeededToStart
    )
{
#pragma warning(suppress: 4996)
    auto errc = m_cppObj->set_managed_initialization(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(joinTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(measurementTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(evaluationTimeout),
        autoEvalute,
        membersNeededToStart
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetMemberInitialization(
    _In_ Windows::Foundation::TimeSpan joinTimeout,
    _In_ Windows::Foundation::TimeSpan measurementTimeout,
    _In_ Windows::Foundation::TimeSpan evaluationTimeout,
    _In_ bool autoEvalute,
    _In_ uint32 membersNeededToStart
    )
{
    auto errc = m_cppObj->set_member_initialization(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(joinTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(measurementTimeout),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(evaluationTimeout),
        autoEvalute,
        membersNeededToStart
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetPeerToPeerRequirements(
    _In_ Windows::Foundation::TimeSpan latencyMaximum,
    _In_ uint32 bandwidthMinimumInKilobitsPerSecond
    )
{
    auto errc = m_cppObj->set_peer_to_peer_requirements(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(latencyMaximum),
        bandwidthMinimumInKilobitsPerSecond
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetPeerToHostRequirements(
    _In_ Windows::Foundation::TimeSpan latencyMaximum,
    _In_ uint32 bandwidthDownMinimumInKilobitsPerSecond,
    _In_ uint32 bandwidthUpMinimumInKilobitsPerSecond,
    _In_ MultiplayMetrics hostSelectionMetric
    )
{
    auto errc = m_cppObj->set_peer_to_host_requirements(
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(latencyMaximum),
        bandwidthDownMinimumInKilobitsPerSecond,
        bandwidthUpMinimumInKilobitsPerSecond,
        static_cast<xbox::services::multiplayer::multiplay_metrics>(hostSelectionMetric)
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetMeasurementServerAddresses(
    _In_ IVectorView<QualityOfServiceServer^>^ measurementServerAddresses
    )
{
    auto errc = m_cppObj->set_measurement_server_addresses(
        UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<xbox::services::game_server_platform::quality_of_service_server>(
            measurementServerAddresses
            )
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetCloudComputePackageJson(
    _In_ Platform::String^ sessionCloudComputePackageConstantsJson
    )
{
    auto cloudComputePackageValueString = UtilsWinRT::JsonValueFromPlatformString(sessionCloudComputePackageConstantsJson);
    auto errc = m_cppObj->set_cloud_compute_package_json(
        cloudComputePackageValueString
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetSessionCapabilities(
    _In_ MultiplayerSessionCapabilities^ capabilities
    )
{
    auto errc = m_cppObj->set_session_capabilities(
        capabilities->GetCppObj()
        );

    THROW_ON_ERR_CODE(errc);
}

void
MultiplayerSession::SetInitializationStatus( 
    _In_ bool initializationSucceeded
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_initialization_status(
            initializationSucceeded
            );
        );
}

void
MultiplayerSession::SetHostDeviceToken( 
    _In_ Platform::String^ hostDeviceToken
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_host_device_token(
            STRING_T_FROM_PLATFORM_STRING(hostDeviceToken)
            );
        );
}

void
MultiplayerSession::SetMatchmakingServerConnectionPath( 
    _In_ Platform::String^ serverConnectionPath
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_matchmaking_server_connection_path(
            STRING_T_FROM_PLATFORM_STRING(serverConnectionPath)
            );
        );
}

void
MultiplayerSession::SetClosed(
    _In_ bool closed
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_closed(closed);
    );
}

void
MultiplayerSession::SetLocked(
    _In_ bool locked
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_locked(locked);
    );
}

void
MultiplayerSession::SetAllocateCloudCompute(
    _In_ bool allocateCloudCompute
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_allocate_cloud_compute(allocateCloudCompute);
    );
}

void
MultiplayerSession::SetMatchmakingResubmit( 
    _In_ bool matchResubmit 
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_matchmaking_resubmit(
            matchResubmit
            );
        )
}

void
MultiplayerSession::SetServerConnectionStringCandidates( 
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ serverConnectionStringCandidates
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_server_connection_string_candidates(
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(serverConnectionStringCandidates)
            );
        );
}

void 
MultiplayerSession::SetSessionChangeSubscription(
    _In_ MultiplayerSessionChangeTypes changeTypes
    )
{
    auto result = m_cppObj->set_session_change_subscription(
        static_cast<xbox::services::multiplayer::multiplayer_session_change_types>(changeTypes)
        );

    THROW_ON_ERR_CODE(result);
}

void
MultiplayerSession::Leave()
{
    auto result = m_cppObj->leave();
    THROW_ON_ERR_CODE(result);
}

void
MultiplayerSession::SetCurrentUserStatus( 
    _In_ MultiplayerSessionMemberStatus status 
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_current_user_status(
            static_cast<xbox::services::multiplayer::multiplayer_session_member_status>(status)
        );
    );
}

void
MultiplayerSession::SetCurrentUserSecureDeviceAddressBase64( 
    _In_ Platform::String^ value 
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_current_user_secure_device_address_base64(
            STRING_T_FROM_PLATFORM_STRING(value)
            );
        );
}

void
MultiplayerSession::SetCurrentUserRoleInfo(
    _In_ IMapView<Platform::String^, Platform::String^>^ roles
    )
{
    m_roles = roles;
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_current_user_role_info(
            UtilsWinRT::CreateStdUnorderedMapStringFromMapObj(m_roles)
        );
    );
}

void
MultiplayerSession::SetCurrentUserMembersInGroup( 
    _In_ IVectorView<MultiplayerSessionMember^>^ membersInGroup
    )
{
    CONVERT_STD_EXCEPTION(
        THROW_INVALIDARGUMENT_IF_NULL(membersInGroup);
        m_cppObj->set_current_user_members_in_group(
            UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>>(membersInGroup)
            );
        );
}

void
MultiplayerSession::SetCurrentUserQualityOfServiceMeasurements( 
    _In_ IVectorView<MultiplayerQualityOfServiceMeasurements^>^ measurements
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->set_current_user_quality_of_service_measurements(
            std::make_shared<std::vector<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>>(
                UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>(
                    measurements
                    )
                )
            )
        );
}

void MultiplayerSession::SetCurrentUserQualityOfServiceServerMeasurementsJson(
    _In_ Platform::String^ valueJson
    )
{
    CONVERT_STD_EXCEPTION(
        auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
        m_cppObj->set_current_user_quality_of_service_measurements_json(
            valueJsonString
            );
        );
}

void 
MultiplayerSession::SetCurrentUserArbitrationResults(
    _In_ Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ results
    )
{
    m_cppObj->set_current_user_member_arbitration_results(
        UtilsWinRT::CreateStdMapObjectWithStringKeyFromPlatformMapObj<tournament_team_result>(results)
        );
}

void
MultiplayerSession::SetCurrentUserMemberCustomPropertyJson( 
    _In_ Platform::String^ name, 
    _In_opt_ Platform::String^ valueJson 
    )
{
    CONVERT_STD_EXCEPTION(
        auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
        m_cppObj->set_current_user_member_custom_property_json(
            STRING_T_FROM_PLATFORM_STRING(name),
            valueJsonString
            );
        );
}

void
MultiplayerSession::DeleteCurrentUserMemberCustomPropertyJson( 
    _In_ Platform::String^ name 
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->delete_current_user_member_custom_property_json(
            STRING_T_FROM_PLATFORM_STRING(name)
            );
        );
}

void MultiplayerSession::SetMatchmakingTargetSessionConstantsJson(
    _In_ Platform::String^ matchmakingTargetSessionConstants
    )
{
    CONVERT_STD_EXCEPTION(
        auto jsonString = UtilsWinRT::JsonValueFromPlatformString(matchmakingTargetSessionConstants);
        m_cppObj->set_matchmaking_target_session_constants_json(
            jsonString
            );
        )
}

void
MultiplayerSession::SetSessionCustomPropertyJson(
    _In_ Platform::String^ name,
    _In_opt_ Platform::String^ valueJson
    )
{
    CONVERT_STD_EXCEPTION(
        auto jsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
        m_cppObj->set_session_custom_property_json(
            STRING_T_FROM_PLATFORM_STRING(name),
            jsonString
            );
        );
}

void
MultiplayerSession::DeleteSessionCustomPropertyJson(
    _In_ Platform::String^ name
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj->delete_session_custom_property_json(
            STRING_T_FROM_PLATFORM_STRING(name)
            );
        );
}

MultiplayerSessionChangeTypes 
MultiplayerSession::CompareMultiplayerSessions(
    _In_ MultiplayerSession^ currentSession,
    _In_ MultiplayerSession^ oldSession
    )
{
    auto result = xbox::services::multiplayer::multiplayer_session::compare_multiplayer_sessions(
            currentSession->GetCppObj(),
            oldSession->GetCppObj()
            );
    THROW_IF_ERR(result);

    return MultiplayerSessionChangeTypes(result.payload());
}

WriteSessionStatus
MultiplayerSession::ConvertHttpStatusToWriteSessionStatus(
    _In_ int32 httpStatusCode
    )
{
    CONVERT_STD_EXCEPTION(
        auto result = xbox::services::multiplayer::multiplayer_session::convert_http_status_to_write_session_status(
            httpStatusCode
            );

        return static_cast<WriteSessionStatus>(result);
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END