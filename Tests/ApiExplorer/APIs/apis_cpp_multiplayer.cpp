// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

using namespace utility;

#if CPP_TESTS_ENABLED
struct MultiplayerStateCpp
{
    std::vector<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> sessionHandles;
    xbox::services::multiplayer::multiplayer_session_reference sessionRef;
    std::shared_ptr<xbox::services::multiplayer::multiplayer_search_handle_details> searchHandleDetails;
    std::string inviteHandle{};
    function_context sessionChangedContext{ 0 };
    function_context subscriptionLostContext{ 0 };
    function_context connectionIdChangedContext{ 0 };
    std::string activityHandle{};

    static std::string GetSessionName(uint64_t sessionId = 0) noexcept
    {
        // ID to make session names unique per API runner run
#if HC_PLATFORM == HC_PLATFORM_GDK
        ULARGE_INTEGER largeInt;
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);

        largeInt.LowPart = fileTime.dwLowDateTime;
        largeInt.HighPart = fileTime.dwHighDateTime;

        static uint64_t runId{ largeInt.QuadPart };
#else
        static uint64_t runId{ datetime::utc_now().to_interval() };
#endif

        std::stringstream ss;
        ss << "GameSession-" << runId << "-ID" << sessionId;
        return ss.str();
    }
};
std::unique_ptr<MultiplayerStateCpp> g_multiplayerStateCpp;

MultiplayerStateCpp* MPStateCpp()
{
    if (!g_multiplayerStateCpp)
    {
        g_multiplayerStateCpp = std::make_unique<MultiplayerStateCpp>();
        g_multiplayerStateCpp->sessionHandles.resize(10);
    }
    return g_multiplayerStateCpp.get();
}


xbox::services::multiplayer::multiplayer_session_write_mode ConvertStringToCppMultiplayerSessionWriteMode(const char* str)
{
    xbox::services::multiplayer::multiplayer_session_write_mode writeMode = xbox::services::multiplayer::multiplayer_session_write_mode::update_or_create_new;

    if (pal::stricmp(str, "multiplayer_session_write_mode::synchronized_update") == 0) writeMode = xbox::services::multiplayer::multiplayer_session_write_mode::synchronized_update;
    else if (pal::stricmp(str, "multiplayer_session_write_mode::create_new") == 0) writeMode = xbox::services::multiplayer::multiplayer_session_write_mode::create_new;
    else if (pal::stricmp(str, "multiplayer_session_write_mode::update_existing") == 0) writeMode = xbox::services::multiplayer::multiplayer_session_write_mode::update_existing;

    return writeMode;
}

//multiplayer_session_reference

void LogSessionRef(const xbox::services::multiplayer::multiplayer_session_reference* sessionRef)
{
    LogToFile("Scid:%s", xbox::services::Utils::StringFromStringT(sessionRef->service_configuration_id()).c_str());
    LogToFile("SessionName:%s", xbox::services::Utils::StringFromStringT(sessionRef->session_name()).c_str());
    LogToFile("SessionTemplateName:%s", xbox::services::Utils::StringFromStringT(sessionRef->session_template_name()).c_str());
}

int MultiplayerSessionReferenceIsValidCpp_Lua(lua_State *L)
{
    bool isValid = !MPStateCpp()->sessionRef.is_null();
    LogToFile("MultiplayerSessionReferenceIsValidCpp isValid:%d", isValid);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionReferenceCreateCpp_Lua(lua_State* L)
{
    string_t scid = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, Data()->scid).c_str());
    string_t sessionTemplateName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "MinGameSession").c_str());
    string_t sessionName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, MultiplayerStateCpp::GetSessionName()).c_str());

    MPStateCpp()->sessionRef = xbox::services::multiplayer::multiplayer_session_reference(scid, sessionTemplateName, sessionName);

    LogToFile("MultiplayerSessionReferenceCreateCpp");
    LogSessionRef(&MPStateCpp()->sessionRef);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionReferenceParseFromUriPathCpp_Lua(lua_State* L)
{
    string_t path = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "").c_str());
    if (path.empty())
    {
        stringstream_t ss;
        ss << L"/serviceconfigs/00000000-0000-0000-0000-000076029b4d/sessionTemplates/MinGameSession/sessions/";
        ss << xbox::services::Utils::StringTFromUtf8(MultiplayerStateCpp::GetSessionName().c_str());
        path = ss.str();
    }

    MPStateCpp()->sessionRef.parse_from_uri_path(path);
    LogToFile("MultiplayerSessionReferenceParseFromUriPathCpp");
    LogToFile("Scid:%s", xbox::services::Utils::StringFromStringT(MPStateCpp()->sessionRef.service_configuration_id()).c_str());
    LogToFile("SessionName:%s", xbox::services::Utils::StringFromStringT(MPStateCpp()->sessionRef.session_name()).c_str());
    LogToFile("SessionTemplateName:%s", xbox::services::Utils::StringFromStringT(MPStateCpp()->sessionRef.session_template_name()).c_str());

    return LuaReturnHR(L, S_OK);
}

//multitplayer_session

std::shared_ptr<xbox::services::multiplayer::multiplayer_session> GetSessionHandleFromArgCpp(lua_State *L, int paramNum, uint64_t* sessionIndexOut = nullptr)
{
    uint64_t sessionIndex { GetUint64FromLua(L, paramNum, 0) };
    assert(MPStateCpp()->sessionHandles.size() > sessionIndex);
    if (sessionIndexOut != nullptr)
    {
        *sessionIndexOut = sessionIndex;
    }
    return MPStateCpp()->sessionHandles[static_cast<uint32_t>(sessionIndex)];
}

int MultiplayerSessionCreateCpp_Lua(lua_State *L)
{
    string_t scid = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, Data()->scid).c_str());
    string_t sessionTemplateName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "MinGameSession").c_str());
    uint64_t sessionIndex{ GetUint64FromLua(L, 4, 0) };
    string_t sessionName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, "").c_str());
    if (sessionName.empty())
    {
        sessionName = xbox::services::Utils::StringTFromUtf8(MultiplayerStateCpp::GetSessionName(sessionIndex).c_str());
    }

    xbox::services::multiplayer::multiplayer_session_reference sessionRef(scid, sessionTemplateName, sessionName);
    string_t xuid = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);

    MPStateCpp()->sessionHandles[static_cast<uint32_t>(sessionIndex)] = std::make_shared<xbox::services::multiplayer::multiplayer_session>(xuid, sessionRef);

    lua_pushinteger(L, static_cast<lua_Integer>(sessionIndex));
    LogToFile("MultiplayerSessionCreateCpp");
    return LuaReturnHR(L, S_OK, 1);
}

int MultiplayerSessionJoinCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    string_t memberCustomConstantsJsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "{}").c_str());
    bool initializeRequested = GetBoolFromLua(L, 3, true);
    bool joinWithActiveStatus = GetBoolFromLua(L, 4, true);
    bool addInitializePropertyToRequest = GetBoolFromLua(L, 5, true);

    web::json::value memberCustomConstantsJson = web::json::value::parse(memberCustomConstantsJsonString);

    sessionHandle->join(memberCustomConstantsJson, initializeRequested, joinWithActiveStatus, addInitializePropertyToRequest);

    LogToFile("MultiplayerSessionJoinCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionAddMemberReservationCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    string_t xuid = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 2, 2814636782672891));
    string_t memberCustomConstantsJsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, "{}").c_str());
    web::json::value memberCustomConstantsJson = web::json::value::parse(memberCustomConstantsJsonString);
    bool initializeRequested = GetBoolFromLua(L, 4, true);

    
    sessionHandle->add_member_reservation(xuid, memberCustomConstantsJson, initializeRequested);

    LogToFile("MultiplayerSessionAddMemberReservationCpp");
    return LuaReturnHR(L, S_OK);
}


int MultiplayerSessionTimeOfSessionCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    datetime timeOfSession = sessionHandle->date_of_session();

    LogToFile("MultiplayerSessionStartTime timeOfSession:%s", xbox::services::Utils::StringFromStringT(timeOfSession.to_string()).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetInitializationInfoCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    xbox::services::multiplayer::multiplayer_initialization_stage stage = sessionHandle->initialization_stage();
    datetime stageStartTime = sessionHandle->initializing_stage_start_time();
    uint32_t episode = sessionHandle->intializing_episode();

    LogToFile("MultiplayerSessionGetInitializationInfoCpp");
    LogToFile("Stage: %d", stage);
    LogToFile("StageStartTime: %ul", stageStartTime);
    LogToFile("Episode: %d", episode);

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSubscribedChangeTypesCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    xbox::services::multiplayer::multiplayer_session_change_types changeTypes = sessionHandle->subscribed_change_types();

    LogToFile("MultiplayerSessionSubscribedChangeTypesCpp");
    LogToFile("changeTypes: 0x%0.4x", changeTypes);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionHostCandidatesCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    std::vector<string_t> hostCandidates = sessionHandle->host_candidates();

    LogToFile("MultiplayerSessionHostCandidatesCpp:");
    for (string_t hostCandidate : hostCandidates)
    {
        LogToFile(xbox::services::Utils::StringFromStringT(hostCandidate).c_str());
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionReferenceCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    xbox::services::multiplayer::multiplayer_session_reference sessionRef = sessionHandle->session_reference();

    LogToFile("MultiplayerSessionSessionReferenceCpp");
    LogToFile("Scid:%s", xbox::services::Utils::StringFromStringT(sessionRef.service_configuration_id()).c_str());
    LogToFile("SessionName:%s", xbox::services::Utils::StringFromStringT(sessionRef.session_name()).c_str());
    LogToFile("SessionTemplateName:%s", xbox::services::Utils::StringFromStringT(sessionRef.session_template_name()).c_str());
    
    MPStateCpp()->sessionRef = sessionRef;

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionConstantsCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    auto sessionConstants = sessionHandle->session_constants();

    LogToFile("MultiplayerSessionSessionConstantsCpp");
    LogToFile("MaxMembersInSession: %d", sessionConstants->max_members_in_session());
    LogToFile("Visibility: %d", sessionConstants->visibility());
    LogToFile("InitiatorXuidsSize: %ul", sessionConstants->initiator_xbox_user_ids().size());
    LogToFile("CustomJson: %s", xbox::services::Utils::StringFromStringT(sessionConstants->session_custom_constants_json().serialize()).c_str());
    LogToFile("SessionCloudComputePackageConstantsJson: %s", xbox::services::Utils::StringFromStringT(sessionConstants->session_cloud_compute_package_constants_json().serialize()).c_str());
    LogToFile("MemberReservedTimeout: %ul", sessionConstants->member_reserved_time_out());
    LogToFile("MemberInactiveTimeout: %ul", sessionConstants->member_inactive_timeout());
    LogToFile("MemberReadyTimeout: %ul", sessionConstants->member_ready_timeout());
    LogToFile("SessionEmptyTimeout: %ul", sessionConstants->session_empty_timeout());
    LogToFile("EnableMetricsLatency: %d", sessionConstants->enable_metrics_latency());
    LogToFile("EnableMetricsBandwidthDown: %d", sessionConstants->enable_metrics_bandwidth_down());
    LogToFile("EnableMetricsBandwidthUp: %d", sessionConstants->enable_metrics_bandwidth_up());
    LogToFile("EnableMetricsCustom: %d", sessionConstants->enable_metrics_custom());
    LogToFile("PeerToPeerRequirements->LatencyMaximum: %ul", sessionConstants->peer_to_peer_requirements().latency_maximum());
    LogToFile("PeerToPeerRequirements->BandwidthMinimumInKbps: %ul", sessionConstants->peer_to_peer_requirements().bandwidth_minimum_in_kilobits_per_second());
    LogToFile("PeerToHostRequirements->LatencyMaximum: %ul", sessionConstants->peer_to_host_requirements().latency_maximum());
    LogToFile("PeerToHostRequirements->BandwidthMinimumInKbps: %ul", sessionConstants->peer_to_host_requirements().bandwidth_down_minimum_in_kilobits_per_second());
    LogToFile("PeerToHostRequirements->BandwidthUpMinimumInKbps: %ul", sessionConstants->peer_to_host_requirements().bandwidth_up_minimum_in_kilobits_per_second());
    LogToFile("PeerToHostRequirements->HostSelectionMetric: %ul", sessionConstants->peer_to_host_requirements().host_selection_metric());
    LogToFile("MeasurementServerAddressesJson: %s", xbox::services::Utils::StringFromStringT(sessionConstants->measurement_server_addresses_json().serialize()).c_str());
    LogToFile("ClientMatchmakingCapable: %d", sessionConstants->client_matchmaking_capable());
    LogToFile("EnableMetricsCustom: %d", sessionConstants->enable_metrics_custom());
    LogToFile("CapabilitiesConnectivity: %d", sessionConstants->capabilities_connectivity());
    LogToFile("CapabilitiesSuppressPresenceActivityCheck: %d", sessionConstants->capabilities_suppress_presence_activity_check());
    LogToFile("CapabilitiesGameplay: %d", sessionConstants->capabilities_gameplay());
    LogToFile("CapabilitiesLarge: %d", sessionConstants->capabilities_large());
    LogToFile("CapabilitiesConnectionRequiredForActiveMembers: %d", sessionConstants->capabilities_connection_required_for_active_member());
    LogToFile("CapabilitiesUserAuthorizationStyle: %d", sessionConstants->capabilities_user_authorization_style());
    LogToFile("CapabilitiesCrossplay: %d", sessionConstants->capabilities_crossplay());
    LogToFile("CapabilitiesSearchable: %d", sessionConstants->capabilities_searchable());

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetVisibilityCpp_Lua(lua_State *L)
{
    xbox::services::multiplayer::multiplayer_session_visibility visibility = static_cast<xbox::services::multiplayer::multiplayer_session_visibility>(GetUint32FromLua(L, 1, 3));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_visibility(visibility);

    LogToFile("MultiplayerSessionSetVisibilityCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMaxMembersInSessionCpp_Lua(lua_State *L)
{
    uint32_t maxMembersInSession = GetUint32FromLua(L, 1, 10);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_max_members_in_session(maxMembersInSession);

    LogToFile("MultiplayerSessionSetMaxMembersInSessionCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetTimeoutsCpp_Lua(lua_State *L)
{
    std::chrono::milliseconds memberReservedTimeout{ GetUint64FromLua(L, 1, 100) };
    std::chrono::milliseconds memberInactiveTimeout{ GetUint64FromLua(L, 2, 100) };
    std::chrono::milliseconds memberReadyTimeout{ GetUint64FromLua(L, 3, 100) };
    std::chrono::milliseconds sessionEmptyTimeout{ GetUint64FromLua(L, 4, 100) };

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 5);
    sessionHandle->set_timeouts(memberReservedTimeout, memberInactiveTimeout, memberReadyTimeout, sessionEmptyTimeout);

    LogToFile("MultiplayerSessionSetTimeoutsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetQosConnectivityMetricsCpp_Lua(lua_State *L)
{
    bool enableLatencyMetric = GetBoolFromLua(L, 1, false);
    bool enableBandwidthDownMetric = GetBoolFromLua(L, 2, false);
    bool enableBandwidthUpMetric = GetBoolFromLua(L, 3, false);
    bool enableCustomMetric = GetBoolFromLua(L, 4, false);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 5);
    sessionHandle->set_quality_of_service_connectivity_metrics(enableLatencyMetric, enableBandwidthDownMetric, enableBandwidthUpMetric, enableCustomMetric);

    LogToFile("MultiplayerSessionSetQosConnectivityMetricsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMemberInitializationCpp_Lua(lua_State *L)
{
    std::chrono::milliseconds joinTimeout{ GetUint64FromLua(L, 1, 100) };
    std::chrono::milliseconds measurementTimeout{ GetUint64FromLua(L, 2, 100) };
    std::chrono::milliseconds evaluationTimeout{ GetUint64FromLua(L, 3, 100) };
    bool externalEvaluation = GetBoolFromLua(L, 4, false);
    uint32_t membersNeededToStart = GetUint32FromLua(L, 5, 2);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 6);
    sessionHandle->set_member_initialization(joinTimeout, measurementTimeout, evaluationTimeout, externalEvaluation, membersNeededToStart);

    LogToFile("MultiplayerSessionSetMemberInitializationCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetPeerToPeerRequirementsCpp_Lua(lua_State *L)
{
    std::chrono::milliseconds latencyMaximum{ GetUint64FromLua(L, 1, 100) };
    uint32_t bandwidthMinimumInKbps = GetUint32FromLua(L, 2, 100);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    sessionHandle->set_peer_to_peer_requirements(latencyMaximum, bandwidthMinimumInKbps);

    LogToFile("MultiplayerSessionSetPeerToPeerRequirementsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetPeerToHostRequirementsCpp_Lua(lua_State *L)
{
    std::chrono::milliseconds latencyMaximum{ GetUint64FromLua(L, 1, 100) };
    uint32_t bandwidthDownMinimumInKbps = GetUint32FromLua(L, 2, 10);
    uint32_t bandwidthUpMinimumInKbps = GetUint32FromLua(L, 3, 10);
    xbox::services::multiplayer::multiplay_metrics hostSelectionMetric = static_cast<xbox::services::multiplayer::multiplay_metrics>(GetUint64FromLua(L, 4, 1));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 5);
    sessionHandle->set_peer_to_host_requirements(latencyMaximum, bandwidthDownMinimumInKbps, bandwidthUpMinimumInKbps, hostSelectionMetric);

    LogToFile("MultiplayerSessionSetPeerToHostRequirementsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetSessionCapabilitiesCpp_Lua(lua_State *L)
{
    xbox::services::multiplayer::multiplayer_session_capabilities caps = {};
    caps.set_connectivity(GetBoolFromLua(L, 1, false));
    caps.set_suppress_presence_activity_check(GetBoolFromLua(L, 4, false));
    caps.set_gameplay(GetBoolFromLua(L, 5, false));
    caps.set_large(GetBoolFromLua(L, 6, true));
    caps.set_connection_required_for_active_members(GetBoolFromLua(L, 7, false));
    caps.set_user_authorization_style(GetBoolFromLua(L, 8, false));
    caps.set_crossplay(GetBoolFromLua(L, 9, false));
    caps.set_searchable(GetBoolFromLua(L, 10, false));
    caps.set_has_owners(GetBoolFromLua(L, 11, false));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 12);
    sessionHandle->set_session_capabilities(caps);

    LogToFile("MultiplayerSessionSetSessionCapabilitiesCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCloudComputePackageJsonCpp_Lua(lua_State *L)
{
    string_t sessionCloudComputePackageConstantsJsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "{}").c_str());
    web::json::value sessionCloudComputePackageConstantsJson = web::json::value::parse(sessionCloudComputePackageConstantsJsonString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_cloud_compute_package_json(sessionCloudComputePackageConstantsJson);

    LogToFile("MultiplayerSessionSetCloudComputePackageJsonCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionPropertiesCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_properties> props = sessionHandle->session_properties();

    LogToFile("MultiplayerSessionSessionPropertiesCpp");
    LogToFile("KeywordCount: %ul", props->keywords().size());
    for (size_t i = 0; i < props->keywords().size(); i++)
    {
        LogToFile("Keywords[%ul]: %s", i, xbox::services::Utils::StringFromStringT(props->keywords()[i]).c_str());
    }
    LogToFile("JoinRestriction: %d", props->join_restriction());
    LogToFile("ReadRestriction: %d", props->read_restriction());
    LogToFile("TurnCollectionCount: %ul", props->turn_collection().size());
    for (size_t i = 0; i < props->turn_collection().size(); i++)
    {
        LogToFile("TurnCollection[%d]: %x", i, props->turn_collection()[i].get());
    }
    LogToFile("MatchmakingTargetSessionConstantsJson: %s", xbox::services::Utils::StringFromStringT(props->matchmaking_target_session_constants_json().serialize()).c_str());
    LogToFile("SessionCustomPropertiesJson: %s", xbox::services::Utils::StringFromStringT(props->session_custom_properties_json().serialize()).c_str());
    LogToFile("MatchmakingServerConnectionString: %s", xbox::services::Utils::StringFromStringT(props->matchmaking_server_connection_string()).c_str());
    LogToFile("ServerConnectionStringCandidatesCount: %ul", props->server_connection_string_candidates().size());
    for (size_t i = 0; i < props->server_connection_string_candidates().size(); i++)
    {
        LogToFile("ServerConnectionStringCandidates[%ul]: %s", i, xbox::services::Utils::StringFromStringT(props->server_connection_string_candidates()[i]).c_str());
    }
    LogToFile("HostDeviceToken: %s", xbox::services::Utils::StringFromStringT(props->host_device_token()).c_str());
    LogToFile("Closed: %d", props->closed());
    LogToFile("Locked: %d", props->locked());
    LogToFile("AllocateCloudCompute: %d", props->allocate_cloud_compute());

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionPropertiesSetKeywordsCpp_Lua(lua_State *L)
{
    string_t key1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "Keyword1").c_str());
    string_t key2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "Keyword2").c_str());

    std::vector<string_t> keywords{ key1, key2 };

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    sessionHandle->session_properties()->set_keywords(keywords);

    LogToFile("MultiplayerSessionPropertiesSetKeywordsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionPropertiesSetJoinRestrictionCpp_Lua(lua_State *L)
{
    auto joinRestriction = static_cast<xbox::services::multiplayer::multiplayer_session_restriction>(GetUint64FromLua(L, 1, static_cast<int>(xbox::services::multiplayer::multiplayer_session_restriction::followed)));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->session_properties()->set_join_restriction(joinRestriction);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionPropertiesSetJoinRestrictionCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionPropertiesSetReadRestrictionCpp_Lua(lua_State *L)
{
    auto readRestriction = static_cast<xbox::services::multiplayer::multiplayer_session_restriction>(GetUint64FromLua(L, 1, static_cast<int>(xbox::services::multiplayer::multiplayer_session_restriction::followed)));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->session_properties()->set_read_restriction(readRestriction);

    LogToFile("MultiplayerSessionPropertiesSetReadRestrictionCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionRoleTypesCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_role_types> roleTypes = sessionHandle->session_role_types();

    LogToFile("MultiplayerSessionRoleTypesCpp");
    return LuaReturnHR(L, S_OK);
}

void LogSessionMember(const xbox::services::multiplayer::multiplayer_session_member* member)
{
    LogToFile("member->MemberId: %d", member->member_id());
    LogToFile("member->InitialTeam: %s", xbox::services::Utils::StringFromStringT(member->initial_team()).c_str());
    LogToFile("member->Xuid: %s", xbox::services::Utils::StringFromStringT(member->xbox_user_id()).c_str());
    LogToFile("member->CustomConstantsJson: %s", xbox::services::Utils::StringFromStringT(member->member_custom_constants_json().serialize()).c_str());
    LogToFile("member->SecureDeviceBaseAddress64: %s", xbox::services::Utils::StringFromStringT(member->secure_device_base_address64()).c_str());
    for (auto role : member->roles())
    {
        LogToFile("member->roles %s : %s", xbox::services::Utils::StringFromStringT(role.first).c_str(), xbox::services::Utils::StringFromStringT(role.second).c_str());
    }
    LogToFile("member->RolesCount: %ul", member->roles().size());
    LogToFile("member->CustomPropertiesJson: %s", xbox::services::Utils::StringFromStringT(member->member_custom_properties_json().serialize()).c_str());
    LogToFile("member->Gamertag: %s", xbox::services::Utils::StringFromStringT(member->gamertag()).c_str());
    LogToFile("member->XblMultiplayerSessionMemberStatus: %d", member->status());
    LogToFile("member->IsTurnAvailable: %d", member->is_turn_available());
    LogToFile("member->IsCurrentUser: %d", member->is_current_user());
    LogToFile("member->InitializeRequested: %d", member->initialize_requested());
    LogToFile("member->MatchmakingResultServerMeasurementsJson: %s", xbox::services::Utils::StringFromStringT(member->matchmaking_result_server_measurements_json().serialize()).c_str());
    LogToFile("member->ServerMeasurementsJson: %s", xbox::services::Utils::StringFromStringT(member->member_server_measurements_json().serialize()).c_str());
    for (size_t i = 0; i < member->members_in_group().size(); i++)
    {
        LogToFile("member->MembersInGroupIds[%d]: %ul", i, member->members_in_group()[i]->member_id());
    }
    LogToFile("member->MembersInGroupCount: %ul", member->members_in_group().size());
    LogToFile("member->DeviceToken: %s", xbox::services::Utils::StringFromStringT(member->device_token()).c_str());
    LogToFile("member->Nat: %d", member->nat());
    LogToFile("member->ActiveTitleId: %d", member->active_title_id());
    LogToFile("member->InitializationEpisode: %d", member->initialization_episode());
    LogToFile("member->JoinTime: %s", xbox::services::Utils::StringFromStringT(member->join_time().to_string()).c_str());
    LogToFile("member->InitializationFailureCause: %d", member->initialization_failure_cause());
    for (size_t i = 0; i < member->groups().size(); i++)
    {
        LogToFile("member->Groups[%d]: %s", i, xbox::services::Utils::StringFromStringT(member->groups()[i]).c_str());
    }
    LogToFile("member->GroupsCount: %ul", member->groups().size());
    for (size_t i = 0; i < member->encounters().size(); i++)
    {
        LogToFile("member->Encounters[%d]: %s", i, xbox::services::Utils::StringFromStringT(member->encounters()[i]).c_str());
    }
    LogToFile("member->EncountersCount: %ul", member->encounters().size());
}

int MultiplayerSessionMembersCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    std::vector<std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>> members = sessionHandle->members();

    LogToFile("MultiplayerSessionMembersCpp");
    for (auto member : members)
    {
        LogSessionMember(member.get());
    }
    LogToFile("membersCount: %d", members.size());

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetMemberCpp_Lua(lua_State *L)
{
    uint32_t memberId = GetUint32FromLua(L, 1, 0);

    LogToFile("MultiplayerSessionGetMemberCpp");
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    for (auto member : sessionHandle->members())
    {
        if (member->member_id() == memberId)
        {
            LogSessionMember(member.get());
        }
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionMatchmakingServerCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    xbox::services::multiplayer::multiplayer_session_matchmaking_server server = sessionHandle->matchmaking_server();

    LogToFile("MultiplayerSessionMatchmakingServerCpp");
    if (server.is_null())
    {
        LogToFile("server is null");
        return LuaReturnHR(L, S_OK);
    }

    LogToFile("server.status: %d", server.status());
    LogToFile("server.status_details: %s", xbox::services::Utils::StringFromStringT(server.status_details()).c_str());
    LogToFile("server.typical_wait: %d", server.typical_wait());
    auto sessionRef = server.target_session_ref();
    LogSessionRef(&sessionRef);

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionMembersAcceptedCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    uint32_t membersAccepted = sessionHandle->members_accepted();

    LogToFile("MultiplayerSessionMembersAcceptedCpp");
    LogToFile("membersAccepted: %d", membersAccepted);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionServersJsonCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    string_t serversJsonString = sessionHandle->servers_json().serialize();

    LogToFile("MultiplayerSessionServersJsonCpp");
    LogToFile("json: %s", xbox::services::Utils::StringFromStringT(serversJsonString).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetServersJsonCpp_Lua(lua_State *L)
{
    string_t jsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "{}").c_str());
    web::json::value json = web::json::value::parse(jsonString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_servers_json(json);

    LogToFile("MultiplayerSessionSetServersJsonCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionEtagCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    string_t etag = sessionHandle->e_tag();

    LogToFile("MultiplayerSessionEtagCpp");
    LogToFile("etag: %s", xbox::services::Utils::StringFromStringT(etag).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCurrentUserCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> currentUser = sessionHandle->current_user();


    LogToFile("MultiplayerSessionCurrentUserCpp");
    if (currentUser == nullptr)
    {
        LogToFile("currentUser == nullptr");
        return LuaReturnHR(L, S_OK);
    }

    LogSessionMember(currentUser.get());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetInfoCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);

    LogToFile("MultiplayerSessionGetInfoCpp:");
    LogToFile("branch: %s", xbox::services::Utils::StringFromStringT(sessionHandle->branch()).c_str());
    LogToFile("change_number: %ul", sessionHandle->change_number());
    LogToFile("multiplayer_correlation_id: %s", xbox::services::Utils::StringFromStringT(sessionHandle->multiplayer_correlation_id()).c_str());
    LogToFile("start_time: %s", xbox::services::Utils::StringFromStringT(sessionHandle->start_time().to_string()).c_str());
    LogToFile("date_of_next_timer: %s", xbox::services::Utils::StringFromStringT(sessionHandle->date_of_next_timer().to_string()).c_str());
    LogToFile("search_handle_id: %s", xbox::services::Utils::StringFromStringT(sessionHandle->search_handle_id()).c_str());

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionWriteStatusCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    xbox::services::multiplayer::write_session_status writeStatus = sessionHandle->write_status();

    LogToFile("MultiplayerSessionWriteStatusCpp");
    LogToFile("writeStatus: %d", writeStatus);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetInitializationStatusCpp_Lua(lua_State *L)
{
    bool initSucceded = GetBoolFromLua(L, 1, false);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_initialization_status(initSucceded);

    LogToFile("MultiplayerSessionSetInitializationStatusCpp %d", initSucceded);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetHostDeviceTokenCpp_Lua(lua_State *L)
{
    string_t hostDeviceToken = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "DefaultHost").c_str());

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_host_device_token(hostDeviceToken);

    LogToFile("MultiplayerSessionSetHostDeviceTokenCpp host:%s", xbox::services::Utils::StringFromStringT(hostDeviceToken).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMatchmakingServerConnectionPathCpp_Lua(lua_State *L)
{
    string_t path = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "DefaultPath").c_str());

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_matchmaking_server_connection_path(path);

    LogToFile("MultiplayerSessionSetMatchmakingServerConnectionPathCpp path:%s", xbox::services::Utils::StringFromStringT(path).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetClosedCpp_Lua(lua_State *L)
{
    bool closed = GetBoolFromLua(L, 1, true);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_closed(closed);

    LogToFile("MultiplayerSessionSetClosedCpp %d", closed);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetLockedCpp_Lua(lua_State *L)
{
    bool locked = GetBoolFromLua(L, 1, false);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_locked(locked);

    LogToFile("MultiplayerSessionSetLockedCpp %d", locked);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetAllocateCloudComputeCpp_Lua(lua_State *L)
{
    bool allocate = GetBoolFromLua(L, 1, false);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_allocate_cloud_compute(allocate);

    LogToFile("MultiplayerSessionSetAllocateCloudComputeCpp %d", allocate);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMatchmakingResubmitCpp_Lua(lua_State *L)
{
    bool matchResubmit = GetBoolFromLua(L, 1, false);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    sessionHandle->set_matchmaking_resubmit(matchResubmit);

    LogToFile("MultiplayerSessionSetMatchmakingResubmitCpp %d", matchResubmit);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetServerConnectionStringCandidatesCpp_Lua(lua_State *L)
{
    string_t candidate1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "Candidate1").c_str());
    string_t candidate2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "Candidate2").c_str());
    std::vector<string_t> candidates{ candidate1, candidate2 };


    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    sessionHandle->set_server_connection_string_candidates(candidates);

    LogToFile("MultiplayerSessionSetServerConnectionStringCandidatesCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetSessionChangeSubscriptionCpp_Lua(lua_State *L)
{
    auto changeTypes = static_cast<xbox::services::multiplayer::multiplayer_session_change_types>(GetUint32FromLua(L, 1, static_cast<int>(xbox::services::multiplayer::multiplayer_session_change_types::everything)));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->set_session_change_subscription(changeTypes);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetSessionChangeSubscriptionCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionLeaveCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    std::error_code err = sessionHandle->leave();
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionLeaveCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetCurrentUserStatusCpp_Lua(lua_State *L)
{
    auto status = static_cast<xbox::services::multiplayer::multiplayer_session_member_status>(GetUint32FromLua(L, 1, 3));

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");
    std::error_code err = sessionHandle->set_current_user_status(status);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetCurrentUserStatusCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp_Lua(lua_State *L)
{
    string_t deviceAddress = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "ExampleDeviceAddress").c_str());

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");
    std::error_code err = sessionHandle->set_current_user_secure_device_address_base64(deviceAddress);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetCurrentUserRolesCpp_Lua(lua_State *L)
{
    string_t roleTypeName1 = _T("roleTypeName1");
    string_t roleName1 = _T("roleName1");
    string_t roleTypeName2 = _T("roleTypeName2");
    string_t roleName2 = _T("roleName2");

    std::unordered_map<string_t, string_t> roleInfo{
        std::pair<string_t, string_t>(roleTypeName1, roleName1),
        std::pair<string_t, string_t>(roleTypeName2, roleName2) };

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    std::error_code err = sessionHandle->set_current_user_role_info(roleInfo);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetCurrentUserRolesCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionCurrentUserSetGroupsCpp_Lua(lua_State *L)
{
    string_t group1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "group1").c_str());
    string_t group2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "group2").c_str());
    std::vector<string_t> groups{ group1, group2 };

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");
    sessionHandle->current_user()->set_groups(groups);

    LogToFile("MultiplayerSessionCurrentUserSetGroupsCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCurrentUserSetEncountersCpp_Lua(lua_State *L)
{
    string_t encounter1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "encounter1").c_str());
    string_t encounter2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "encounter2").c_str());
    std::vector<string_t> encounters{ encounter1, encounter2 };

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");
    sessionHandle->current_user()->set_encounters(encounters);

    LogToFile("MultiplayerSessionCurrentUserSetEncountersCpp");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserQosMeasurementsJsonCpp_Lua(lua_State *L)
{
    string_t measurementsJsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "{\"measurements1\":5}").c_str());
    web::json::value measurementsJson = web::json::value::parse(measurementsJsonString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->set_current_user_quality_of_service_measurements_json(measurementsJson);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetCurrentUserQosMeasurementsCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp_Lua(lua_State *L)
{
    string_t name = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "name1").c_str());
    string_t jsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "{\"myscore\":123}").c_str());
    web::json::value json = web::json::value::parse(jsonString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");
    std::error_code err = sessionHandle->set_current_user_member_custom_property_json(name, json);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp_Lua(lua_State *L)
{
    string_t name = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "name1").c_str());

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->delete_current_user_member_custom_property_json(name);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp_Lua(lua_State *L)
{
    string_t constsString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "{}").c_str());
    web::json::value consts = web::json::value::parse(constsString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->set_matchmaking_target_session_constants_json(consts);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionSetSessionCustomPropertyJsonCpp_Lua(lua_State *L)
{
    string_t name = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "name1").c_str());
    string_t jsonString = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "{}").c_str());
    web::json::value json = web::json::value::parse(jsonString);

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);
    std::error_code err = sessionHandle->set_session_custom_property_json(name, json);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionSetSessionCustomPropertyJsonCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionDeleteSessionCustomPropertyJsonCpp_Lua(lua_State *L)
{
    string_t name = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "name1").c_str());

    auto sessionHandle = GetSessionHandleFromArgCpp(L, 2);
    std::error_code err = sessionHandle->delete_session_custom_property_json(name);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);

    LogToFile("MultiplayerSessionDeleteSessionCustomPropertyJsonCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int MultiplayerSessionCompareCpp_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1);
    auto sessionHandle2 = sessionHandle;

    xbox::services::xbox_live_result<xbox::services::multiplayer::multiplayer_session_change_types> result = xbox::services::multiplayer::multiplayer_session::compare_multiplayer_sessions(sessionHandle, sessionHandle2);
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("MultiplayerSessionCompareCpp: hr=%s", ConvertHR(hr).c_str());
    if (SUCCEEDED(hr))
    {
        xbox::services::multiplayer::multiplayer_session_change_types changeTypes = result.payload();
        LogToFile("changeTypes: %d", changeTypes);
    }

    return LuaReturnHR(L, hr);
}

//multiplayer_service

int MultiplayerServiceWriteSession_Lua(lua_State *L)
{
    uint64_t sessionIndex;
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 1, &sessionIndex);
    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    auto sessionWriteMode = ConvertStringToCppMultiplayerSessionWriteMode(GetStringFromLua(L, 2, "multiplayer_session_write_mode::update_or_create_new").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().write_session(
        sessionHandle, 
        sessionWriteMode
    ).then(
        [sessionIndex](xbox::services::xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceWriteSession: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                MPStateCpp()->sessionHandles[static_cast<uint32_t>(sessionIndex)] = result.payload();
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceWriteSession");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceWriteSessionByHandle_Lua(lua_State *L)
{
    auto writeMode = static_cast<xbox::services::multiplayer::multiplayer_session_write_mode>(GetUint32FromLua(L, 1, static_cast<int>(xbox::services::multiplayer::multiplayer_session_write_mode::update_existing)));
    string_t handleId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, MPStateCpp()->activityHandle).c_str());
    auto sessionHandle = GetSessionHandleFromArgCpp(L, 3);

    LogToFile("MultiplayerServiceWriteSessionByHandle");
    LogToFile("writeMode: %d", writeMode);
    LogToFile("handleId: %s", xbox::services::Utils::StringFromStringT(handleId).c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().write_session_by_handle(
        sessionHandle, 
        writeMode, 
        handleId
    ).then(
        [](xbox::services::xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceWriteSessionByHandle: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session = result.payload();
                UNREFERENCED_PARAMETER(session);
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceWriteSessionByHandle");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetCurrentSession_Lua(lua_State *L)
{
    string_t scid = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, Data()->scid).c_str());
    string_t sessionTemplateName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "MinGameSession").c_str());
    uint64_t sessionIndex{ GetUint64FromLua(L, 4, 0) };
    string_t sessionName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, MultiplayerStateCpp::GetSessionName(sessionIndex)).c_str());

    xbox::services::multiplayer::multiplayer_session_reference sessionRef{ scid, sessionTemplateName, sessionName };

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_current_session(sessionRef).then(
        [sessionIndex](xbox::services::xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetCurrentSession: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                MPStateCpp()->sessionHandles[static_cast<uint32_t>(sessionIndex)] = result.payload();
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetCurrentSession");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetCurrentSessionByHandle_Lua(lua_State *L)
{
    string_t handleId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, MPStateCpp()->inviteHandle).c_str());
    auto sessionIndex{ GetUint64FromLua(L, 2, 0) };
    if (handleId.empty())
    {
        handleId = _T("86191619-4002-044f-4846-f8f903c71512");
    }

    LogToFile("handleId: %s", xbox::services::Utils::StringFromStringT(handleId).c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_current_session_by_handle(handleId).then(
        [sessionIndex](xbox::services::xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetCurrentSessionByHandle: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                MPStateCpp()->sessionHandles[static_cast<uint32_t>(sessionIndex)] = result.payload();
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetCurrentSessionByHandle");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetSessions_Lua(lua_State *L)
{
    string_t scid = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, Data()->scid).c_str());
    uint32_t maxItems = GetUint32FromLua(L, 2, 0);

    bool includePrivateSessions = GetBoolFromLua(L, 3, false);
    bool includeReservations = GetBoolFromLua(L, 4, false);
    bool includeInactiveSessions = GetBoolFromLua(L, 5, false);
    string_t keywordFilter = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 6, "killzone").c_str());
    string_t sessionTemplateNameFilter = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 7, "").c_str());
    xbox::services::multiplayer::multiplayer_session_visibility visibilityFilter = static_cast<xbox::services::multiplayer::multiplayer_session_visibility>(GetUint32FromLua(L, 8, 5));
    uint32_t contractVersionFilter = GetUint32FromLua(L, 9, 0);

    xbox::services::multiplayer::multiplayer_get_sessions_request request(scid, maxItems);
    request.set_include_private_sessions(includePrivateSessions);
    request.set_include_reservations(includeReservations);
    request.set_include_inactive_sessions(includeInactiveSessions);
    request.set_keyword_filter(keywordFilter);
    request.set_session_template_name_filter(sessionTemplateNameFilter);
    request.set_visibility_filter(visibilityFilter);
    request.set_contract_version_filter(contractVersionFilter);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_sessions(request).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::multiplayer::multiplayer_session_states>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetSessions: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::multiplayer::multiplayer_session_states> sessionStates = result.payload();
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetSessions");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetActivity_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().set_activity(MPStateCpp()->sessionRef).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceSetActivity: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceSetActivity");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceClearActivity_Lua(lua_State *L)
{
    string_t serviceConfigurationID = xbox::services::Utils::StringTFromUtf8(Data()->scid);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().clear_activity(serviceConfigurationID).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceClearActivity: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceClearActivity");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSendInvites_Lua(lua_State *L)
{
    string_t contextStringId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "contextStringId1").c_str());
    string_t customActivationContext = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "customActivationContext1").c_str());
    string_t targetXuid = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 3, 2814679169942680));

    xbox::services::multiplayer::multiplayer_session_reference sessionReference = MPStateCpp()->sessionRef;
    std::vector<string_t> xuids{ targetXuid };
    uint32_t titleId = Data()->titleId;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().send_invites(
        sessionReference,
        xuids,
        titleId,
        contextStringId,
        customActivationContext
    ).then(
        [](xbox::services::xbox_live_result<std::vector<string_t>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceSendInvites: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<string_t> inviteHandles = result.payload();
                LogToFile("MultiplayerServiceSendInvites got %d invite handles", inviteHandles.size());
                MPStateCpp()->inviteHandle = xbox::services::Utils::StringFromStringT(inviteHandles[0]);
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceSendInvites");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetActivitiesForSocialGroup_Lua(lua_State *L)
{
    string_t serviceConfigurationID = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    string_t socialGroupOwnerXuid = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 1, 2814632956486799));
    string_t socialGroup = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "people").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_activities_for_social_group(
        serviceConfigurationID,
        socialGroupOwnerXuid,
        socialGroup
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::multiplayer::multiplayer_activity_details>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetActivitiesForSocialGroup: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::multiplayer::multiplayer_activity_details> activityDetails = result.payload();
                UNREFERENCED_PARAMETER(activityDetails);
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetActivitiesForSocialGroup");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetActivitiesForUsers_Lua(lua_State *L)
{
    uint64_t xuid1 = GetUint64FromLua(L, 1, Data()->m_multiDeviceManager->GetRemoteXuid());
    if (xuid1 == 0) xuid1 = 2814636782672891;

    string_t serviceConfigurationID = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    std::vector<string_t> xuids{ xbox::services::Utils::StringTFromUint64(xuid1) };

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_activities_for_users(
        serviceConfigurationID,
        xuids
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::multiplayer::multiplayer_activity_details>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetActivitiesForUsers: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::multiplayer::multiplayer_activity_details> activityDetails = result.payload();

                if (activityDetails.size() > 0)
                {
                    std::string handleIdStr = xbox::services::Utils::StringFromStringT(activityDetails[0].handle_id());
                    LogToScreen("Joining lobby via handle %s", handleIdStr.c_str());
                    MPStateCpp()->activityHandle = handleIdStr; // CODE SNIP SKIP
                }
                else
                {
                    if (Data()->m_multiDeviceManager->GetRemoteXuid() != 0)
                    {
                        LogToScreen("No activity handle to join.  Failing...");
                        hr = E_FAIL;
                    }
                }
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetActivitiesForUsers");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceEnableMultiplayerSubscriptions_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    std::error_code err = xblc->multiplayer_service().enable_multiplayer_subscriptions();

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(err);
    LogToFile("MultiplayerServiceEnableMultiplayerSubscriptions: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int MultiplayerServiceDisableMultiplayerSubscriptions_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().disable_multiplayer_subscriptions();
    LogToFile("MultiplayerServiceDisableMultiplayerSubscriptions");

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceMultiplayerSubscriptionsEnabled_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    bool enabled = xblc->multiplayer_service().subscriptions_enabled();

    LogToFile("MultiplayerServiceMultiplayerSubscriptionsEnabled");
    LogToFile("enabled: %d", enabled);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerSessionChangedHandler_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    function_context fnContext = xblc->multiplayer_service().add_multiplayer_session_changed_handler(
        [](const xbox::services::multiplayer::multiplayer_session_change_event_args& args)
        {
            LogToFile("MultiplayerServiceAddMultiplayerSessionChangedHandler");
            LogToFile("ChangeNumber: %d", args.change_number());
            CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceAddMultiplayerSessionChangedHandler");
        });

    MPStateCpp()->sessionChangedContext = fnContext;
    LogToFile("MultiplayerServiceAddMultiplayerSessionChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerSessionChangedHandler_Lua(lua_State *L)
{
    function_context fnContext = MPStateCpp()->sessionChangedContext;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().remove_multiplayer_session_changed_handler(fnContext);
    MPStateCpp()->sessionChangedContext = nullptr;

    LogToFile("MultiplayerServiceRemoveMultiplayerSessionChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerSubscriptionLostHandler_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    function_context fnContext = xblc->multiplayer_service().add_multiplayer_subscription_lost_handler(
        []()
        {
            LogToFile("MultiplayerServiceAddMultiplayerSubscriptionLostHandler");
            CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceAddMultiplayerSubscriptionLostHandler");
        });

    MPStateCpp()->subscriptionLostContext = fnContext;
    LogToFile("MultiplayerServiceAddMultiplayerSubscriptionLostHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler_Lua(lua_State *L)
{
    function_context fnContext = MPStateCpp()->subscriptionLostContext;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().remove_multiplayer_subscription_lost_handler(fnContext);
    MPStateCpp()->subscriptionLostContext = nullptr;

    LogToFile("MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerConnectionIdChangedHandler_Lua(lua_State *L)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    function_context fnContext = xblc->multiplayer_service().add_multiplayer_connection_id_changed_handler(
        []()
        {
            LogToFile("MultiplayerServiceAddMultiplayerConnectionIdChangedHandler");
            CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceAddMultiplayerConnectionIdChangedHandler");
        });

    MPStateCpp()->connectionIdChangedContext = fnContext;
    LogToFile("MultiplayerServiceAddMultiplayerConnectionIdChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler_Lua(lua_State *L)
{
    function_context fnContext = MPStateCpp()->connectionIdChangedContext;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().remove_multiplayer_connection_id_changed_handler(fnContext);
    MPStateCpp()->connectionIdChangedContext = nullptr;

    LogToFile("MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetTransferHandle_Lua(lua_State* L)
{
    uint64_t targetIndex{ GetUint64FromLua(L, 1, 0) };
    uint64_t originIndex{ GetUint64FromLua(L, 2, 0) };

    auto targetReference = MPStateCpp()->sessionHandles[static_cast<uint32_t>(targetIndex)]->session_reference();
    auto originReference = MPStateCpp()->sessionHandles[static_cast<uint32_t>(originIndex)]->session_reference();

    if (!originReference.is_null() && !targetReference.is_null())
    {
        std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
        xblc->multiplayer_service().set_transfer_handle(
            targetReference,
            originReference
        ).then(
            [](xbox::services::xbox_live_result<string_t> result)
            {
                HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
                LogToFile("MultiplayerServiceSetTransferHandle: hr=%s", ConvertHR(hr).c_str());

                if (SUCCEEDED(hr))
                {
                    string_t transferId = result.payload();
                    LogToFile("Successfully set transfer handle, ID = %s", xbox::services::Utils::StringFromStringT(transferId).c_str());
                }

                CallLuaFunctionWithHr(hr, "OnMultiplayerServiceSetTransferHandle");
            });
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetSearchHandle_Lua(lua_State *L)
{
    std::vector<string_t> tags{ _T("SessionTag") };
    std::unordered_map<string_t, double> numbersMetadata{ std::pair<string_t, double>(_T("numberattributename"), 1.1) };
    std::unordered_map<string_t, string_t> stringsMetadata{ std::pair<string_t, string_t>(_T("stringattributename"), _T("string attribute value")) };

    xbox::services::multiplayer::multiplayer_session_reference sessionRef = MPStateCpp()->sessionRef;
    xbox::services::multiplayer::multiplayer_search_handle_request searchRequest(sessionRef);
    searchRequest.set_tags(tags);
    searchRequest.set_numbers_metadata(numbersMetadata);
    searchRequest.set_strings_metadata(stringsMetadata);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().set_search_handle(searchRequest).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceSetSearchHandle: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {

            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceSetSearchHandle");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceClearSearchHandle_Lua(lua_State *L)
{
    string_t handleId;
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().clear_search_handle(handleId).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceClearSearchHandle: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceClearSearchHandle");
        });

    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetSearchHandles_Lua(lua_State *L)
{
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    bool orderAscending{ false };
    string_t sessionTemplateName = _T("MinGameSession");
    string_t orderByAttribute = _T("");
    string_t searchFilter = _T("");

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->multiplayer_service().get_search_handles(
        serviceConfigurationId,
        sessionTemplateName,
        orderByAttribute,
        orderAscending,
        searchFilter
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::multiplayer::multiplayer_search_handle_details>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MultiplayerServiceGetSearchHandles: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::multiplayer::multiplayer_search_handle_details> searchHandles = result.payload();
                LogToFile("Got %u search handles:", searchHandles.size());
                for (auto searchHandle : searchHandles)
                {
                    LogToFile("\t%s", xbox::services::Utils::StringFromStringT(searchHandle.handle_id()).c_str());
                }

                if (searchHandles.size() > 0)
                {
                    MPStateCpp()->searchHandleDetails = std::make_shared< xbox::services::multiplayer::multiplayer_search_handle_details>(searchHandles[0]);
                }
            }

            CallLuaFunctionWithHr(hr, "OnMultiplayerServiceGetSearchHandles");
        });

    return LuaReturnHR(L, S_OK);
}

//multiplayer_search_handle_detail

int MultiplayerSearchHandleDetailsCloseHandle_Lua(lua_State *L)
{
    MPStateCpp()->searchHandleDetails = nullptr;

    LogToFile("MultiplayerSearchHandleDetailsCloseHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsSessionReference_Lua(lua_State *L)
{
    auto sessionRef = MPStateCpp()->searchHandleDetails->session_reference();

    LogToFile("MultiplayerSearchHandleDetailsSessionReference");
    LogSessionRef(&sessionRef);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsHandleId_Lua(lua_State *L)
{
    string_t handleId = MPStateCpp()->searchHandleDetails->handle_id();

    LogToFile("MultiplayerSearchHandleDetailsHandleId");
    LogToFile("Search Handle Id: %s", xbox::services::Utils::StringFromStringT(handleId).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsSessionOwnerXuids_Lua(lua_State *L)
{
    std::vector<string_t> xuids;
    if (MPStateCpp()->searchHandleDetails)
    {
        xuids = MPStateCpp()->searchHandleDetails->session_owner_xbox_user_ids();
    }

    LogToFile("MultiplayerSearchHandleDetailsSessionOwnerXuids");
    LogToFile("There are %u session owners:", xuids.size());
    for (string_t xuid : xuids)
    {
        LogToFile("\t%s", xbox::services::Utils::StringFromStringT(xuid).c_str());
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsTags_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    std::vector<string_t> tags = MPStateCpp()->searchHandleDetails->tags();
    
    LogToFile("MultiplayerSearchHandleDetailsTags");
    LogToFile("There are %u tags for the session:", tags.size());
    for (string_t tag : tags)
    {
        LogToFile("\t%s", xbox::services::Utils::StringFromStringT(tag).c_str());
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsStringsMetadata_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    auto stringsMetadata = MPStateCpp()->searchHandleDetails->strings_metadata();

    LogToFile("MultiplayerSearchHandleDetailsStringsMetadata");
    LogToFile("There are %u string metadata for the session:", stringsMetadata.size());
    for (std::pair<string_t, string_t> stringMetadata : stringsMetadata)
    {
        LogToFile("\t%s : %s", xbox::services::Utils::StringFromStringT(stringMetadata.first).c_str(), xbox::services::Utils::StringFromStringT(stringMetadata.second).c_str());
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsNumbersMetadata_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    auto numbersMetadata = MPStateCpp()->searchHandleDetails->numbers_metadata();

    LogToFile("MultiplayerSearchHandleDetailsNumbersMetadata");
    LogToFile("There are %u number metadata for the session:", numbersMetadata.size());
    for (std::pair<string_t, double> numberMetadata : numbersMetadata)
    {
        LogToFile("\t%s : %f", xbox::services::Utils::StringFromStringT(numberMetadata.first).c_str(), numberMetadata.second);
    }

    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsVisibility_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    xbox::services::multiplayer::multiplayer_session_visibility sessionVisibility = MPStateCpp()->searchHandleDetails->visibility();

    LogToFile("MultiplayerSearchHandleDetailsVisibility: visibility=%u", sessionVisibility);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsJoinRestriction_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    xbox::services::multiplayer::multiplayer_session_restriction joinRestriction = MPStateCpp()->searchHandleDetails->join_restriction();

    LogToFile("MultiplayerSearchHandleDetailsJoinRestriction: restriction=%u", joinRestriction);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsClosed_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    bool sessionClosed = MPStateCpp()->searchHandleDetails->closed();

    LogToFile("MultiplayerSearchHandleDetailsClosed: closed=%d", sessionClosed);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsMemberCounts_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    uint32_t membersCount = MPStateCpp()->searchHandleDetails->members_count();
    uint32_t maxMembersCount = MPStateCpp()->searchHandleDetails->max_members_count();

    LogToFile("MultiplayerSearchHandleDetailsMemberCounts: max members=%u, current members=%u ", maxMembersCount, membersCount);
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsHandleCreationTime_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    auto creationTime = MPStateCpp()->searchHandleDetails->handle_creation_time();

    LogToFile("MultiplayerSearchHandleDetailsHandleCreationTime: creation time=%s", xbox::services::Utils::StringFromStringT(creationTime.to_string()).c_str());
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsCustomSessionPropertiesJson_Lua(lua_State *L)
{
    if (MPStateCpp()->searchHandleDetails == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    string_t customPropertiesJsonStr = MPStateCpp()->searchHandleDetails->custom_session_properties_json().serialize();

    LogToFile("MultiplayerSearchHandleDetailsCustomSessionPropertiesJson: properties=%s", xbox::services::Utils::StringFromStringT(customPropertiesJsonStr).c_str());
    return LuaReturnHR(L, S_OK);
}

//matchmaking_service

int MatchmakingServiceCreateTicket_Lua(lua_State* L)
{
    string_t hopperName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "PlayerSkillNoQoS").c_str());
    string_t attributesJsonStr = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "{}").c_str());
    std::chrono::seconds timeoutInSeconds{ GetUint32FromLua(L, 6, 100) };

    auto sessionRef = MPStateCpp()->sessionRef;
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    xbox::services::matchmaking::preserve_session_mode preserveSession = xbox::services::matchmaking::preserve_session_mode::never;
    web::json::value attributesJson = web::json::value::parse(attributesJsonStr);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->matchmaking_service().create_match_ticket(
        sessionRef,
        serviceConfigurationId,
        hopperName,
        timeoutInSeconds,
        preserveSession,
        attributesJson
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::matchmaking::create_match_ticket_response> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MatchmakingServiceCreateTicket: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::matchmaking::create_match_ticket_response ticketResponse = result.payload();
                LogToScreen("create_match_ticket_response.match_ticket_id: %s", xbox::services::Utils::StringFromStringT(ticketResponse.match_ticket_id()).c_str());
                LogToScreen("create_match_ticket_response.estimated_wait_time: %d", ticketResponse.estimated_wait_time());
                Data()->matchTicketResponseCpp = ticketResponse;
            }

            CallLuaFunctionWithHr(hr, "OnMatchmakingServiceCreateTicket");
        });

    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceGetMatchTicketDetails_Lua(lua_State* L)
{
    string_t hopperName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "PlayerSkillNoQoS").c_str());
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, Data()->scid).c_str());
    string_t ticketId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, "").c_str());
    if (ticketId.empty())
    {
        ticketId = Data()->matchTicketResponseCpp.match_ticket_id();
    }

    if (Data()->xalUser == nullptr)
    {
        return LuaReturnHR(L, E_UNEXPECTED);
    }

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->matchmaking_service().get_match_ticket_details(
        serviceConfigurationId,
        hopperName,
        ticketId
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::matchmaking::match_ticket_details_response> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MatchmakingServiceGetMatchTicketDetails: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::matchmaking::match_ticket_details_response ticketDetailsResponse = result.payload();

                LogToScreen("match_ticket_details_response.match_status: %d", ticketDetailsResponse.match_status());
                LogToScreen("match_ticket_details_response.estimated_wait_time: %d", ticketDetailsResponse.estimated_wait_time());
                LogToScreen("match_ticket_details_response.preserve_session: %d", ticketDetailsResponse.preserve_session());
                LogToScreen("match_ticket_details_response.ticketSession: SCID: %s, Session Name: %s, Session Template Name: %s",
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.ticket_session().service_configuration_id()).c_str(),
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.ticket_session().session_name()).c_str(),
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.ticket_session().session_template_name()).c_str());
                LogToScreen("match_ticket_details_response.targetSession: SCID: %s, Session Name: %s, Session Template Name: %s",
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.target_session().service_configuration_id()).c_str(),
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.target_session().session_name()).c_str(),
                    xbox::services::Utils::StringFromStringT(ticketDetailsResponse.target_session().session_template_name()).c_str());

                if (!ticketDetailsResponse.ticket_attributes().is_null())
                {
                    LogToScreen("match_ticket_details_response.ticket_attributes: %d", xbox::services::Utils::StringFromStringT(ticketDetailsResponse.ticket_attributes().serialize()).c_str());
                }
            }

            CallLuaFunctionWithHr(hr, "OnMatchmakingServiceGetMatchTicketDetails");
        });

    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceGetHopperStatistics_Lua(lua_State* L)
{
    string_t hopperName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "PlayerSkillNoQoS").c_str());
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, Data()->scid).c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->matchmaking_service().get_hopper_statistics(serviceConfigurationId, hopperName).then(
        [](xbox::services::xbox_live_result<xbox::services::matchmaking::hopper_statistics_response> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MatchmakingServiceGetHopperStatistics: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::matchmaking::hopper_statistics_response hopperStatisticsResponse = result.payload();

                LogToScreen("hopper_statistics_response.hopper_name: %s", xbox::services::Utils::StringFromStringT(hopperStatisticsResponse.hopper_name()).c_str());
                LogToScreen("hopper_statistics_response.estimated_wait_time: %d", hopperStatisticsResponse.estimated_wait_time());
                LogToScreen("hopper_statistics_response.players_waiting_to_match: %d", hopperStatisticsResponse.players_waiting_to_match());
            }

            CallLuaFunctionWithHr(hr, "OnMatchmakingServiceGetHopperStatistics");
        });

    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceDeleteMatchTicket_Lua(lua_State* L)
{
    string_t hopperName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "PlayerSkillNoQoS").c_str());
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, Data()->scid).c_str());
    string_t ticketId = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, "").c_str());
    if (ticketId.empty())
    {
        ticketId = Data()->matchTicketResponseCpp.match_ticket_id();
    }

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->matchmaking_service().delete_match_ticket(
        serviceConfigurationId,
        hopperName,
        ticketId
    ).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("MatchmakingServiceDeleteMatchTicket: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnMatchmakingServiceDeleteMatchTicket");
        });

    return LuaReturnHR(L, S_OK);
}

#else //CPP_TESTS_ENABLED

//multiplayer_session_reference

int MultiplayerSessionReferenceIsValidCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionReferenceCreateCpp_Lua(lua_State* L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionReferenceParseFromUriPathCpp_Lua(lua_State* L)
{
    return LuaReturnHR(L, S_OK);
}

//multitplayer_session

int MultiplayerSessionCreateCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionJoinCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionAddMemberReservationCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionTimeOfSessionCpp_Lua(lua_State* L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetInitializationInfoCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSubscribedChangeTypesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionHostCandidatesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionReferenceCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionConstantsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetVisibilityCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMaxMembersInSessionCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetTimeoutsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetQosConnectivityMetricsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMemberInitializationCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetPeerToPeerRequirementsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetPeerToHostRequirementsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetSessionCapabilitiesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCloudComputePackageJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSessionPropertiesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionPropertiesSetKeywordsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionPropertiesSetJoinRestrictionCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionPropertiesSetReadRestrictionCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionRoleTypesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionMembersCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetMemberCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionMatchmakingServerCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionMembersAcceptedCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionServersJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetServersJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionEtagCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCurrentUserCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionGetInfoCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionWriteStatusCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetInitializationStatusCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetHostDeviceTokenCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMatchmakingServerConnectionPathCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetClosedCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetLockedCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetAllocateCloudComputeCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMatchmakingResubmitCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetServerConnectionStringCandidatesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetSessionChangeSubscriptionCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionLeaveCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserStatusCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserRolesCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCurrentUserSetGroupsCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCurrentUserSetEncountersCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserQosMeasurementsJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionSetSessionCustomPropertyJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionDeleteSessionCustomPropertyJsonCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSessionCompareCpp_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

//multiplayer_service

int MultiplayerServiceWriteSession_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceWriteSession");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceWriteSessionByHandle_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceWriteSessionByHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetCurrentSession_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetCurrentSession");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetCurrentSessionByHandle_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetCurrentSessionByHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetSessions_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetSessions");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetActivity_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceSetActivity");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceClearActivity_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceClearActivity");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSendInvites_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceSendInvites");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetActivitiesForSocialGroup_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetActivitiesForSocialGroup");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetActivitiesForUsers_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetActivitiesForUsers");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceEnableMultiplayerSubscriptions_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceDisableMultiplayerSubscriptions_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceMultiplayerSubscriptionsEnabled_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerSessionChangedHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerSessionChangedHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerSubscriptionLostHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceAddMultiplayerConnectionIdChangedHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetTransferHandle_Lua(lua_State* L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceSetTransferHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceSetSearchHandle_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceSetSearchHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceClearSearchHandle_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceClearSearchHandle");
    return LuaReturnHR(L, S_OK);
}

int MultiplayerServiceGetSearchHandles_Lua(lua_State *L)
{
    CallLuaFunctionWithHr(S_OK, "OnMultiplayerServiceGetSearchHandles");
    return LuaReturnHR(L, S_OK);
}

//multiplayer_search_handle_detail

int MultiplayerSearchHandleDetailsCloseHandle_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsSessionReference_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsHandleId_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsSessionOwnerXuids_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsTags_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsStringsMetadata_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsNumbersMetadata_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsVisibility_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsJoinRestriction_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsClosed_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsMemberCounts_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsHandleCreationTime_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

int MultiplayerSearchHandleDetailsCustomSessionPropertiesJson_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}

//matchmaking_service

int MatchmakingServiceCreateTicket_Lua(lua_State* L)
{
    CallLuaFunctionWithHr(S_OK, "OnMatchmakingServiceCreateTicket");
    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceGetMatchTicketDetails_Lua(lua_State* L)
{
    CallLuaFunctionWithHr(S_OK, "OnMatchmakingServiceGetMatchTicketDetails");
    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceGetHopperStatistics_Lua(lua_State* L)
{
    CallLuaFunctionWithHr(S_OK, "OnMatchmakingServiceGetHopperStatistics");
    return LuaReturnHR(L, S_OK);
}

int MatchmakingServiceDeleteMatchTicket_Lua(lua_State* L)
{
    CallLuaFunctionWithHr(S_OK, "OnMatchmakingServiceDeleteMatchTicket");
    return LuaReturnHR(L, S_OK);
}

#endif //CPP_TESTS_ENABLED

void SetupAPIs_CppMultiplayer()
{
    //multiplayer_session_reference
    lua_register(Data()->L, "MultiplayerSessionReferenceCreateCpp", MultiplayerSessionReferenceCreateCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionReferenceParseFromUriPathCpp", MultiplayerSessionReferenceParseFromUriPathCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionReferenceIsValidCpp", MultiplayerSessionReferenceIsValidCpp_Lua);

    //multiplayer_session
    lua_register(Data()->L, "MultiplayerSessionCreateCpp", MultiplayerSessionCreateCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionJoinCpp", MultiplayerSessionJoinCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionAddMemberReservationCpp", MultiplayerSessionAddMemberReservationCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionTimeOfSessionCpp", MultiplayerSessionTimeOfSessionCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionGetInitializationInfoCpp", MultiplayerSessionGetInitializationInfoCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSubscribedChangeTypesCpp", MultiplayerSessionSubscribedChangeTypesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionHostCandidatesCpp", MultiplayerSessionHostCandidatesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSessionReferenceCpp", MultiplayerSessionSessionReferenceCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSessionConstantsCpp", MultiplayerSessionSessionConstantsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetVisibilityCpp", MultiplayerSessionSetVisibilityCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetMaxMembersInSessionCpp", MultiplayerSessionSetMaxMembersInSessionCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetTimeoutsCpp", MultiplayerSessionSetTimeoutsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetQosConnectivityMetricsCpp", MultiplayerSessionSetQosConnectivityMetricsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetMemberInitializationCpp", MultiplayerSessionSetMemberInitializationCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetPeerToPeerRequirementsCpp", MultiplayerSessionSetPeerToPeerRequirementsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetPeerToHostRequirementsCpp", MultiplayerSessionSetPeerToHostRequirementsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetSessionCapabilitiesCpp", MultiplayerSessionSetSessionCapabilitiesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCloudComputePackageJsonCpp", MultiplayerSessionSetCloudComputePackageJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSessionPropertiesCpp", MultiplayerSessionSessionPropertiesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionPropertiesSetKeywordsCpp", MultiplayerSessionPropertiesSetKeywordsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionPropertiesSetJoinRestrictionCpp", MultiplayerSessionPropertiesSetJoinRestrictionCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionPropertiesSetReadRestrictionCpp", MultiplayerSessionPropertiesSetReadRestrictionCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionRoleTypesCpp", MultiplayerSessionRoleTypesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionMembersCpp", MultiplayerSessionMembersCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionGetMemberCpp", MultiplayerSessionGetMemberCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionMatchmakingServerCpp", MultiplayerSessionMatchmakingServerCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionMembersAcceptedCpp", MultiplayerSessionMembersAcceptedCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionServersJsonCpp", MultiplayerSessionServersJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetServersJsonCpp", MultiplayerSessionSetServersJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionEtagCpp", MultiplayerSessionEtagCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionCurrentUserCpp", MultiplayerSessionCurrentUserCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionGetInfoCpp", MultiplayerSessionGetInfoCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionWriteStatusCpp", MultiplayerSessionWriteStatusCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetInitializationStatusCpp", MultiplayerSessionSetInitializationStatusCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetHostDeviceTokenCpp", MultiplayerSessionSetHostDeviceTokenCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetMatchmakingServerConnectionPathCpp", MultiplayerSessionSetMatchmakingServerConnectionPathCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetClosedCpp", MultiplayerSessionSetClosedCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetLockedCpp", MultiplayerSessionSetLockedCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetAllocateCloudComputeCpp", MultiplayerSessionSetAllocateCloudComputeCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetMatchmakingResubmitCpp", MultiplayerSessionSetMatchmakingResubmitCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetServerConnectionStringCandidatesCpp", MultiplayerSessionSetServerConnectionStringCandidatesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetSessionChangeSubscriptionCpp", MultiplayerSessionSetSessionChangeSubscriptionCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionLeaveCpp", MultiplayerSessionLeaveCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCurrentUserStatusCpp", MultiplayerSessionSetCurrentUserStatusCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp", MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCurrentUserRolesCpp", MultiplayerSessionSetCurrentUserRolesCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionCurrentUserSetGroupsCpp", MultiplayerSessionCurrentUserSetGroupsCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionCurrentUserSetEncountersCpp", MultiplayerSessionCurrentUserSetEncountersCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCurrentUserQosMeasurementsJsonCpp", MultiplayerSessionSetCurrentUserQosMeasurementsJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp", MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp", MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp", MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionSetSessionCustomPropertyJsonCpp", MultiplayerSessionSetSessionCustomPropertyJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionDeleteSessionCustomPropertyJsonCpp", MultiplayerSessionDeleteSessionCustomPropertyJsonCpp_Lua);
    lua_register(Data()->L, "MultiplayerSessionCompareCpp", MultiplayerSessionCompareCpp_Lua);

    //multiplayer_service
    lua_register(Data()->L, "MultiplayerServiceWriteSession", MultiplayerServiceWriteSession_Lua);
    lua_register(Data()->L, "MultiplayerServiceWriteSessionByHandle", MultiplayerServiceWriteSessionByHandle_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetCurrentSession", MultiplayerServiceGetCurrentSession_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetCurrentSessionByHandle", MultiplayerServiceGetCurrentSessionByHandle_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetSessions", MultiplayerServiceGetSessions_Lua);
    lua_register(Data()->L, "MultiplayerServiceSetActivity", MultiplayerServiceSetActivity_Lua);
    lua_register(Data()->L, "MultiplayerServiceClearActivity", MultiplayerServiceClearActivity_Lua);
    lua_register(Data()->L, "MultiplayerServiceSendInvites", MultiplayerServiceSendInvites_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetActivitiesForSocialGroup", MultiplayerServiceGetActivitiesForSocialGroup_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetActivitiesForUsers", MultiplayerServiceGetActivitiesForUsers_Lua);
    lua_register(Data()->L, "MultiplayerServiceEnableMultiplayerSubscriptions", MultiplayerServiceEnableMultiplayerSubscriptions_Lua);
    lua_register(Data()->L, "MultiplayerServiceDisableMultiplayerSubscriptions", MultiplayerServiceDisableMultiplayerSubscriptions_Lua);
    lua_register(Data()->L, "MultiplayerServiceMultiplayerSubscriptionsEnabled", MultiplayerServiceMultiplayerSubscriptionsEnabled_Lua);
    lua_register(Data()->L, "MultiplayerServiceAddMultiplayerSessionChangedHandler", MultiplayerServiceAddMultiplayerSessionChangedHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceRemoveMultiplayerSessionChangedHandler", MultiplayerServiceRemoveMultiplayerSessionChangedHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceAddMultiplayerSubscriptionLostHandler", MultiplayerServiceAddMultiplayerSubscriptionLostHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler", MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceAddMultiplayerConnectionIdChangedHandler", MultiplayerServiceAddMultiplayerConnectionIdChangedHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler", MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler_Lua);
    lua_register(Data()->L, "MultiplayerServiceSetTransferHandle", MultiplayerServiceSetTransferHandle_Lua);
    lua_register(Data()->L, "MultiplayerServiceSetSearchHandle", MultiplayerServiceSetSearchHandle_Lua);
    lua_register(Data()->L, "MultiplayerServiceClearSearchHandle", MultiplayerServiceClearSearchHandle_Lua);
    lua_register(Data()->L, "MultiplayerServiceGetSearchHandles", MultiplayerServiceGetSearchHandles_Lua);

    //multiplayer_search_handle_details
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsCloseHandle", MultiplayerSearchHandleDetailsCloseHandle_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsSessionReference", MultiplayerSearchHandleDetailsSessionReference_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsHandleId", MultiplayerSearchHandleDetailsHandleId_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsSessionOwnerXuids", MultiplayerSearchHandleDetailsSessionOwnerXuids_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsTags", MultiplayerSearchHandleDetailsTags_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsStringsMetadata", MultiplayerSearchHandleDetailsStringsMetadata_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsNumbersMetadata", MultiplayerSearchHandleDetailsNumbersMetadata_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsVisibility", MultiplayerSearchHandleDetailsVisibility_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsJoinRestriction", MultiplayerSearchHandleDetailsJoinRestriction_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsClosed", MultiplayerSearchHandleDetailsClosed_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsMemberCounts", MultiplayerSearchHandleDetailsMemberCounts_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsHandleCreationTime", MultiplayerSearchHandleDetailsHandleCreationTime_Lua);
    lua_register(Data()->L, "MultiplayerSearchHandleDetailsCustomSessionPropertiesJson", MultiplayerSearchHandleDetailsCustomSessionPropertiesJson_Lua);

    //matchmaking_service
    lua_register(Data()->L, "MatchmakingServiceCreateTicket", MatchmakingServiceCreateTicket_Lua);
    lua_register(Data()->L, "MatchmakingServiceGetMatchTicketDetails", MatchmakingServiceGetMatchTicketDetails_Lua);
    lua_register(Data()->L, "MatchmakingServiceGetHopperStatistics", MatchmakingServiceGetHopperStatistics_Lua);
    lua_register(Data()->L, "MatchmakingServiceDeleteMatchTicket", MatchmakingServiceDeleteMatchTicket_Lua);
}

