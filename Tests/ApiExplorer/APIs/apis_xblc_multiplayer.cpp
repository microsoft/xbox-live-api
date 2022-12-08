// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

struct MultiplayerState
{
    std::vector<XblMultiplayerSessionHandle> sessionHandles;
    XblMultiplayerSessionReference sessionRef{};
    XblMultiplayerSearchHandle searchHandle{ nullptr };
    XblMultiplayerInviteHandle inviteHandle{};
    XblFunctionContext sessionChange{ 0 };
    XblFunctionContext lostHandler{ 0 };
    std::string activityHandle;

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
        static uint64_t runId{ utility::datetime::utc_now().to_interval() };
#endif

        std::stringstream ss;
        ss << "GameSession-" << runId << "-ID" << sessionId;
        return ss.str();
    }
};
std::unique_ptr<MultiplayerState> g_multiplayerState;


MultiplayerState* MPState()
{
    if (g_multiplayerState == nullptr)
    {
        g_multiplayerState = std::make_unique<MultiplayerState>();
        g_multiplayerState->sessionHandles.resize(10);
    }
    return g_multiplayerState.get();
}

XblMultiplayerSessionHandle GetSessionHandleFromArg(lua_State *L, int paramNum, uint64_t* sessionIndexOut = nullptr)
{
    auto sessionIndex{ GetUint64FromLua(L, paramNum, 0) };
    assert(MPState()->sessionHandles.size() > sessionIndex);
    if (sessionIndexOut != nullptr)
    {
        *sessionIndexOut = sessionIndex;
    }
    return MPState()->sessionHandles[static_cast<std::vector<XblMultiplayerSessionHandle>::size_type>(sessionIndex)];
}

int XblMultiplayerSessionCreateHandle_Lua(lua_State *L)
{
    uint64_t xuid = Data()->xboxUserId;

    std::string scid = GetStringFromLua(L, 1, Data()->scid);
    std::string sessionTemplateName = GetStringFromLua(L, 2, "MinGameSession");
    std::string sessionName = GetStringFromLua(L, 3, "");
    auto sessionIndex{ GetUint64FromLua(L, 4, 0) };

    if (sessionName.empty())
    {
        sessionName = MultiplayerState::GetSessionName(sessionIndex);
    }

    // CODE SNIPPET START: XblMultiplayerSessionCreateHandle
    XblMultiplayerSessionReference ref;
    pal::strcpy(ref.Scid, sizeof(ref.Scid), scid.c_str());
    pal::strcpy(ref.SessionTemplateName, sizeof(ref.SessionTemplateName), sessionTemplateName.c_str());
    pal::strcpy(ref.SessionName, sizeof(ref.SessionName), sessionName.c_str());

    XblMultiplayerSessionInitArgs args = {};

    XblMultiplayerSessionHandle sessionHandle = XblMultiplayerSessionCreateHandle(xuid, &ref, &args);
    // CODE SNIPPET END

    auto state{ MPState() };
    auto& session{ state->sessionHandles[static_cast<size_t>(sessionIndex)] };
    if (session)
    {
        XblMultiplayerSessionCloseHandle(session);
    }
    state->sessionHandles[static_cast<unsigned int>(sessionIndex)] = sessionHandle;

    lua_pushinteger(L, static_cast<lua_Integer>(sessionIndex));

    LogToFile("XblMultiplayerSessionCreateHandle");
    return LuaReturnHR(L, S_OK, 1);
}

int XblMultiplayerSessionJoin_Lua(lua_State *L)
{
    // Params:
    // 1) Session handle id returned when the session was created
    // 2) member custom constants json
    // 3) initialize requested
    // 4) join with active status

    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    std::string memberCustomConstantsJson = GetStringFromLua(L, 2, "{}");
    bool initializeRequested = GetBoolFromLua(L, 3, true);
    bool joinWithActiveStatus = GetBoolFromLua(L, 4, true);

    // CODE SNIPPET START: XblMultiplayerSessionJoin
    auto hr = XblMultiplayerSessionJoin(
        sessionHandle,
        memberCustomConstantsJson.c_str(),
        initializeRequested,
        joinWithActiveStatus);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionJoin: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

XblMultiplayerSessionWriteMode ConvertStringToXblMultiplayerSessionWriteMode(const char* str)
{
    XblMultiplayerSessionWriteMode writeMode = XblMultiplayerSessionWriteMode::UpdateOrCreateNew;

    if (pal::stricmp(str, "XblMultiplayerSessionWriteMode::SynchronizedUpdate") == 0) writeMode = XblMultiplayerSessionWriteMode::SynchronizedUpdate;
    else if (pal::stricmp(str, "XblMultiplayerSessionWriteMode::CreateNew") == 0) writeMode = XblMultiplayerSessionWriteMode::CreateNew;
    else if (pal::stricmp(str, "XblMultiplayerSessionWriteMode::UpdateExisting") == 0) writeMode = XblMultiplayerSessionWriteMode::UpdateExisting;

    return writeMode;
}

int XblMultiplayerWriteSessionAsync_Lua(lua_State *L)
{
    // Params:
    // 1) Session handle id returned when the session was created
    // 2) XblContextHandle to use. Defaults to Data()->xboxLiveContextse

    CreateQueueIfNeeded();
    auto sessionIndex{ GetUint64FromLua(L, 1, 0) };
    assert(MPState()->sessionHandles.size() > sessionIndex);

    XblContextHandle xboxLiveContext = (XblContextHandle)GetUint64FromLua(L, 2, (uint64_t)Data()->xboxLiveContext);

    ENSURE_IS_TRUE(MPState()->sessionHandles[static_cast<uint32_t>(sessionIndex)] != nullptr, "No valid multiplayer session.");

    XblMultiplayerSessionWriteMode writeMode = ConvertStringToXblMultiplayerSessionWriteMode(GetStringFromLua(L, 2, "XblMultiplayerSessionWriteMode::UpdateOrCreateNew").c_str());

    // CODE SNIPPET START: XblMultiplayerWriteSessionAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    auto contextPtr = std::make_unique<size_t>(static_cast<size_t>(sessionIndex));
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = contextPtr.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<size_t> sessionIndexPtr{ static_cast<size_t*>(asyncBlock->context) };
        auto sessionIndex{ *sessionIndexPtr };
        assert(MPState()->sessionHandles.size() > sessionIndex);
        auto& session{ MPState()->sessionHandles[sessionIndex] };

        if (session)
        {
            XblMultiplayerSessionCloseHandle(session);
            MPState()->sessionHandles[sessionIndex] = nullptr;
        }

        HRESULT hr = XblMultiplayerWriteSessionResult(asyncBlock, &session);
        if (SUCCEEDED(hr) && hr != 0x80070714) // HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
        {
            auto status = XblMultiplayerSessionWriteStatus(session);
            MPState()->sessionHandles[sessionIndex] = session;
            LogToFile("XblMultiplayerWriteSessionResult: hr=%s writeResult=%d", ConvertHR(hr).c_str(), status); // CODE SNIP SKIP
        }
        else
        {
            LogToFile("XblMultiplayerWriteSessionResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerWriteSessionAsync"); // CODE SNIP SKIP
    };

    auto hr = XblMultiplayerWriteSessionAsync(xboxLiveContext, MPState()->sessionHandles[static_cast<uint32_t>(sessionIndex)], writeMode, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        contextPtr.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerWriteSessionAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCloseHandle_Lua(lua_State *L)
{
    uint64_t sessionIndex = 0;
    auto sessionHandle = GetSessionHandleFromArg(L, 1, &sessionIndex);
    // CODE SNIPPET START: XblMultiplayerWriteSessionAsync
    XblMultiplayerSessionCloseHandle(sessionHandle);
    // CODE SNIPPET END
    MPState()->sessionHandles[static_cast<unsigned int>(sessionIndex)] = nullptr;

    LogToFile("XblMultiplayerSessionCloseHandle");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionAddMemberReservation_Lua(lua_State *L)
{
    uint64_t xuid = GetUint64FromLua(L, 1, 2814636782672891);
    bool initializeRequested = GetBoolFromLua(L, 2, true);
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    // CODE SNIPPET START: XblMultiplayerWriteSessionAsync
    auto hr = XblMultiplayerSessionAddMemberReservation(sessionHandle, xuid, nullptr, initializeRequested);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionAddMemberReservation: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void LogSessionRef(const XblMultiplayerSessionReference* sessionRef)
{
    LogToFile("Scid:%s", sessionRef->Scid);
    LogToFile("SessionName:%s", sessionRef->SessionName);
    LogToFile("SessionTemplateName:%s", sessionRef->SessionTemplateName);
}


int XblMultiplayerSessionReferenceCreate_Lua(lua_State *L)
{
    std::string scid = GetStringFromLua(L, 1, Data()->scid);
    std::string sessionTemplateName = GetStringFromLua(L, 2, "MinGameSession");
    std::string sessionName = GetStringFromLua(L, 3, MultiplayerState::GetSessionName());

    // CODE SNIPPET START: XblMultiplayerSessionReferenceCreate
    MPState()->sessionRef = XblMultiplayerSessionReferenceCreate(scid.c_str(), sessionTemplateName.c_str(), sessionName.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionReferenceCreate");
    LogSessionRef(&MPState()->sessionRef);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionReferenceParseFromUriPath_Lua(lua_State* L)
{
    std::string path = GetStringFromLua(L, 1, "");
    if (path.empty())
    {
        std::stringstream ss;
        ss << "/serviceconfigs/00000000-0000-0000-0000-000076029b4d/sessionTemplates/MinGameSession/sessions/";
        ss << MultiplayerState::GetSessionName();
        path = ss.str();
    }

    // CODE SNIPPET START: XblMultiplayerSessionReferenceParseFromUriPath
    HRESULT hr = XblMultiplayerSessionReferenceParseFromUriPath(path.c_str(), &MPState()->sessionRef);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionReferenceParseFromUriPath: hr=%s", ConvertHR(hr).c_str());
    LogToFile("Scid:%s", MPState()->sessionRef.Scid);
    LogToFile("SessionName:%s", MPState()->sessionRef.SessionName);
    LogToFile("SessionTemplateName:%s", MPState()->sessionRef.SessionTemplateName);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionReferenceIsValid_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSessionReferenceIsValid
    bool isValid = XblMultiplayerSessionReferenceIsValid(&MPState()->sessionRef);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionReferenceIsValid isValid:%d", isValid);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionDuplicateHandle_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionDuplicateHandle
    XblMultiplayerSessionHandle newHandle{};
    HRESULT hr = XblMultiplayerSessionDuplicateHandle(sessionHandle, &newHandle);
    // CODE SNIPPET END
    XblMultiplayerSessionCloseHandle(newHandle);

    LogToFile("XblMultiplayerSessionDuplicateHandle");
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionTimeOfSession_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionTimeOfSession
    time_t timeOfSession = XblMultiplayerSessionTimeOfSession(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionTimeOfSession timeOfSession:%d", timeOfSession);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionGetInitializationInfo_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionGetInitializationInfo
    const XblMultiplayerSessionInitializationInfo* info = XblMultiplayerSessionGetInitializationInfo(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionGetInitializationInfo");
    LogToFile("Stage: %d", info->Stage);
    LogToFile("StageStartTime: %ul", info->StageStartTime);
    LogToFile("Episode: %d", info->Episode);

    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSubscribedChangeTypes_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionSubscribedChangeTypes
    XblMultiplayerSessionChangeTypes changeTypes = XblMultiplayerSessionSubscribedChangeTypes(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSubscribedChangeTypes");
    LogToFile("changeTypes: 0x%0.4x", changeTypes);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionHostCandidates_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionHostCandidates
    const XblDeviceToken* deviceTokens = nullptr;
    size_t deviceTokensCount = 0;
    HRESULT hr = XblMultiplayerSessionHostCandidates(sessionHandle, &deviceTokens, &deviceTokensCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionHostCandidates: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSessionReference_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionSessionReference
    const XblMultiplayerSessionReference* sessionRef = XblMultiplayerSessionSessionReference(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSessionReference");
    LogToFile("Scid:%s", sessionRef->Scid);
    LogToFile("SessionName:%s", sessionRef->SessionName);
    LogToFile("SessionTemplateName:%s", sessionRef->SessionTemplateName);
    memcpy(&MPState()->sessionRef, sessionRef, sizeof(XblMultiplayerSessionReference));
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSessionConstants_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionSessionConstants
    const XblMultiplayerSessionConstants* consts = XblMultiplayerSessionSessionConstants(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSessionConstants");
    LogToFile("MaxMembersInSession: %d", consts->MaxMembersInSession);
    LogToFile("Visibility: %d", consts->Visibility);
    //uint64_t* InitiatorXuids;
    LogToFile("InitiatorXuidsCount: %ul", consts->InitiatorXuidsCount);
    if (consts->CustomJson) LogToFile("CustomJson: %s", consts->CustomJson);
    if (consts->SessionCloudComputePackageConstantsJson) LogToFile("SessionCloudComputePackageConstantsJson: %s", consts->SessionCloudComputePackageConstantsJson);
    LogToFile("MemberReservedTimeout: %ul", consts->MemberReservedTimeout);
    LogToFile("MemberInactiveTimeout: %ul", consts->MemberInactiveTimeout);
    LogToFile("MemberReadyTimeout: %ul", consts->MemberReadyTimeout);
    LogToFile("SessionEmptyTimeout: %ul", consts->SessionEmptyTimeout);
    LogToFile("EnableMetricsLatency: %d", consts->EnableMetricsLatency);
    LogToFile("EnableMetricsBandwidthDown: %d", consts->EnableMetricsBandwidthDown);
    LogToFile("EnableMetricsBandwidthUp: %d", consts->EnableMetricsBandwidthUp);
    LogToFile("EnableMetricsCustom: %d", consts->EnableMetricsCustom);
    //XblMultiplayerMemberInitialization* MemberInitialization;
    LogToFile("PeerToPeerRequirements->LatencyMaximum: %ul", consts->PeerToPeerRequirements.LatencyMaximum);
    LogToFile("PeerToPeerRequirements->BandwidthMinimumInKbps: %ul", consts->PeerToPeerRequirements.BandwidthMinimumInKbps);
    LogToFile("PeerToHostRequirements->LatencyMaximum: %ul", consts->PeerToHostRequirements.LatencyMaximum);
    LogToFile("PeerToHostRequirements->BandwidthMinimumInKbps: %ul", consts->PeerToHostRequirements.BandwidthDownMinimumInKbps);
    LogToFile("PeerToHostRequirements->BandwidthUpMinimumInKbps: %ul", consts->PeerToHostRequirements.BandwidthUpMinimumInKbps);
    LogToFile("PeerToHostRequirements->HostSelectionMetric: %ul", consts->PeerToHostRequirements.HostSelectionMetric);
    if (consts->MeasurementServerAddressesJson) LogToFile("MeasurementServerAddressesJson: %s", consts->MeasurementServerAddressesJson);
    LogToFile("ClientMatchmakingCapable: %d", consts->ClientMatchmakingCapable);
    LogToFile("EnableMetricsCustom: %d", consts->EnableMetricsCustom);
    LogToFile("SessionCapabilities->Connectivity: %d", consts->SessionCapabilities.Connectivity);
    LogToFile("SessionCapabilities->SuppressPresenceActivityCheck: %d", consts->SessionCapabilities.SuppressPresenceActivityCheck);
    LogToFile("SessionCapabilities->Gameplay: %d", consts->SessionCapabilities.Gameplay);
    LogToFile("SessionCapabilities->Large: %d", consts->SessionCapabilities.Large);
    LogToFile("SessionCapabilities->ConnectionRequiredForActiveMembers: %d", consts->SessionCapabilities.ConnectionRequiredForActiveMembers);
    LogToFile("SessionCapabilities->UserAuthorizationStyle: %d", consts->SessionCapabilities.UserAuthorizationStyle);
    LogToFile("SessionCapabilities->Crossplay: %d", consts->SessionCapabilities.Crossplay);
    LogToFile("SessionCapabilities->Searchable: %d", consts->SessionCapabilities.Searchable);
    LogToFile("SessionCapabilities->HasOwners: %d", consts->SessionCapabilities.HasOwners);

    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionConstantsSetMaxMembersInSession_Lua(lua_State *L)
{
    uint32_t maxMembersInSession = GetUint32FromLua(L, 1, 10);
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetMaxMembersInSession
    XblMultiplayerSessionConstantsSetMaxMembersInSession(sessionHandle, maxMembersInSession);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetMaxMembersInSession");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionConstantsSetVisibility_Lua(lua_State *L)
{
    XblMultiplayerSessionVisibility visibility = static_cast<XblMultiplayerSessionVisibility>(GetUint32FromLua(L, 1, 3));
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetVisibility
    XblMultiplayerSessionConstantsSetVisibility(sessionHandle, visibility);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetVisibility");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionConstantsSetTimeouts_Lua(lua_State *L)
{
    uint64_t memberReservedTimeout = GetUint64FromLua(L, 1, 100);
    uint64_t memberInactiveTimeout = GetUint64FromLua(L, 2, 100);
    uint64_t memberReadyTimeout = GetUint64FromLua(L, 3, 100);
    uint64_t sessionEmptyTimeout = GetUint64FromLua(L, 4, 100);
    auto sessionHandle = GetSessionHandleFromArg(L, 5);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetTimeouts
    HRESULT hr = XblMultiplayerSessionConstantsSetTimeouts(
        sessionHandle,
        memberReservedTimeout,
        memberInactiveTimeout,
        memberReadyTimeout,
        sessionEmptyTimeout);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetTimeouts: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetQosConnectivityMetrics_Lua(lua_State *L)
{
    bool enableLatencyMetric = GetBoolFromLua(L, 1, false);
    bool enableBandwidthDownMetric = GetBoolFromLua(L, 2, false);
    bool enableBandwidthUpMetric = GetBoolFromLua(L, 3, false);
    bool enableCustomMetric = GetBoolFromLua(L, 4, false);
    auto sessionHandle = GetSessionHandleFromArg(L, 5);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetQosConnectivityMetrics
    HRESULT hr = XblMultiplayerSessionConstantsSetQosConnectivityMetrics(
        sessionHandle,
        enableLatencyMetric,
        enableBandwidthDownMetric,
        enableBandwidthUpMetric,
        enableCustomMetric);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetQosConnectivityMetrics: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetMemberInitialization_Lua(lua_State *L)
{
    XblMultiplayerMemberInitialization init = {};
    init.JoinTimeout = GetUint64FromLua(L, 1, 100);
    init.MeasurementTimeout = GetUint64FromLua(L, 2, 100);
    init.EvaluationTimeout = GetUint64FromLua(L, 3, 100);
    init.ExternalEvaluation = GetBoolFromLua(L, 4, false);
    init.MembersNeededToStart = GetUint32FromLua(L, 5, 2);
    auto sessionHandle = GetSessionHandleFromArg(L, 6);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetMemberInitialization
    HRESULT hr = XblMultiplayerSessionConstantsSetMemberInitialization(sessionHandle, init);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetMemberInitialization: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetPeerToPeerRequirements_Lua(lua_State *L)
{
    XblMultiplayerPeerToPeerRequirements requirements = {};
    requirements.LatencyMaximum = GetUint64FromLua(L, 1, 100);
    requirements.BandwidthMinimumInKbps = GetUint64FromLua(L, 2, 100);
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetPeerToPeerRequirements
    HRESULT hr = XblMultiplayerSessionConstantsSetPeerToPeerRequirements(sessionHandle, requirements);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetPeerToPeerRequirements: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetPeerToHostRequirements_Lua(lua_State *L)
{
    XblMultiplayerPeerToHostRequirements requirements = {};
    requirements.LatencyMaximum = GetUint64FromLua(L, 1, 100);
    requirements.BandwidthDownMinimumInKbps = GetUint64FromLua(L, 2, 10);
    requirements.BandwidthUpMinimumInKbps = GetUint64FromLua(L, 3, 10);
    requirements.HostSelectionMetric = static_cast<XblMultiplayerMetrics>(GetUint64FromLua(L, 4, 1));
    auto sessionHandle = GetSessionHandleFromArg(L, 5);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetPeerToHostRequirements
    HRESULT hr = XblMultiplayerSessionConstantsSetPeerToHostRequirements(sessionHandle, requirements);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetPeerToHostRequirements: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson_Lua(lua_State *L)
{
    std::string measurementServerAddressesJson = GetStringFromLua(L, 1, "{}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson
    HRESULT hr = XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson(
        sessionHandle,
        measurementServerAddressesJson.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetCapabilities_Lua(lua_State *L)
{
    XblMultiplayerSessionCapabilities caps = {};
    caps.Connectivity = GetBoolFromLua(L, 1, false);
    caps.SuppressPresenceActivityCheck = GetBoolFromLua(L, 4, false);
    caps.Gameplay = GetBoolFromLua(L, 5, false);
    caps.Large = GetBoolFromLua(L, 6, true);
    caps.ConnectionRequiredForActiveMembers = GetBoolFromLua(L, 7, false);
    caps.UserAuthorizationStyle = GetBoolFromLua(L, 8, false);
    caps.Crossplay = GetBoolFromLua(L, 9, false);
    caps.Searchable = GetBoolFromLua(L, 10, false);
    caps.HasOwners = GetBoolFromLua(L, 11, false);
    auto sessionHandle = GetSessionHandleFromArg(L, 12);

    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetCapabilities
    HRESULT hr = XblMultiplayerSessionConstantsSetCapabilities(
        sessionHandle,
        caps);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetCapabilities: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionConstantsSetCloudComputePackageJson_Lua(lua_State *L)
{
    std::string sessionCloudComputePackageConstantsJson = GetStringFromLua(L, 1, "{}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionConstantsSetCloudComputePackageJson
    HRESULT hr = XblMultiplayerSessionConstantsSetCloudComputePackageJson(
        sessionHandle,
        sessionCloudComputePackageConstantsJson.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionConstantsSetCloudComputePackageJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSessionProperties_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionSessionProperties
    const XblMultiplayerSessionProperties* props = XblMultiplayerSessionSessionProperties(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSessionProperties");
    for (size_t i = 0; i < props->KeywordCount; i++)
    {
        LogToFile("Keywords[%ul]: %s", i, props->Keywords[i]);
    }
    LogToFile("KeywordCount: %ul", props->KeywordCount);
    LogToFile("JoinRestriction: %d", props->JoinRestriction);
    LogToFile("ReadRestriction: %d", props->ReadRestriction);
    for (size_t i = 0; i < props->TurnCollectionCount; i++)
    {
        LogToFile("TurnCollection[%d]: %ul", i, props->TurnCollection[i]);
    }
    LogToFile("TurnCollectionCount: %ul", props->TurnCollectionCount);
    LogToFile("MatchmakingTargetSessionConstantsJson: %s", props->MatchmakingTargetSessionConstantsJson);
    LogToFile("SessionCustomPropertiesJson: %s", props->SessionCustomPropertiesJson);
    LogToFile("MatchmakingServerConnectionString: %s", props->MatchmakingServerConnectionString);
    for (size_t i = 0; i < props->ServerConnectionStringCandidatesCount; i++)
    {
        LogToFile("ServerConnectionStringCandidates[%ul]: %s", i, props->ServerConnectionStringCandidates[i]);
    }
    LogToFile("ServerConnectionStringCandidatesCount: %ul", props->ServerConnectionStringCandidatesCount);
    for (size_t i = 0; i < props->SessionOwnerMemberIdsCount; i++)
    {
        LogToFile("SessionOwnerMemberIds[%ul]: %s", i, props->SessionOwnerMemberIds[i]);
    }
    LogToFile("SessionOwnerMemberIdsCount: %ul", props->SessionOwnerMemberIdsCount);
    LogToFile("HostDeviceToken: %s", props->HostDeviceToken.Value);
    LogToFile("Closed: %d", props->Closed);
    LogToFile("Locked: %d", props->Locked);
    LogToFile("AllocateCloudCompute: %d", props->AllocateCloudCompute);
    LogToFile("MatchmakingResubmit: %d", props->MatchmakingResubmit);

    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionPropertiesSetKeywords_Lua(lua_State *L)
{
    std::string key1 = GetStringFromLua(L, 1, "Keyword1");
    std::string key2 = GetStringFromLua(L, 2, "Keyword2");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);
    // CODE SNIPPET START: XblMultiplayerSessionPropertiesSetKeywords
    const char* keywords[2] = {};
    keywords[0] = key1.c_str();
    keywords[1] = key2.c_str();
    size_t keywordsCount = 2;

    HRESULT hr = XblMultiplayerSessionPropertiesSetKeywords(
        sessionHandle,
        keywords,
        keywordsCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionPropertiesSetKeywords: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionPropertiesSetJoinRestriction_Lua(lua_State *L)
{
    XblMultiplayerSessionRestriction joinRestriction = static_cast<XblMultiplayerSessionRestriction>(GetUint64FromLua(L, 1, static_cast<int>(XblMultiplayerSessionRestriction::Followed)));
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionPropertiesSetJoinRestriction
    XblMultiplayerSessionPropertiesSetJoinRestriction(
        sessionHandle,
        joinRestriction);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionPropertiesSetJoinRestriction");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionPropertiesSetReadRestriction_Lua(lua_State *L)
{
    XblMultiplayerSessionRestriction readRestriction = static_cast<XblMultiplayerSessionRestriction>(GetUint64FromLua(L, 1, static_cast<int>(XblMultiplayerSessionRestriction::Followed)));
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionPropertiesSetReadRestriction
    XblMultiplayerSessionPropertiesSetReadRestriction(
        sessionHandle,
        readRestriction
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionPropertiesSetReadRestriction");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionPropertiesSetTurnCollection_Lua(lua_State *L)
{
    uint32_t turnId1 = GetUint32FromLua(L, 1, 0);
    uint32_t turnId2 = GetUint32FromLua(L, 2, 1);
    auto sessionHandle = GetSessionHandleFromArg(L, 3);
    // CODE SNIPPET START: XblMultiplayerSessionPropertiesSetTurnCollection
    uint32_t turnCollectionMemberIds[2] = {};
    turnCollectionMemberIds[0] = turnId1;
    turnCollectionMemberIds[1] = turnId2;
    size_t turnCollectionMemberIdsCount = 2;
    HRESULT hr = XblMultiplayerSessionPropertiesSetTurnCollection(
        sessionHandle,
        turnCollectionMemberIds,
        turnCollectionMemberIdsCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionPropertiesSetTurnCollection: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionRoleTypes_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionRoleTypes
    const XblMultiplayerRoleType* roleTypes = nullptr;
    size_t roleTypesCount = 0;

    HRESULT hr = XblMultiplayerSessionRoleTypes(
        sessionHandle,
        &roleTypes,
        &roleTypesCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionRoleTypes: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionGetRoleByName_Lua(lua_State *L)
{
    std::string roleTypeName = GetStringFromLua(L, 1, "Role1");
    std::string roleName = GetStringFromLua(L, 2, "RoleName1");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);
    // CODE SNIPPET START: XblMultiplayerSessionGetRoleByName
    const XblMultiplayerRole* role = nullptr;
    HRESULT hr = XblMultiplayerSessionGetRoleByName(
        sessionHandle,
        roleTypeName.c_str(),
        roleName.c_str(),
        &role);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionGetRoleByName: hr=%s", ConvertHR(hr).c_str());
    if (role == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    //XblMultiplayerRoleType* RoleType;
    LogToFile("role.Name: %s", role->Name);
    for (uint32_t i = 0; i < role->MemberCount; i++)
    {
        LogToFile("role.MemberXuids[%ul]: %ul", i, role->MemberXuids[i]);
    }
    LogToFile("role.MemberCount: %d", role->MemberCount);
    LogToFile("role.TargetCount: %d", role->TargetCount);
    LogToFile("role.MaxMemberCount: %d", role->MaxMemberCount);

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSetMutableRoleSettings_Lua(lua_State *L)
{
    std::string roleTypeName = GetStringFromLua(L, 1, "Role1");
    std::string roleName = GetStringFromLua(L, 2, "RoleName1");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);
    // CODE SNIPPET START: XblMultiplayerSessionSetMutableRoleSettings
    uint32_t maxMemberCount = 0;
    uint32_t targetMemberCount = 0;

    HRESULT hr = XblMultiplayerSessionSetMutableRoleSettings(
        sessionHandle,
        roleTypeName.c_str(),
        roleName.c_str(),
        &maxMemberCount,
        &targetMemberCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetMutableRoleSettings: hr=%s", ConvertHR(hr).c_str());
    LogToFile("maxMemberCount: %d", maxMemberCount);
    LogToFile("targetMemberCount: %d", targetMemberCount);
    return LuaReturnHR(L, hr);
}

void LogSessionMember(const XblMultiplayerSessionMember* member)
{
    // TODO
    LogToFile("member->MemberId: %d", member->MemberId);
    LogToFile("member->InitialTeam: %s", member->InitialTeam);
    LogToFile("member->Xuid: %ul", member->Xuid);
    LogToFile("member->CustomConstantsJson: %s", member->CustomConstantsJson);
    LogToFile("member->SecureDeviceBaseAddress64: %s", member->SecureDeviceBaseAddress64);
    for (size_t i = 0; i < member->RolesCount; i++)
    {
        LogToFile("member->Roles[%ul].roleTypeName %s", i, member->Roles[i].roleTypeName);
        LogToFile("member->Roles[%ul].roleName %s", i, member->Roles[i].roleName);
    }
    LogToFile("member->RolesCount: %ul", member->RolesCount);
    LogToFile("member->CustomPropertiesJson: %s", member->CustomPropertiesJson);
    LogToFile("member->Gamertag: %s", member->Gamertag);
    LogToFile("member->XblMultiplayerSessionMemberStatus: %d", member->Status);
    LogToFile("member->IsTurnAvailable: %d", member->IsTurnAvailable);
    LogToFile("member->IsCurrentUser: %d", member->IsCurrentUser);
    LogToFile("member->InitializeRequested: %d", member->InitializeRequested);
    LogToFile("member->MatchmakingResultServerMeasurementsJson: %s", member->MatchmakingResultServerMeasurementsJson);
    LogToFile("member->ServerMeasurementsJson: %s", member->ServerMeasurementsJson);
    for (size_t i = 0; i < member->MembersInGroupCount; i++)
    {
        LogToFile("member->MembersInGroupIds[%d]: %ul", i, member->MembersInGroupIds[i]);
    }
    LogToFile("member->MembersInGroupCount: %ul", member->MembersInGroupCount);
    LogToFile("member->QosMeasurementsJson: %s", member->QosMeasurementsJson);
    LogToFile("member->DeviceToken: %s", member->DeviceToken.Value);
    LogToFile("member->Nat: %d", member->Nat);
    LogToFile("member->ActiveTitleId: %d", member->ActiveTitleId);
    LogToFile("member->InitializationEpisode: %d", member->InitializationEpisode);
    LogToFile("member->JoinTime: %ul", member->JoinTime);
    LogToFile("member->InitializationFailureCause: %d", member->InitializationFailureCause);
    for (size_t i = 0; i < member->GroupsCount; i++)
    {
        LogToFile("member->Groups[%d]: %s", i, member->Groups[i]);
    }
    LogToFile("member->GroupsCount: %ul", member->GroupsCount);
    for (size_t i = 0; i < member->EncountersCount; i++)
    {
        LogToFile("member->Encounters[%d]: %s", i, member->Encounters[i]);
    }
    LogToFile("member->EncountersCount: %ul", member->EncountersCount);
}

int XblMultiplayerSessionMembers_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionMembers
    const XblMultiplayerSessionMember* members = nullptr;
    size_t membersCount = 0;
    HRESULT hr = XblMultiplayerSessionMembers(
        sessionHandle,
        &members,
        &membersCount
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionMembers: hr=%s", ConvertHR(hr).c_str());
    for (size_t i = 0; i < membersCount; i++)
    {
        LogSessionMember(&members[i]);
    }

    LogToFile("membersCount: %d", membersCount);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionGetMember_Lua(lua_State *L)
{
    uint32_t memberId = GetUint32FromLua(L, 1, 0);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionGetMember
    const XblMultiplayerSessionMember* member = XblMultiplayerSessionGetMember(
        sessionHandle,
        memberId);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionGetMember");
    LogSessionMember(member);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionMatchmakingServer_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionMatchmakingServer
    const XblMultiplayerMatchmakingServer* server = XblMultiplayerSessionMatchmakingServer(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionMatchmakingServer");
    if (server == nullptr)
    {
        LogToFile("server == nullptr");
        return LuaReturnHR(L, S_OK);
    }

    LogToFile("server->Status: %d", server->Status);
    LogToFile("server->StatusDetails: %s", server->StatusDetails);
    LogToFile("server->TypicalWaitInSeconds: %d", server->TypicalWaitInSeconds);
    LogSessionRef(&server->TargetSessionRef);

    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionMembersAccepted_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionMembersAccepted
    uint32_t membersAccepted = XblMultiplayerSessionMembersAccepted(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionMembersAccepted");
    LogToFile("membersAccepted: %d", membersAccepted);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionRawServersJson_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionRawServersJson
    const char* json = XblMultiplayerSessionRawServersJson(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionRawServersJson");
    LogToFile("json: %s", json);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetRawServersJson_Lua(lua_State *L)
{
    std::string json = GetStringFromLua(L, 1, "{}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetRawServersJson
    HRESULT hr = XblMultiplayerSessionSetRawServersJson(sessionHandle, json.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetRawServersJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionEtag_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionEtag
    const char* etag = XblMultiplayerSessionEtag(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionEtag");
    LogToFile("etag: %s", etag);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionCurrentUser_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionCurrentUser
    const XblMultiplayerSessionMember* member = XblMultiplayerSessionCurrentUser(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUser");
    if (member == nullptr)
    {
        LogToFile("member == nullptr");
        return LuaReturnHR(L, S_OK);
    }

    LogSessionMember(member);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionGetInfo_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionGetInfo
    const XblMultiplayerSessionInfo * sessionInfo = XblMultiplayerSessionGetInfo(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionGetInfo");
    LogToFile("sessionInfo->ContractVersion: %d", sessionInfo->ContractVersion);
    LogToFile("sessionInfo->Branch: %s", sessionInfo->Branch);
    LogToFile("sessionInfo->ChangeNumber: %ul", sessionInfo->ChangeNumber);
    LogToFile("sessionInfo->CorrelationId: %s", sessionInfo->CorrelationId);
    LogToFile("sessionInfo->StartTime: %ul", sessionInfo->StartTime);
    LogToFile("sessionInfo->NextTimer: %ul", sessionInfo->NextTimer);
    LogToFile("sessionInfo->SearchHandleId: %s", sessionInfo->SearchHandleId);

    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionWriteStatus_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionWriteStatus
    XblWriteSessionStatus status = XblMultiplayerSessionWriteStatus(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionWriteStatus");
    LogToFile("status: %d", status);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetInitializationSucceeded_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    bool initSucceded = GetBoolFromLua(L, 1, false);
    // CODE SNIPPET START: XblMultiplayerSessionSetInitializationSucceeded
    XblMultiplayerSessionSetInitializationSucceeded(sessionHandle, initSucceded);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetInitializationSucceeded %d", initSucceded);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetHostDeviceToken_Lua(lua_State *L)
{
    std::string host = GetStringFromLua(L, 1, "DefaultHost");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetHostDeviceToken
    XblDeviceToken hostDeviceToken = {};
    pal::strcpy(hostDeviceToken.Value, sizeof(hostDeviceToken.Value), host.c_str());
    XblMultiplayerSessionSetHostDeviceToken(sessionHandle, hostDeviceToken);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetHostDeviceToken host:%s", host.c_str());
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetMatchmakingServerConnectionPath_Lua(lua_State *L)
{
    std::string path = GetStringFromLua(L, 1, "DefaultPath");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetMatchmakingServerConnectionPath
    XblMultiplayerSessionSetMatchmakingServerConnectionPath(
        sessionHandle, path.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetMatchmakingServerConnectionPath path:%s", path.c_str());
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetClosed_Lua(lua_State *L)
{
    bool closed = GetBoolFromLua(L, 1, true);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetClosed
    XblMultiplayerSessionSetClosed(sessionHandle, closed);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetClosed %d", closed);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetLocked_Lua(lua_State *L)
{
    bool locked = GetBoolFromLua(L, 1, false);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetLocked
    XblMultiplayerSessionSetLocked(sessionHandle, locked);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetLocked %d", locked);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetAllocateCloudCompute_Lua(lua_State *L)
{
    bool allocate = GetBoolFromLua(L, 1, false);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetAllocateCloudCompute
    XblMultiplayerSessionSetAllocateCloudCompute(sessionHandle, allocate);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetAllocateCloudCompute %d", allocate);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetMatchmakingResubmit_Lua(lua_State *L)
{
    bool matchResubmit = GetBoolFromLua(L, 1, false);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetMatchmakingResubmit
    XblMultiplayerSessionSetMatchmakingResubmit(sessionHandle, matchResubmit);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetMatchmakingResubmit %d", matchResubmit);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionSetServerConnectionStringCandidates_Lua(lua_State *L)
{
    std::string candidate1 = GetStringFromLua(L, 1, "Candidate1");
    std::string candidate2 = GetStringFromLua(L, 2, "Candidate1");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);
    HRESULT hr = S_OK;
    if (sessionHandle != nullptr) // might be null if previous call fails
    {
        // CODE SNIPPET START: XblMultiplayerSessionSetServerConnectionStringCandidates
        const char* serverConnectionStringCandidates[2] = {};
        serverConnectionStringCandidates[0] = candidate1.c_str();
        serverConnectionStringCandidates[1] = candidate2.c_str();
        size_t serverConnectionStringCandidatesCount = 2;

        hr = XblMultiplayerSessionSetServerConnectionStringCandidates(
            sessionHandle,
            serverConnectionStringCandidates,
            serverConnectionStringCandidatesCount);
        // CODE SNIPPET END
    }

    LogToFile("XblMultiplayerSessionSetServerConnectionStringCandidates: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSetSessionChangeSubscription_Lua(lua_State *L)
{
    XblMultiplayerSessionChangeTypes changeTypes = static_cast<XblMultiplayerSessionChangeTypes>(GetUint32FromLua(L, 1, static_cast<int>(XblMultiplayerSessionChangeTypes::Everything)));
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionSetSessionChangeSubscription
    HRESULT hr = XblMultiplayerSessionSetSessionChangeSubscription(
        sessionHandle,
        changeTypes);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetSessionChangeSubscription: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionLeave_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    // CODE SNIPPET START: XblMultiplayerSessionLeave
    HRESULT hr = XblMultiplayerSessionLeave(sessionHandle);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionLeave: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetStatus_Lua(lua_State *L)
{
    XblMultiplayerSessionMemberStatus status = static_cast<XblMultiplayerSessionMemberStatus>(GetUint32FromLua(L, 1, 3));
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetStatus
    HRESULT hr = XblMultiplayerSessionCurrentUserSetStatus(
        sessionHandle,
        status);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetStatus: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64_Lua(lua_State *L)
{
    std::string deviceAddress = GetStringFromLua(L, 1, "ExampleDeviceAddress");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64
    HRESULT hr = XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64(
        sessionHandle,
        deviceAddress.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblFormatSecureDeviceAddress_Lua(lua_State *L)
{
#if HC_PLATFORM != HC_PLATFORM_XDK && HC_PLATFORM != HC_PLATFORM_UWP
    std::string deviceIdStr = GetStringFromLua(L, 1, "ExampleDeviceAddress");
    auto deviceId = deviceIdStr.c_str();

    // CODE SNIPPET START: XblFormatSecureDeviceAddress
    XblFormattedSecureDeviceAddress address{ };
    HRESULT hr = XblFormatSecureDeviceAddress(deviceId, &address);
    // CODE SNIPPET END

    LogToFile("XblFormatSecureDeviceAddress: hr=%s sda=%s", hr, address.value);
#endif
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSessionCurrentUserSetRoles_Lua(lua_State *L)
{
    std::string roleTypeName1 = "roleTypeName1";
    std::string roleName1 = "roleName1";
    std::string roleTypeName2 = "roleTypeName2";
    std::string roleName2 = "roleName2";
    auto sessionHandle = GetSessionHandleFromArg(L, 1);

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetRoles
    XblMultiplayerSessionMemberRole roles[2] = {};
    roles[0].roleTypeName = roleTypeName1.c_str();
    roles[0].roleName = roleName1.c_str();
    roles[1].roleTypeName = roleTypeName2.c_str();
    roles[1].roleName = roleName2.c_str();
    size_t rolesCount = 2;
    HRESULT hr = XblMultiplayerSessionCurrentUserSetRoles(
        sessionHandle,
        roles,
        rolesCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetRoles: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetMembersInGroup_Lua(lua_State *L)
{
    uint32_t memberId1 = GetUint32FromLua(L, 1, 0);
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetMembersInGroup
    uint32_t memberIds[1] = {};
    memberIds[0] = memberId1;
    size_t memberIdsCount = 1;
    HRESULT hr = XblMultiplayerSessionCurrentUserSetMembersInGroup(
        sessionHandle,
        memberIds,
        memberIdsCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetMembersInGroup: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetGroups_Lua(lua_State *L)
{
    std::string group1 = GetStringFromLua(L, 1, "group1");
    std::string group2 = GetStringFromLua(L, 2, "group2");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetGroups
    const char* groups[2] = {};
    groups[0] = group1.c_str();
    groups[1] = group2.c_str();
    size_t groupsCount = 2;

    HRESULT hr = XblMultiplayerSessionCurrentUserSetGroups(
        sessionHandle,
        groups,
        groupsCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetGroups: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetEncounters_Lua(lua_State *L)
{
    std::string encounter1 = GetStringFromLua(L, 1, "encounter1");
    std::string encounter2 = GetStringFromLua(L, 2, "encounter2");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetEncounters
    const char* encounters[2] = {};
    encounters[0] = encounter1.c_str();
    encounters[1] = encounter2.c_str();
    size_t encountersCount = 2;

    HRESULT hr = XblMultiplayerSessionCurrentUserSetEncounters(
        sessionHandle,
        encounters,
        encountersCount);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetEncounters: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetQosMeasurements_Lua(lua_State *L)
{
    std::string measurements = GetStringFromLua(L, 1, "{\"measurements1\":5}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetQosMeasurements
    HRESULT hr = XblMultiplayerSessionCurrentUserSetQosMeasurements(
        sessionHandle,
        measurements.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetQosMeasurements: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetServerQosMeasurements_Lua(lua_State *L)
{
    std::string measurements = GetStringFromLua(L, 1, "{\"measurements1\":5}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);
    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetServerQosMeasurements
    HRESULT hr = XblMultiplayerSessionCurrentUserSetServerQosMeasurements(
        sessionHandle,
        measurements.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetServerQosMeasurements: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserSetCustomPropertyJson_Lua(lua_State *L)
{
    std::string name = GetStringFromLua(L, 1, "name1");
    std::string json = GetStringFromLua(L, 2, "{\"myscore\":123}");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    ENSURE_IS_TRUE(sessionHandle != nullptr, "No valid multiplayer session.");

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserSetCustomPropertyJson
    HRESULT hr = XblMultiplayerSessionCurrentUserSetCustomPropertyJson(
        sessionHandle,
        name.c_str(),
        json.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserSetCustomPropertyJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson_Lua(lua_State *L)
{
    std::string name = GetStringFromLua(L, 1, "name1");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    // CODE SNIPPET START: XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson
    HRESULT hr = XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(
        sessionHandle,
        name.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson_Lua(lua_State *L)
{
    std::string consts = GetStringFromLua(L, 1, "{}");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    HRESULT hr = S_OK;
    if (sessionHandle != nullptr) // might be null if previous call fails
    {
        // CODE SNIPPET START: XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson
        hr = XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson(
            sessionHandle,
            consts.c_str());
        // CODE SNIPPET END
    }

    LogToFile("XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionSetCustomPropertyJson_Lua(lua_State *L)
{
    std::string name = GetStringFromLua(L, 1, "name1");
    std::string json = GetStringFromLua(L, 2, "{}");
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    // CODE SNIPPET START: XblMultiplayerSessionSetCustomPropertyJson
    HRESULT hr = XblMultiplayerSessionSetCustomPropertyJson(
        sessionHandle,
        name.c_str(),
        json.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionSetCustomPropertyJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionDeleteCustomPropertyJson_Lua(lua_State *L)
{
    std::string name = GetStringFromLua(L, 1, "name1");
    auto sessionHandle = GetSessionHandleFromArg(L, 2);

    // CODE SNIPPET START: XblMultiplayerSessionDeleteCustomPropertyJson
    HRESULT hr = XblMultiplayerSessionDeleteCustomPropertyJson(
        sessionHandle,
        name.c_str());
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSessionDeleteCustomPropertyJson: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSessionCompare_Lua(lua_State *L)
{
    auto sessionHandle = GetSessionHandleFromArg(L, 1);
    XblMultiplayerSessionHandle sessionHandle2{};
    XblMultiplayerSessionDuplicateHandle(sessionHandle, &sessionHandle2);

    // CODE SNIPPET START: XblMultiplayerSessionCompare
    XblMultiplayerSessionChangeTypes changeTypes = XblMultiplayerSessionCompare(
        sessionHandle,
        sessionHandle2);
    // CODE SNIPPET END

    XblMultiplayerSessionCloseHandle(sessionHandle2);
    LogToFile("XblMultiplayerSessionCompare");
    LogToFile("changeTypes: %d", changeTypes);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerWriteSessionByHandleAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    XblMultiplayerSessionWriteMode writeMode = static_cast<XblMultiplayerSessionWriteMode>(GetUint32FromLua(L, 1, 
        static_cast<int>(XblMultiplayerSessionWriteMode::UpdateExisting)));
    std::string handleId = GetStringFromLua(L, 2, MPState()->activityHandle);
    auto sessionHandle = GetSessionHandleFromArg(L, 3);

    // CODE SNIPPET START: XblMultiplayerWriteSessionByHandleAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblMultiplayerSessionHandle sessionHandle;
        auto hr = XblMultiplayerWriteSessionByHandleResult(asyncBlock, &sessionHandle);
        LogToFile("XblMultiplayerWriteSessionByHandleResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerWriteSessionByHandleAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerWriteSessionByHandleAsync(
        Data()->xboxLiveContext,
        sessionHandle,
        writeMode,
        handleId.c_str(),
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblMultiplayerWriteSessionByHandleAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("writeMode: %d", writeMode);
    LogToFile("handleId: %s", handleId.c_str());

    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetSessionAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    XblMultiplayerSessionReference sessionRef;
    std::string scid = GetStringFromLua(L, 1, Data()->scid);
    std::string sessionTemplateName = GetStringFromLua(L, 2, "MinGameSession");
    auto sessionIndex{ GetUint64FromLua(L, 4, 0) };
    std::string sessionName = GetStringFromLua(L, 3, MultiplayerState::GetSessionName(sessionIndex));

    pal::strcpy(sessionRef.Scid, sizeof(sessionRef.Scid), scid.c_str());
    pal::strcpy(sessionRef.SessionName, sizeof(sessionRef.SessionName), sessionName.c_str());
    pal::strcpy(sessionRef.SessionTemplateName, sizeof(sessionRef.SessionTemplateName), sessionTemplateName.c_str());

    // CODE SNIPPET START: XblMultiplayerWriteSessionByHandleAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    auto contextPtr = std::make_unique<size_t>(static_cast<size_t>(sessionIndex));
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = contextPtr.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<size_t> sessionIndexPtr{ static_cast<size_t*>(asyncBlock->context) };
        auto sessionIndex{ *sessionIndexPtr };

        auto& session{ MPState()->sessionHandles[sessionIndex] }; //CODE SNIP SKIP
        if (session) //CODE SNIP SKIP
        {
            XblMultiplayerSessionCloseHandle(session); //CODE SNIP SKIP
            MPState()->sessionHandles[sessionIndex] = nullptr; //CODE SNIP SKIP
        }

        XblMultiplayerSessionHandle sessionHandle = nullptr;
        auto hr = XblMultiplayerGetSessionResult(asyncBlock, &sessionHandle);
        LogToFile("XblMultiplayerGetSessionResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        MPState()->sessionHandles[sessionIndex] = sessionHandle; // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetSessionAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerGetSessionAsync(
        Data()->xboxLiveContext,
        &sessionRef,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        contextPtr.release();
    }
    // CODE SNIPPET END
    LogToFile("XblMultiplayerGetSessionAsync: hr=%s", ConvertHR(hr).c_str());
    LogSessionRef(&sessionRef);

    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetSessionByHandleAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string handleId = GetStringFromLua(L, 1, MPState()->inviteHandle.Data);
    auto sessionIndex{ GetUint64FromLua(L, 2, 0) };

    if (handleId.empty())
    {
        handleId = "86191619-4002-044f-4846-f8f903c71512";
    }

    // CODE SNIPPET START: XblMultiplayerGetSessionByHandleAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    auto contextPtr = std::make_unique<size_t>(static_cast<size_t>(sessionIndex));
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = contextPtr.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<size_t> sessionIndexPtr{ static_cast<size_t*>(asyncBlock->context) };
        auto sessionIndex{ *sessionIndexPtr };

        auto& session{ MPState()->sessionHandles[sessionIndex] }; //CODE SNIP SKIP
        if (session) //CODE SNIP SKIP
        {
            XblMultiplayerSessionCloseHandle(session); //CODE SNIP SKIP
            MPState()->sessionHandles[sessionIndex] = nullptr; //CODE SNIP SKIP
        }

        XblMultiplayerSessionHandle sessionHandle = nullptr;
        auto hr = XblMultiplayerGetSessionByHandleResult(asyncBlock, &sessionHandle);
        LogToFile("XblMultiplayerGetSessionByHandleResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        MPState()->sessionHandles[sessionIndex] = sessionHandle; // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetSessionByHandleAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerGetSessionByHandleAsync(
        Data()->xboxLiveContext,
        handleId.c_str(),
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        contextPtr.release();
    }
    // CODE SNIPPET END
    LogToFile("XblMultiplayerGetSessionAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("handleId: %s", handleId.c_str());

    return LuaReturnHR(L, hr);
}

int XblMultiplayerQuerySessionsAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string Scid = GetStringFromLua(L, 1, Data()->scid);
    uint32_t MaxItems = GetUint32FromLua(L, 2, 0);
    bool IncludePrivateSessions = GetBoolFromLua(L, 3, false);
    bool IncludeReservations = GetBoolFromLua(L, 4, false);
    bool IncludeInactiveSessions = GetBoolFromLua(L, 5, false);
    std::string KeywordFilter = GetStringFromLua(L, 6, "killzone");
    std::string SessionTemplateNameFilter = GetStringFromLua(L, 7, "");
    XblMultiplayerSessionVisibility VisibilityFilter = static_cast<XblMultiplayerSessionVisibility>(GetUint32FromLua(L, 8, 5));
    uint32_t ContractVersionFilter = GetUint32FromLua(L, 9, 0);
    //uint64_t* XuidFilters;
    //size_t XuidFiltersCount;

    // CODE SNIPPET START: XblMultiplayerQuerySessionsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t sessionCount = 0;
        HRESULT hr = XblMultiplayerQuerySessionsResultCount(asyncBlock, &sessionCount);
        LogToFile("sessionCount %d", sessionCount);
        if (SUCCEEDED(hr))
        {
            std::vector<XblMultiplayerSessionQueryResult> sessions(sessionCount);
            hr = XblMultiplayerQuerySessionsResult(asyncBlock, sessionCount, sessions.data());
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerQuerySessionsAsync"); // CODE SNIP SKIP
    };

    XblMultiplayerSessionQuery sessionQuery = {};
    pal::strcpy(sessionQuery.Scid, sizeof(sessionQuery.Scid), Scid.c_str());
    sessionQuery.MaxItems = MaxItems;
    sessionQuery.IncludePrivateSessions = IncludePrivateSessions;
    sessionQuery.IncludeReservations = IncludeReservations;
    sessionQuery.IncludeInactiveSessions = IncludeInactiveSessions;
    sessionQuery.KeywordFilter = KeywordFilter.c_str();
    pal::strcpy(sessionQuery.SessionTemplateNameFilter, sizeof(sessionQuery.SessionTemplateNameFilter), SessionTemplateNameFilter.c_str());
    sessionQuery.VisibilityFilter = VisibilityFilter;
    sessionQuery.ContractVersionFilter = ContractVersionFilter;
    //sessionQuery.XuidFilters
    //sessionQuery.XuidFiltersCount 

    HRESULT hr = XblMultiplayerQuerySessionsAsync(
        Data()->xboxLiveContext,
        &sessionQuery,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblMultiplayerQuerySessionsAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSetActivityAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSetActivityAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerSetActivityAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerSetActivityAsync(
        Data()->xboxLiveContext,
        &MPState()->sessionRef,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSetActivityAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerClearActivityAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerClearActivityAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerClearActivityAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerClearActivityAsync(
        Data()->xboxLiveContext,
        Data()->scid, // TODO: fix type
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerClearActivityAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSendInvitesAsync_Lua(lua_State *L)
{
    std::string contextStringIdStr = GetStringFromLua(L, 1, "contextStringId1");
    std::string customActivationContextStr = GetStringFromLua(L, 2, "customActivationContext1");
    uint64_t targetXuid = GetUint64FromLua(L, 3, 2814679169942680);

    auto xblContextHandle = Data()->xboxLiveContext;
    auto sessionReference = MPState()->sessionRef;
    auto titleId = Data()->titleId;
    auto contextStringId = contextStringIdStr.c_str();
    auto customActivationContext = customActivationContextStr.c_str();

    // CODE SNIPPET START: XblMultiplayerSendInvitesAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t handlesCount = 1; // must be equal to invites requested
        XblMultiplayerInviteHandle handles[1] = {};
        HRESULT hr = XblMultiplayerSendInvitesResult(asyncBlock, handlesCount, handles);
        MPState()->inviteHandle = handles[0]; // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerSendInvitesAsync"); // CODE SNIP SKIP
    };

    uint64_t xuids[1] = {};
    xuids[0] = targetXuid;
    size_t xuidsCount = 1;

    HRESULT hr = XblMultiplayerSendInvitesAsync(
        xblContextHandle,
        &sessionReference,
        xuids,
        xuidsCount,
        titleId,
        contextStringId,
        customActivationContext,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSendInvitesAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetActivitiesForSocialGroupAsync_Lua(lua_State *L)
{
    uint64_t socialGroupOwnerXuid = GetUint64FromLua(L, 1, 2814632956486799);
    std::string socialGroup = GetStringFromLua(L, 2, "people");

    // CODE SNIPPET START: XblMultiplayerGetActivitiesForSocialGroupAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultCount{ 0 };
        HRESULT hr = XblMultiplayerGetActivitiesForSocialGroupResultCount(asyncBlock, &resultCount);
        LogToFile("activityCount %d", resultCount); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            std::vector<XblMultiplayerActivityDetails> activityDetails(resultCount);
            hr = XblMultiplayerGetActivitiesForSocialGroupResult(asyncBlock, resultCount, activityDetails.data());
        }
        else if (hr == HTTP_E_STATUS_SERVICE_UNAVAIL)
        {
            CallLuaFunctionWithHr(S_OK, "OnXblMultiplayerGetActivitiesForSocialGroupAsyncRetry");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetActivitiesForSocialGroupAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerGetActivitiesForSocialGroupAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        socialGroupOwnerXuid,
        socialGroup.c_str(),
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerGetActivitiesForSocialGroupAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync_Lua(lua_State *L)
{
    uint64_t socialGroupOwnerXuid = GetUint64FromLua(L, 1, 2814632956486799);
    std::string socialGroup = GetStringFromLua(L, 2, "people");

    // CODE SNIPPET START: XblMultiplayerGetActivitiesForSocialGroupAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize{ 0 };
        HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(asyncBlock, &resultSize);
        LogToFile("activityCount %d", resultSize); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            if (resultSize > 0)
            {
                size_t count{ 0 };
                std::vector<char> buffer(resultSize, 0);
                XblMultiplayerActivityDetails* activityDetails{};
                hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(asyncBlock, resultSize, buffer.data(), &activityDetails, &count, nullptr);
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        socialGroupOwnerXuid,
        socialGroup.c_str(),
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetActivitiesForUsersAsync_Lua(lua_State *L)
{
    uint64_t xuid1 = GetUint64FromLua(L, 1, Data()->m_multiDeviceManager->GetRemoteXuid());
    if (xuid1 == 0) xuid1 = 2814636782672891;

    // CODE SNIPPET START: XblMultiplayerGetActivitiesForUsersAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (SUCCEEDED(hr))
        {
            size_t resultCount{ 0 };
            hr = XblMultiplayerGetActivitiesForUsersResultCount(asyncBlock, &resultCount);
            if (SUCCEEDED(hr))
            {
                std::vector<XblMultiplayerActivityDetails> activityDetails(resultCount);
                hr = XblMultiplayerGetActivitiesForUsersResult(asyncBlock, resultCount, activityDetails.data());
                if (SUCCEEDED(hr))
                {
                    if (resultCount > 0)
                    {
                        std::string handleIdStr = activityDetails[0].HandleId;
                        LogToScreen("Joining lobby via handle %s", handleIdStr.c_str());
                        MPState()->activityHandle = handleIdStr; // CODE SNIP SKIP
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
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetActivitiesForUsersAsync"); // CODE SNIP SKIP
    };

    uint64_t xuids[1] = {};
    xuids[0] = xuid1;
    size_t xuidsCount = 1;

    HRESULT hr = XblMultiplayerGetActivitiesForUsersAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        xuids,
        xuidsCount,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerGetActivitiesForUsersAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerGetActivitiesWithPropertiesForUsersAsync_Lua(lua_State *L)
{
    uint64_t xuid1 = GetUint64FromLua(L, 1, Data()->m_multiDeviceManager->GetRemoteXuid());
    if (xuid1 == 0) xuid1 = 2814636782672891;

    // CODE SNIPPET START: XblMultiplayerGetActivitiesForUsersAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (SUCCEEDED(hr))
        {
            size_t resultSize{ 0 };
            hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(asyncBlock, &resultSize);
            if (SUCCEEDED(hr))
            {
                size_t count{ 0 };
                std::vector<char> buffer(resultSize);
                XblMultiplayerActivityDetails* activityDetails{};
                if (resultSize > 0)
                {
                    hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResult(asyncBlock, resultSize, buffer.data(), &activityDetails, &count, nullptr);
                    if (SUCCEEDED(hr))
                    {
                            std::string handleIdStr = activityDetails[0].HandleId;
                            LogToScreen("Joining lobby via handle %s", handleIdStr.c_str());
                            MPState()->activityHandle = handleIdStr; // CODE SNIP SKIP
                    }
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
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetActivitiesWithPropertiesForUsersAsync"); // CODE SNIP SKIP
    };

    uint64_t xuids[1] = {};
    xuids[0] = xuid1;
    size_t xuidsCount = 1;

    HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        xuids,
        xuidsCount,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerGetActivitiesWithPropertiesForUsersAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSetSubscriptionsEnabled_Lua(lua_State *L)
{

    bool enabled = GetBoolFromLua(L, 1, true);
    XblContextHandle xboxLiveContext = (XblContextHandle)GetUint64FromLua(L, 2, (uint64_t)Data()->xboxLiveContext);

    // CODE SNIPPET START: XblMultiplayerSetSubscriptionsEnabled
    HRESULT hr = XblMultiplayerSetSubscriptionsEnabled(xboxLiveContext, enabled);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSetSubscriptionsEnabled: hr=%s", ConvertHR(hr).c_str());
    LogToFile("enabled: %d", enabled);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSubscriptionsEnabled_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSubscriptionsEnabled
    bool enabled = XblMultiplayerSubscriptionsEnabled(Data()->xboxLiveContext);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSubscriptionsEnabled");
    LogToFile("enabled: %d", enabled);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerAddSessionChangedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerAddSessionChangedHandler
    void* context = nullptr;
    XblFunctionContext fnContext = XblMultiplayerAddSessionChangedHandler(
        Data()->xboxLiveContext,
        [](_In_opt_ void*, _In_ XblMultiplayerSessionChangeEventArgs args)
    {
        LogToFile("XblMultiplayerAddSessionChangedHandler");
        LogToFile("ChangeNumber: %d", args.ChangeNumber);
        CallLuaFunctionWithHr(S_OK, "OnXblMultiplayerAddSessionChangedHandler"); // CODE SNIP SKIP
    },
        context);
    // CODE SNIPPET END
    MPState()->sessionChange = fnContext;

    LogToFile("XblMultiplayerAddSessionChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerRemoveSessionChangedHandler_Lua(lua_State *L)
{
    XblFunctionContext fnContext = MPState()->sessionChange;
    // CODE SNIPPET START: XblMultiplayerRemoveSessionChangedHandler
    XblMultiplayerRemoveSessionChangedHandler(Data()->xboxLiveContext, fnContext);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerRemoveSessionChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerAddSubscriptionLostHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerAddSubscriptionLostHandler
    void* context = nullptr;
    XblFunctionContext fnContext = XblMultiplayerAddSubscriptionLostHandler(
        Data()->xboxLiveContext,
        [](_In_opt_ void*)
    {
        LogToFile("XblMultiplayerAddSubscriptionLostHandler");
        CallLuaFunctionWithHr(S_OK, "OnXblMultiplayerAddSubscriptionLostHandler"); // CODE SNIP SKIP
    },
        context);
    // CODE SNIPPET END
    MPState()->lostHandler = fnContext;

    LogToFile("XblMultiplayerAddSubscriptionLostHandler");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerRemoveSubscriptionLostHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerRemoveSubscriptionLostHandler
    XblMultiplayerRemoveSubscriptionLostHandler(
        Data()->xboxLiveContext,
        MPState()->lostHandler);
    // CODE SNIPPET END

    LogToFile("XblMultiplayerRemoveSubscriptionLostHandler");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSearchHandleDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleDuplicateHandle
    XblMultiplayerSearchHandle duplicate{ nullptr };

    HRESULT hr = XblMultiplayerSearchHandleDuplicateHandle(
        MPState()->searchHandle,
        &duplicate
    );

    if (SUCCEEDED(hr))
    {
        XblMultiplayerSearchHandleCloseHandle(duplicate);
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleDuplicateHandle: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleCloseHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleCloseHandle
    XblMultiplayerSearchHandleCloseHandle(MPState()->searchHandle);
    MPState()->searchHandle = nullptr;
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleCloseHandle");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerSearchHandleGetSessionReference_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetSessionReference
    XblMultiplayerSessionReference sessionRef{};

    HRESULT hr = XblMultiplayerSearchHandleGetSessionReference(
        MPState()->searchHandle,
        &sessionRef
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetSessionReference: hr=%s", ConvertHR(hr).c_str());
    LogSessionRef(&sessionRef);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetId_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetId
    const char* handleId{ nullptr };

    HRESULT hr = XblMultiplayerSearchHandleGetId(
        MPState()->searchHandle,
        &handleId
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetId: hr=%s", ConvertHR(hr).data());
    LogToFile("Search Handle Id: %s", handleId);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetSessionOwnerXuids_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetSessionOwnerXuids
    const uint64_t* xuids{ nullptr };
    size_t xuidsCount{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetSessionOwnerXuids(
        MPState()->searchHandle,
        &xuids,
        &xuidsCount
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetSessionOwnerXuids: hr=%s", ConvertHR(hr).data());
    LogToFile("There are %u session owners:", xuidsCount);
    for (auto i = 0u; i < xuidsCount; ++i)
    {
        LogToFile("\t%u", xuids[i]);
    }

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetTags_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetTags
    const XblMultiplayerSessionTag* tags{ nullptr };
    size_t tagsCount{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetTags(
        MPState()->searchHandle,
        &tags,
        &tagsCount
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetTags: hr=%s", ConvertHR(hr).data());
    LogToFile("There are %u tags for the session:", tagsCount);
    for (auto i = 0u; i < tagsCount; ++i)
    {
        LogToFile("\t%s", tags[i].value);
    }

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetStringAttributes_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetStringAttributes
    const XblMultiplayerSessionStringAttribute* attributes{ nullptr };
    size_t attributesCount{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetStringAttributes(
        MPState()->searchHandle,
        &attributes,
        &attributesCount
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetStringAttributes: hr=%s", ConvertHR(hr).data());
    LogToFile("There are %u string attributes for the session:", attributesCount);
    for (auto i = 0u; i < attributesCount; ++i)
    {
        LogToFile("\t%s : %s", attributes[i].name, attributes[i].value);
    }

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetNumberAttributes_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetNumberAttributes
    const XblMultiplayerSessionNumberAttribute* attributes{ nullptr };
    size_t attributesCount{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetNumberAttributes(
        MPState()->searchHandle,
        &attributes,
        &attributesCount
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetNumberAttributes: hr=%s", ConvertHR(hr).data());
    LogToFile("There are %u number attributes for the session:", attributesCount);
    for (auto i = 0u; i < attributesCount; ++i)
    {
        LogToFile("\t%s : %f", attributes[i].name, attributes[i].value);
    }

    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetVisibility_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetVisibility
    XblMultiplayerSessionVisibility visibility{ XblMultiplayerSessionVisibility::Unknown };

    HRESULT hr = XblMultiplayerSearchHandleGetVisibility(
        MPState()->searchHandle,
        &visibility
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetVisibility: visibility=%u, hr=%s", visibility, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetJoinRestriction_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetJoinRestriction
    XblMultiplayerSessionRestriction joinRestriction{ XblMultiplayerSessionRestriction::Unknown };

    HRESULT hr = XblMultiplayerSearchHandleGetJoinRestriction(
        MPState()->searchHandle,
        &joinRestriction
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetJoinRestriction: restriction=%u, hr=%s", joinRestriction, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetSessionClosed_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetSessionClosed
    bool closed{ false };

    HRESULT hr = XblMultiplayerSearchHandleGetSessionClosed(
        MPState()->searchHandle,
        &closed
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetSessionClosed: closed=%d, hr=%s", closed, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetMemberCounts_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetMemberCounts
    size_t currentMembers{ 0 };
    size_t maxMembers{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetMemberCounts(
        MPState()->searchHandle,
        &maxMembers,
        &currentMembers
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetMemberCounts: max members=%u, current members=%u hr=%s", maxMembers, currentMembers, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetCreationTime_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetCreationTime
    time_t creationTime{ 0 };

    HRESULT hr = XblMultiplayerSearchHandleGetCreationTime(
        MPState()->searchHandle,
        &creationTime
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetCreationTime: creation time=%u hr=%s", creationTime, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSearchHandleGetCustomSessionPropertiesJson_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerSearchHandleGetCustomSessionPropertiesJson
    const char* properties{ nullptr };

    HRESULT hr = XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(
        MPState()->searchHandle,
        &properties
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerSearchHandleGetCustomSessionPropertiesJson: properties=%s hr=%s", properties, ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerCreateSearchHandleAsync_Lua(lua_State *L)
{
    auto xblContextHandle = Data()->xboxLiveContext;
    auto xblMultiplayerSessionReference = MPState()->sessionRef;

    // CODE SNIPPET START: XblMultiplayerCreateSearchHandleAsync_C
    size_t tagsCount = 1;
    XblMultiplayerSessionTag tags[1] = {};
    tags[0] = XblMultiplayerSessionTag{ "SessionTag" };

    size_t numberAttributesCount = 1;
    XblMultiplayerSessionNumberAttribute numberAttributes[1] = {};
    numberAttributes[0] = XblMultiplayerSessionNumberAttribute{ "numberattributename", 1.1 };

    size_t strAttributesCount = 1;
    XblMultiplayerSessionStringAttribute strAttributes[1] = {};
    strAttributes[0] = XblMultiplayerSessionStringAttribute{ "stringattributename", "string attribute value" };

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblMultiplayerSearchHandle searchHandle{ nullptr };
        HRESULT hr = XblMultiplayerCreateSearchHandleResult(asyncBlock, &searchHandle);
        MPState()->searchHandle = searchHandle; // CODE SNIP SKIP

        LogToFile("XblMultiplayerCreateSearchHandleResult completed with result=%s", ConvertHR(hr).data()); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            const char* handleId{ nullptr };
            XblMultiplayerSearchHandleGetId(searchHandle, &handleId);
            LogToFile("Search handle id = %s", handleId); // CODE SNIP SKIP
        }
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerCreateSearchHandleAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerCreateSearchHandleAsync(
        xblContextHandle,
        &xblMultiplayerSessionReference,
        tags,
        tagsCount,
        numberAttributes,
        numberAttributesCount,
        strAttributes,
        strAttributesCount,
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerCreateSearchHandleAsync: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerDeleteSearchHandleAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMultiplayerDeleteSearchHandleAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, true);
        LogToFile("XblMultiplayerDeleteSearchHandleAsync completed with result = %s", ConvertHR(hr).data());
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerDeleteSearchHandleAsync"); // CODE SNIP SKIP
    };

    const char* handleId{ nullptr };
    XblMultiplayerSearchHandleGetId(MPState()->searchHandle, &handleId);

    HRESULT hr = XblMultiplayerDeleteSearchHandleAsync(
        Data()->xboxLiveContext,
        handleId,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerDeleteSearchHandleAsync: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerGetSearchHandlesAsync_Lua(lua_State *L)
{
    auto xblContextHandle = Data()->xboxLiveContext;
    auto scid = Data()->scid;

    // CODE SNIPPET START: XblMultiplayerGetSearchHandlesAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultCount{ 0 };
        auto hr = XblMultiplayerGetSearchHandlesResultCount(asyncBlock, &resultCount);
        if (SUCCEEDED(hr) && resultCount > 0)
        {
            auto handles = new XblMultiplayerSearchHandle[resultCount];

            hr = XblMultiplayerGetSearchHandlesResult(asyncBlock, handles, resultCount);
            LogToFile("XblMultiplayerGetSearchHandlesResult: hr=%s", ConvertHR(hr).data()); // CODE SNIP SKIP

            if (SUCCEEDED(hr))
            {
                LogToFile("Got %u search handles:", resultCount); // CODE SNIP SKIP

                // Process handles
                for (auto i = 0u; i < resultCount; ++i)
                {
                    const char* handleId{ nullptr };
                    XblMultiplayerSearchHandleGetId(handles[i], &handleId);
                    LogToFile("\t%s", handleId); // CODE SNIP SKIP

                    XblMultiplayerSearchHandleCloseHandle(handles[i]);
                }
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetSearchHandlesAsync"); // CODE SNIP SKIP
    };

    const char* sessionName{ "MinGameSession" };
    const char* orderByAttribute{ nullptr };
    bool orderAscending{ false };
    const char* searchFilter{ nullptr };
    const char* socialGroup{ nullptr };

    HRESULT hr = XblMultiplayerGetSearchHandlesAsync(
        xblContextHandle,
        scid,
        sessionName,
        orderByAttribute,
        orderAscending,
        searchFilter,
        socialGroup,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblMultiplayerGetSearchHandlesAsync: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerSetTransferHandleAsync_Lua(lua_State* L)
{
    // Params:
    // 1) Target session index
    // 2) Origin session index
    auto targetIndex{ GetUint64FromLua(L, 1, 0) };
    auto originIndex{ GetUint64FromLua(L, 2, 0) };

    // CODE SNIPPET START: XblMultiplayerSetTransferHandleAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblMultiplayerSessionHandleId id{};
        auto hr = XblMultiplayerSetTransferHandleResult(asyncBlock, &id);
        if (SUCCEEDED(hr))
        {
            LogToFile("Sucessfully set transfer handle, ID = %s", id.value);
        }
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerSetTransferHandleAsync"); // CODE SNIP SKIP
    };

    auto targetReference = XblMultiplayerSessionSessionReference(MPState()->sessionHandles[static_cast<uint32_t>(targetIndex)]);
    auto originReference = XblMultiplayerSessionSessionReference(MPState()->sessionHandles[static_cast<uint32_t>(originIndex)]);

    HRESULT hr = S_OK;
    if (originReference != nullptr && targetReference != nullptr)
    {
        hr = XblMultiplayerSetTransferHandleAsync(
            Data()->xboxLiveContext,
            *targetReference,
            *originReference,
            asyncBlock.get()
        );
        if (SUCCEEDED(hr))
        {
            // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
            // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
            asyncBlock.release();
        }
    }

    // CODE SNIP END

    LogToFile("XblMultiplayerSetTransferHandleAsync: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblMatchmakingCreateTicket_Lua(lua_State* L)
{
    // Params:
    // 1) matchmaking hopper name
    // 2) attributes json
    // 3) SCID
    // 4) session template name
    // 5) session name
    // 6) timeout in seconds
    auto hopperName{ GetStringFromLua(L, 1, "PlayerSkillNoQoS") };
    auto attributesJson{ GetStringFromLua(L, 2, "{}") };
    uint32_t timeoutInSeconds = GetUint32FromLua(L, 6, 100);
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblMatchmakingCreateMatchTicketAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblCreateMatchTicketResponse result{};

        auto hr = XblMatchmakingCreateMatchTicketResult(asyncBlock, &result);
        LogToScreen("XblMatchmakingCreateMatchTicketResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            if (Data()->matchTicketResponse) { // CODE SNIP SKIP
                delete(Data()->matchTicketResponse); // CODE SNIP SKIP
            } // CODE SNIP SKIP

            Data()->matchTicketResponse = new XblCreateMatchTicketResponse(result); // CODE SNIP SKIP
            LogToScreen("CreateMatchTicketResponse->matchTicketId: %s", result.matchTicketId); // CODE SNIP SKIP
            LogToScreen("CreateMatchTicketResponse->estimatedWaitTime: %d", result.estimatedWaitTime); // CODE SNIP SKIP	
        }

        CallLuaFunctionWithHr(hr, "OnXblMatchmakingCreateTicket"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMatchmakingCreateMatchTicketAsync(
        Data()->xboxLiveContext,
        MPState()->sessionRef,
        Data()->scid,
        hopperName.c_str(),
        timeoutInSeconds,
        XblPreserveSessionMode::Never,
        attributesJson.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XblMatchmakingCreateMatchTicketAsync: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMatchmakingGetMatchTicketDetails_Lua(lua_State* L)
{
    // Params:
    // 1) matchmaking hopper name
    // 2) SCID
    // 3) match ticket ID
    std::string hopperName{ GetStringFromLua(L, 1, "PlayerSkillNoQoS") };
    std::string scid = GetStringFromLua(L, 2, Data()->scid);
    std::string ticketId = GetStringFromLua(L, 3, Data()->matchTicketResponse->matchTicketId);
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblMatchmakingGetMatchTicketDetailsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t bufferSize;
        auto hr = XblMatchmakingGetMatchTicketDetailsResultSize(asyncBlock, &bufferSize);
        LogToFile("XblMatchmakingGetMatchTicketDetailsResultSize: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(bufferSize, 0);
            XblMatchTicketDetailsResponse* resultPtr;
            hr = XblMatchmakingGetMatchTicketDetailsResult(asyncBlock, bufferSize, buffer.data(), &resultPtr, nullptr);
            LogToScreen("XblMatchmakingGetMatchTicketDetailsResult: hr= %s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
            if (SUCCEEDED(hr)) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                LogToScreen("XblMatchTicketDetailsResponse->matchStatus: %d", resultPtr->matchStatus); // CODE SNIP SKIP
                LogToScreen("XblMatchTicketDetailsResponse->estimatedWaitTime: %d", resultPtr->estimatedWaitTime); // CODE SNIP SKIP
                LogToScreen("XblMatchTicketDetailsResponse->preserveSession: %d", resultPtr->preserveSession); // CODE SNIP SKIP
                LogToScreen("XblMatchTicketDetailsResponse->ticketSession: SCID: %s, Session Name: %s, Session Template Name: %s", resultPtr->ticketSession.Scid, resultPtr->ticketSession.SessionName, resultPtr->ticketSession.SessionTemplateName); // CODE SNIP SKIP
                LogToScreen("XblMatchTicketDetailsResponse->targetSession: SCID: %s, Session Name: %s, Session Template Name: %s", resultPtr->targetSession.Scid, resultPtr->targetSession.SessionName, resultPtr->targetSession.SessionTemplateName); // CODE SNIP SKIP
                
                if (resultPtr->ticketAttributes != nullptr)
                {
                    LogToScreen("XblMatchTicketDetailsResponse->TicketAttributes: %d", resultPtr->ticketAttributes); // CODE SNIP SKIP
                }
            } // CODE SNIP SKIP
        }

        CallLuaFunctionWithHr(hr, "OnXblMatchmakingGetMatchTicketDetails"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMatchmakingGetMatchTicketDetailsAsync(
        Data()->xboxLiveContext,
        scid.c_str(),
        hopperName.c_str(),
        ticketId.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XblMatchmakingGetMatchTicketDetailsAsync: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMatchmakingGetHopperStatistics_Lua(lua_State* L)
{
    // Params:
    // 1) matchmaking hopper name
    // 2) SCID
    std::string hopperName{ GetStringFromLua(L, 1, "PlayerSkillNoQoS") };
    std::string scid = GetStringFromLua(L, 2, Data()->scid);
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblMatchmakingGetHopperStatisticsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t bufferSize;
        HRESULT hr = XblMatchmakingGetHopperStatisticsResultSize(asyncBlock, &bufferSize);
        LogToScreen("XblMatchmakingGetHopperStatisticsResultSize: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(bufferSize, 0);
            XblHopperStatisticsResponse* result{};
            hr = XblMatchmakingGetHopperStatisticsResult(asyncBlock, bufferSize, buffer.data(), &result, nullptr);
            LogToScreen("XblMatchmakingGetHopperStatisticsResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
            if (SUCCEEDED(hr)) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                LogToScreen("XblHopperStatisticsResponse->hopperName: %s", result->hopperName); // CODE SNIP SKIP
                LogToScreen("XblHopperStatisticsResponse->estimatedWaitTime: %d", result->estimatedWaitTime); // CODE SNIP SKIP
                LogToScreen("XblHopperStatisticsResponse->playersWaitingToMatch: %d", result->playersWaitingToMatch); // CODE SNIP SKIP

            } // CODE SNIP SKIP

            CallLuaFunctionWithHr(hr, "OnXblMatchmakingGetHopperStatistics"); // CODE SNIP SKIP
        }
    };

    HRESULT hr = XblMatchmakingGetHopperStatisticsAsync(
        Data()->xboxLiveContext,
        scid.c_str(),
        hopperName.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XblMatchmakingGetHopperStatisticsAsync: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMatchmakingDeleteMatchTicket_Lua(lua_State* L)
{
    // Params:
    // 1) matchmaking hopper name
    // 2) SCID
    // 3) ticket ID
    std::string hopperName{ GetStringFromLua(L, 1, "PlayerSkillNoQoS") };
    std::string scid = GetStringFromLua(L, 2, Data()->scid);
    std::string ticketId = GetStringFromLua(L, 3, Data()->matchTicketResponse->matchTicketId);
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblMatchmakingDeleteMatchTicketAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        CallLuaFunctionWithHr(S_OK, "OnXblMatchmakingDeleteMatchTicket"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMatchmakingDeleteMatchTicketAsync(
        Data()->xboxLiveContext,
        scid.c_str(),
        hopperName.c_str(),
        ticketId.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XblMatchmakingDeleteMatchTicketAsync: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblMultiplayer()
{
    lua_register(Data()->L, "XblMultiplayerSessionReferenceCreate", XblMultiplayerSessionReferenceCreate_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionReferenceParseFromUriPath", XblMultiplayerSessionReferenceParseFromUriPath_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionReferenceIsValid", XblMultiplayerSessionReferenceIsValid_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCreateHandle", XblMultiplayerSessionCreateHandle_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionDuplicateHandle", XblMultiplayerSessionDuplicateHandle_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCloseHandle", XblMultiplayerSessionCloseHandle_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionTimeOfSession", XblMultiplayerSessionTimeOfSession_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionGetInitializationInfo", XblMultiplayerSessionGetInitializationInfo_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSubscribedChangeTypes", XblMultiplayerSessionSubscribedChangeTypes_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionHostCandidates", XblMultiplayerSessionHostCandidates_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSessionReference", XblMultiplayerSessionSessionReference_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSessionConstants", XblMultiplayerSessionSessionConstants_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetMaxMembersInSession", XblMultiplayerSessionConstantsSetMaxMembersInSession_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetVisibility", XblMultiplayerSessionConstantsSetVisibility_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetTimeouts", XblMultiplayerSessionConstantsSetTimeouts_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetQosConnectivityMetrics", XblMultiplayerSessionConstantsSetQosConnectivityMetrics_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetMemberInitialization", XblMultiplayerSessionConstantsSetMemberInitialization_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetPeerToPeerRequirements", XblMultiplayerSessionConstantsSetPeerToPeerRequirements_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetPeerToHostRequirements", XblMultiplayerSessionConstantsSetPeerToHostRequirements_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson", XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetCapabilities", XblMultiplayerSessionConstantsSetCapabilities_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionConstantsSetCloudComputePackageJson", XblMultiplayerSessionConstantsSetCloudComputePackageJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSessionProperties", XblMultiplayerSessionSessionProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionPropertiesSetKeywords", XblMultiplayerSessionPropertiesSetKeywords_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionPropertiesSetJoinRestriction", XblMultiplayerSessionPropertiesSetJoinRestriction_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionPropertiesSetReadRestriction", XblMultiplayerSessionPropertiesSetReadRestriction_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionPropertiesSetTurnCollection", XblMultiplayerSessionPropertiesSetTurnCollection_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionRoleTypes", XblMultiplayerSessionRoleTypes_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionGetRoleByName", XblMultiplayerSessionGetRoleByName_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetMutableRoleSettings", XblMultiplayerSessionSetMutableRoleSettings_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionMembers", XblMultiplayerSessionMembers_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionGetMember", XblMultiplayerSessionGetMember_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionMatchmakingServer", XblMultiplayerSessionMatchmakingServer_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionMembersAccepted", XblMultiplayerSessionMembersAccepted_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionRawServersJson", XblMultiplayerSessionRawServersJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetRawServersJson", XblMultiplayerSessionSetRawServersJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionEtag", XblMultiplayerSessionEtag_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUser", XblMultiplayerSessionCurrentUser_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionGetInfo", XblMultiplayerSessionGetInfo_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionWriteStatus", XblMultiplayerSessionWriteStatus_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionAddMemberReservation", XblMultiplayerSessionAddMemberReservation_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionJoin", XblMultiplayerSessionJoin_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetInitializationSucceeded", XblMultiplayerSessionSetInitializationSucceeded_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetHostDeviceToken", XblMultiplayerSessionSetHostDeviceToken_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetMatchmakingServerConnectionPath", XblMultiplayerSessionSetMatchmakingServerConnectionPath_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetClosed", XblMultiplayerSessionSetClosed_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetLocked", XblMultiplayerSessionSetLocked_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetAllocateCloudCompute", XblMultiplayerSessionSetAllocateCloudCompute_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetMatchmakingResubmit", XblMultiplayerSessionSetMatchmakingResubmit_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetServerConnectionStringCandidates", XblMultiplayerSessionSetServerConnectionStringCandidates_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetSessionChangeSubscription", XblMultiplayerSessionSetSessionChangeSubscription_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionLeave", XblMultiplayerSessionLeave_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetStatus", XblMultiplayerSessionCurrentUserSetStatus_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64", XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64_Lua);
    lua_register(Data()->L, "XblFormatSecureDeviceAddress", XblFormatSecureDeviceAddress_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetRoles", XblMultiplayerSessionCurrentUserSetRoles_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetMembersInGroup", XblMultiplayerSessionCurrentUserSetMembersInGroup_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetGroups", XblMultiplayerSessionCurrentUserSetGroups_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetEncounters", XblMultiplayerSessionCurrentUserSetEncounters_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetQosMeasurements", XblMultiplayerSessionCurrentUserSetQosMeasurements_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetServerQosMeasurements", XblMultiplayerSessionCurrentUserSetServerQosMeasurements_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserSetCustomPropertyJson", XblMultiplayerSessionCurrentUserSetCustomPropertyJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson", XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson", XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionSetCustomPropertyJson", XblMultiplayerSessionSetCustomPropertyJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionDeleteCustomPropertyJson", XblMultiplayerSessionDeleteCustomPropertyJson_Lua);
    lua_register(Data()->L, "XblMultiplayerSessionCompare", XblMultiplayerSessionCompare_Lua);
    lua_register(Data()->L, "XblMultiplayerWriteSessionAsync", XblMultiplayerWriteSessionAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerWriteSessionByHandleAsync", XblMultiplayerWriteSessionByHandleAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetSessionAsync", XblMultiplayerGetSessionAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetSessionByHandleAsync", XblMultiplayerGetSessionByHandleAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerQuerySessionsAsync", XblMultiplayerQuerySessionsAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerSetActivityAsync", XblMultiplayerSetActivityAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerClearActivityAsync", XblMultiplayerClearActivityAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerSendInvitesAsync", XblMultiplayerSendInvitesAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetActivitiesForSocialGroupAsync", XblMultiplayerGetActivitiesForSocialGroupAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetActivitiesForUsersAsync", XblMultiplayerGetActivitiesForUsersAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync", XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetActivitiesWithPropertiesForUsersAsync", XblMultiplayerGetActivitiesWithPropertiesForUsersAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerSetSubscriptionsEnabled", XblMultiplayerSetSubscriptionsEnabled_Lua);
    lua_register(Data()->L, "XblMultiplayerSubscriptionsEnabled", XblMultiplayerSubscriptionsEnabled_Lua);
    lua_register(Data()->L, "XblMultiplayerAddSessionChangedHandler", XblMultiplayerAddSessionChangedHandler_Lua);
    lua_register(Data()->L, "XblMultiplayerRemoveSessionChangedHandler", XblMultiplayerRemoveSessionChangedHandler_Lua);
    lua_register(Data()->L, "XblMultiplayerAddSubscriptionLostHandler", XblMultiplayerAddSubscriptionLostHandler_Lua);
    lua_register(Data()->L, "XblMultiplayerRemoveSubscriptionLostHandler", XblMultiplayerRemoveSubscriptionLostHandler_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleDuplicateHandle", XblMultiplayerSearchHandleDuplicateHandle_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleCloseHandle", XblMultiplayerSearchHandleCloseHandle_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetSessionReference", XblMultiplayerSearchHandleGetSessionReference_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetId", XblMultiplayerSearchHandleGetId_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetSessionOwnerXuids", XblMultiplayerSearchHandleGetSessionOwnerXuids_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetTags", XblMultiplayerSearchHandleGetTags_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetStringAttributes", XblMultiplayerSearchHandleGetStringAttributes_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetNumberAttributes", XblMultiplayerSearchHandleGetNumberAttributes_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetVisibility", XblMultiplayerSearchHandleGetVisibility_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetJoinRestriction", XblMultiplayerSearchHandleGetJoinRestriction_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetSessionClosed", XblMultiplayerSearchHandleGetSessionClosed_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetMemberCounts", XblMultiplayerSearchHandleGetMemberCounts_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetCreationTime", XblMultiplayerSearchHandleGetCreationTime_Lua);
    lua_register(Data()->L, "XblMultiplayerSearchHandleGetCustomSessionPropertiesJson", XblMultiplayerSearchHandleGetCustomSessionPropertiesJson_Lua);
    lua_register(Data()->L, "XblMultiplayerCreateSearchHandleAsync", XblMultiplayerCreateSearchHandleAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerDeleteSearchHandleAsync", XblMultiplayerDeleteSearchHandleAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerGetSearchHandlesAsync", XblMultiplayerGetSearchHandlesAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerSetTransferHandleAsync", XblMultiplayerSetTransferHandleAsync_Lua);


    // XSAPI Matchmaking APIs
    lua_register(Data()->L, "XblMatchmakingCreateTicket", XblMatchmakingCreateTicket_Lua);
    lua_register(Data()->L, "XblMatchmakingGetMatchTicketDetails", XblMatchmakingGetMatchTicketDetails_Lua);
    lua_register(Data()->L, "XblMatchmakingGetHopperStatistics", XblMatchmakingGetHopperStatistics_Lua);
    lua_register(Data()->L, "XblMatchmakingDeleteMatchTicket", XblMatchmakingDeleteMatchTicket_Lua);
}

