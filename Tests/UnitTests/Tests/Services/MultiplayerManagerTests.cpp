// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"adityat"
#define TEST_CLASS_AREA L"MultiplayerManager"
#include "UnitTestIncludes.h"
#include "MultiplayerManager_WinRT.h"
#include "RtaTestHelper.h"
#include "multiplayer_manager_internal.h"
#include "FindMatchCompletedEventArgs_WinRT.h"
#include "JoinLobbyCompletedEventArgs_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Microsoft::Xbox::Services::Multiplayer::Manager;
using namespace Windows::ApplicationModel::Activation;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TITLE_ID                            0x116ACB82
#define GAME_SERVICE_CONFIG_ID              L"MockScid"
#define GAME_SESSION_TEMPLATE_NAME          L"MockGameSessionTemplateName"
#define LOBBY_TEMPLATE_NAME                 L"MockLobbySessionTemplateName"
#define HOPPER_NAME_NO_QOS                  L"PlayerSkillNoQoS"
#define HOPPER_NAME_WITH_QOS                L"PlayerSkill"

DEFINE_TEST_CLASS(MultiplayerManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MultiplayerManagerTests)

    const string_t filePath = _T("\\TestResponses\\MultiplayerManager.json");
    web::json::value testResponseJsonFromFile = utils::read_test_response_file(filePath);
    
    const string_t emptyJson = testResponseJsonFromFile[L"emptyJson"].serialize();
    const string_t classPropertiesJson = testResponseJsonFromFile[L"propertiesJson"].serialize();
    const string_t syncPropertiesJson = testResponseJsonFromFile[L"syncPropertiesJson"].serialize();
    const string_t propertiesNoTransferHandleJson = testResponseJsonFromFile[L"propertiesNoTransferHandleJson"].serialize();
    const string_t defaultLobbySessionResponse = testResponseJsonFromFile[L"defaultLobbySessionResponse"].serialize();
    const string_t defaultLobbySessionNoCustomMemberPropsResponse = testResponseJsonFromFile[L"defaultLobbySessionNoCustomMemberPropsResponse"].serialize();
    const string_t defaultMultipleLocalUsersLobbyResponse = testResponseJsonFromFile[L"defaultMultipleLocalUsersLobbyResponse"].serialize();
    const string_t multipleLocalUsersLobbyResponse = testResponseJsonFromFile[L"multipleLocalUsersLobbyResponse"].serialize();
    const string_t lobbyWithNoTransferHandleResponse = testResponseJsonFromFile[L"lobbyWithNoTransferHandleResponse"].serialize();
    const string_t updatedLobbyWithNoTransferHandleResponse = testResponseJsonFromFile[L"updatedLobbyWithNoTransferHandleResponse"].serialize();
    const string_t updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse = testResponseJsonFromFile[L"updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse"].serialize();
    const string_t lobbyWithPendingTransferHandleResponse = testResponseJsonFromFile[L"lobbyWithPendingTransferHandleResponse"].serialize();
    const string_t lobbyWithCompletedTransferHandleResponse = testResponseJsonFromFile[L"lobbyWithCompletedTransferHandleResponse"].serialize();
    const string_t defaultGameSessionResponse = testResponseJsonFromFile[L"defaultGameSessionResponse"].serialize();
    const string_t defaultGameSessionWithXuidsResponse = testResponseJsonFromFile[L"defaultGameSessionWithXuidsResponse"].serialize();
    const string_t defaultMultipleLocalUsersGameResponse = testResponseJsonFromFile[L"defaultMultipleLocalUsersGameResponse"].serialize();
    const string_t gameSessionResponseDiffXuid = testResponseJsonFromFile[L"gameSessionResponseDiffXuid"].serialize();
    const string_t sessionChangeNum2 = testResponseJsonFromFile[L"sessionChangeNum2"].serialize();
    const string_t sessionChangeNum3 = testResponseJsonFromFile[L"sessionChangeNum3"].serialize();
    const string_t sessionChangeNum4 = testResponseJsonFromFile[L"sessionChangeNum4"].serialize();
    const string_t sessionChangeNum5 = testResponseJsonFromFile[L"sessionChangeNum5"].serialize();
    const string_t sessionChangeNum6 = testResponseJsonFromFile[L"sessionChangeNum6"].serialize();
    const string_t sessionChangeNum8 = testResponseJsonFromFile[L"sessionChangeNum8"].serialize();
    const string_t matchTicket = testResponseJsonFromFile[L"matchTicket"].serialize();
    const string_t transferHandle = testResponseJsonFromFile[L"transferHandle"].serialize();
    const string_t lobbyMatchStatusSearching = testResponseJsonFromFile[L"lobbyMatchStatusSearching"].serialize();
    const string_t lobbyMatchStatusExpiredByService = testResponseJsonFromFile[L"lobbyMatchStatusExpiredByService"].serialize();
    const string_t lobbyMatchStatusFound = testResponseJsonFromFile[L"lobbyMatchStatusFound"].serialize();
    const string_t lobbyMatchStatusFoundWithTransHandle = testResponseJsonFromFile[L"lobbyMatchStatusFoundWithTransHandle"].serialize();
    const string_t matchSessionJoin_1 = testResponseJsonFromFile[L"matchSessionJoin_1"].serialize();
    const string_t matchSessionJoin_2 = testResponseJsonFromFile[L"matchSessionJoin_2"].serialize();
    const string_t matchSessionRemoteClientFailedToJoin = testResponseJsonFromFile[L"matchSessionRemoteClientFailedToJoin"].serialize();
    const string_t matchSessionMeasuring = testResponseJsonFromFile[L"matchSessionMeasuring"].serialize();
    const string_t matchSessionMeasuringWithQoS = testResponseJsonFromFile[L"matchSessionMeasuringWithQoS"].serialize();
    const string_t matchSessionMeasuringWithQoSComplete = testResponseJsonFromFile[L"matchSessionMeasuringWithQoSComplete"].serialize();
    const string_t matchRemoteClientFailedToUploadQoS = testResponseJsonFromFile[L"matchRemoteClientFailedToUploadQoS"].serialize();
    const string_t lobbyMatchStatusCanceledByService = testResponseJsonFromFile[L"lobbyMatchStatusCanceledByService"].serialize();

    web::json::value defaultLobbySessionResponseJson = web::json::value::parse(defaultLobbySessionResponse);
    web::json::value defaultLobbySessionNoCustomMemberPropsResponseJson = web::json::value::parse(defaultLobbySessionNoCustomMemberPropsResponse);
    web::json::value defaultGameSessionResponseJson = web::json::value::parse(defaultGameSessionResponse);
    web::json::value defaultGameSessionWithXuidsResponseJson = web::json::value::parse(defaultGameSessionWithXuidsResponse); 
    web::json::value defaultMultipleLocalUsersGameResponseJson = web::json::value::parse(defaultMultipleLocalUsersGameResponse);
    web::json::value gameSessionResponseDiffXuidJson = web::json::value::parse(gameSessionResponseDiffXuid);
    web::json::value lobbyNoHandleResponseJson = web::json::value::parse(lobbyWithNoTransferHandleResponse);
    web::json::value updatedLobbyNoHandleResponseJson = web::json::value::parse(updatedLobbyWithNoTransferHandleResponse);
    web::json::value updatedMultipleLocalUsersLobbyWithNoTransferHandleResponseJson = web::json::value::parse(updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse);
    web::json::value lobbyCompletedHandleResponseJson = web::json::value::parse(lobbyWithCompletedTransferHandleResponse);
    web::json::value defaultMultipleLocalUsersLobbyResponseJson = web::json::value::parse(defaultMultipleLocalUsersLobbyResponse);
    std::shared_ptr<http_call_response> defaultLobbyResponse = StockMocks::CreateMockHttpCallResponse(defaultLobbySessionResponseJson);
    std::shared_ptr<http_call_response> defaultLobbyNoCustomMemberPropsResponseJson = StockMocks::CreateMockHttpCallResponse(defaultLobbySessionNoCustomMemberPropsResponseJson);
    std::shared_ptr<http_call_response> lobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyNoHandleResponseJson);
    std::shared_ptr<http_call_response> lobbyWithPendingHandleResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyWithPendingTransferHandleResponse));
    std::shared_ptr<http_call_response> lobbyCompletedHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyCompletedHandleResponseJson);
    std::shared_ptr<http_call_response> updatedLobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(updatedLobbyWithNoTransferHandleResponse));
    std::shared_ptr<http_call_response> updatedMultipleLocalUsersLobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse));
    std::shared_ptr<http_call_response> defaultGameSessionHttpCallResponse = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson);
    std::shared_ptr<http_call_response> gameSessionEmptyJsonResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(emptyJson), DefaultGameHttpResponse());
    std::shared_ptr<http_call_response> lobbySessionEmptyJsonResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(emptyJson), DefaultLobbyHttpResponse());
    std::shared_ptr<http_call_response> gameSessionDiffXuidResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseDiffXuidJson);

    std::shared_ptr<http_call_response> sessionChangeNum2Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum2));
    std::shared_ptr<http_call_response> sessionChangeNum3Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum3));
    std::shared_ptr<http_call_response> sessionChangeNum4Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum4));
    std::shared_ptr<http_call_response> sessionChangeNum5Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum5));
    std::shared_ptr<http_call_response> sessionChangeNum6Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum6));
    std::shared_ptr<http_call_response> sessionChangeNum8Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(sessionChangeNum8));

    std::shared_ptr<http_call_response> matchTicketResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchTicket));
    std::shared_ptr<http_call_response> transferHandleResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(transferHandle));
    std::shared_ptr<http_call_response> matchStatusSearchingResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyMatchStatusSearching));
    std::shared_ptr<http_call_response> matchStatusExpiredByServiceResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyMatchStatusExpiredByService));
    std::shared_ptr<http_call_response> matchStatusCanceledByServiceResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyMatchStatusCanceledByService));
    std::shared_ptr<http_call_response> matchStatusFoundResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyMatchStatusFound));
    std::shared_ptr<http_call_response> matchStatusFoundWithTransHandleResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(lobbyMatchStatusFoundWithTransHandle));
    std::shared_ptr<http_call_response> matchJoin_1_Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionJoin_1));
    std::shared_ptr<http_call_response> matchJoin_2_Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionJoin_2));
    std::shared_ptr<http_call_response> matchRemoteClientFailedToJoin_Response = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionRemoteClientFailedToJoin));
    std::shared_ptr<http_call_response> matchMeasuringResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionMeasuring));
    std::shared_ptr<http_call_response> matchMeasuringWithQoSResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionMeasuringWithQoS));
    std::shared_ptr<http_call_response> matchMeasuringWithQoSCompleteResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchSessionMeasuringWithQoSComplete));
    std::shared_ptr<http_call_response> matchRemoteClientFailedToUploadQoSResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(matchRemoteClientFailedToUploadQoS));

    const string_t defaultGameHttpHeaderUri = _T("/serviceconfigs/MockScid/sessionTemplates/MockGameSessionTemplateName/sessions/MockGameSessionName");
    const string_t defaultMpsdUri = _T("https://sessiondirectory.mockenv.xboxlive.com");
    web::http::http_response DefaultGameHttpResponse()
    {
        web::http::http_response httpGameResponse;
        httpGameResponse.headers().add(L"ETag", L"MockETag");
        httpGameResponse.headers().add(L"Retry-After", L"1");
        httpGameResponse.headers().add(L"Content-Location", L"/serviceconfigs/MockScid/sessionTemplates/MockGameSessionTemplateName/sessions/MockGameSessionName");
        return httpGameResponse;
    }

    web::http::http_response DefaultLobbyHttpResponse()
    {
        web::http::http_response httpLobbyResponse;
        httpLobbyResponse.headers().add(L"ETag", L"MockETag");
        httpLobbyResponse.headers().add(L"Retry-After", L"1");
        httpLobbyResponse.headers().add(L"Content-Location", L"/serviceconfigs/MockScid/sessionTemplates/MockLobbySessionTemplateName/sessions/MockLobbySessionName");
        return httpLobbyResponse;
    }

    bool IsGameHost( Platform::String^ memberDeviceToken, MultiplayerGameSession^ gameSession)
    {
        if (memberDeviceToken->IsEmpty()) return false;
        if (gameSession == nullptr || gameSession->Host == nullptr) return false;

        return utils::str_icmp(memberDeviceToken->Data(), gameSession->Host->_DeviceToken->Data()) == 0;
    }

    bool IsLobbyHost(Platform::String^ memberDeviceToken, MultiplayerLobbySession^ lobbySession)
    {
        if (memberDeviceToken->IsEmpty()) return false;
        if (lobbySession == nullptr || lobbySession->Host == nullptr) return false;

        return utils::str_icmp(memberDeviceToken->Data(), lobbySession->Host->_DeviceToken->Data()) == 0;
    }

    bool IsPlayerInLobby(Platform::String^ xboxUserId, MultiplayerLobbySession^ lobbySession)
    {
        if (xboxUserId->IsEmpty()) return false;
        if (lobbySession == nullptr) return false;

        for each (auto member in lobbySession->Members)
        {
            if (utils::str_icmp(xboxUserId->Data(), member->XboxUserId->Data()) == 0)
            {
                return true;
            }
        }

        return false;
    }

    void InitializeManager(uint32_t numberOfWebSocketClientsToInit = 1)
    {
        const int subId = 666;
        m_mockXboxSystemFactory->reinit();
        auto mockSockets = m_mockXboxSystemFactory->AddMultipleMockWebSocketClients(numberOfWebSocketClientsToInit);
        const string_t rtaConnectionIdJson = testResponseJsonFromFile[L"rtaConnectionIdJson"].serialize();
        SetMultipleClientWebSocketRTAAutoResponser(mockSockets, rtaConnectionIdJson, subId);
        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->Initialize(LOBBY_TEMPLATE_NAME);
    }

    void DestructManager(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext, bool forceShutdhow =  false)
    {
        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        DestructManager(xboxLiveContexts->GetView(), forceShutdhow);
    }

    void DestructManager(IVectorView<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts, bool forceShutdhow = false)
    {
        auto mpInstance = MultiplayerManager::SingletonInstance;
        if (mpInstance->LobbySession->LocalMembers->Size == 0 || forceShutdhow)
        {
            mpInstance->_Shutdown();
            return;
        };

        m_mockXboxSystemFactory->clear_states();
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { lobbySessionEmptyJsonResponse };
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        size_t userRemoved = 0;
        for (auto xboxLiveContext : xboxLiveContexts)
        {
            try { mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User); }
            catch (Platform::Exception^ ex) { userRemoved++; }
        }

        bool clientDisconnected = false;
        while (userRemoved != xboxLiveContexts->Size || !clientDisconnected)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved++;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_TRUE(!clientDisconnected);
                    clientDisconnected  = true;
                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 0);
    }

    void VerifyMultiplayerMember(MultiplayerMember^ member, web::json::value resultToVerify)
    {
        web::json::value constantsJson = resultToVerify[L"constants"];
        web::json::value constantsSystemJson = constantsJson[L"system"];

        VERIFY_ARE_EQUAL_INT(member->MemberId, constantsSystemJson[L"index"].as_integer());
        VERIFY_ARE_EQUAL(member->XboxUserId->Data(), constantsSystemJson[L"xuid"].as_string());
        if (member->Status != MultiplayerSessionMemberStatus::Reserved)
        {
            VERIFY_ARE_EQUAL(member->DebugGamertag->Data(), resultToVerify[L"gamertag"].as_string());
            VERIFY_ARE_EQUAL(member->_DeviceToken->Data(), resultToVerify[L"deviceToken"].as_string());
            VERIFY_ARE_EQUAL(member->IsLocal, true);
            VERIFY_ARE_EQUAL_STR(member->ConnectionAddress, "AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7");
        }

        VERIFY_ARE_EQUAL(member->IsInLobby, IsPlayerInLobby(member->XboxUserId, MultiplayerManager::SingletonInstance->LobbySession));

        web::json::value propertiesJson = resultToVerify[L"properties"];
        web::json::value propertiesSystemJson = propertiesJson[L"system"];
        switch (member->Status)
        {
        case MultiplayerSessionMemberStatus::Active:
            VERIFY_IS_TRUE(propertiesSystemJson[L"active"].as_bool());
            break;
        case MultiplayerSessionMemberStatus::Ready:
            VERIFY_IS_TRUE(propertiesSystemJson[L"ready"].as_bool());
            break;
        case MultiplayerSessionMemberStatus::Reserved:
            VERIFY_IS_TRUE(resultToVerify[L"reserved"].as_bool());
            break;
        case MultiplayerSessionMemberStatus::Inactive:
            VERIFY_IS_FALSE(resultToVerify[L"reserved"].as_bool());
            VERIFY_IS_FALSE(propertiesSystemJson[L"active"].as_bool());
            VERIFY_IS_FALSE(propertiesSystemJson[L"ready"].as_bool());
            break;
        default:
            throw std::invalid_argument("Enum value out of range");
        }

        VERIFY_ARE_EQUAL(member->Properties->Data(), propertiesJson[L"custom"].serialize());
    }

    void VerifyLobby(MultiplayerLobbySession^ lobbySession, web::json::value resultToVerify)
    {
        web::json::value memberInfoJson = utils::extract_json_field(resultToVerify, _T("membersInfo"), false);
        web::json::value propertiesJson = resultToVerify[L"properties"];
        web::json::value propertiesSystemJson = propertiesJson[L"system"];

        web::json::value membersJson = resultToVerify[L"members"];
        uint32 memberCount = memberInfoJson[L"count"].as_integer();
        uint32 memberFirst = memberInfoJson[L"first"].as_integer();
        for (uint32 i = memberFirst; i < memberCount; ++i)
        {
            stringstream_t stream;
            stream << i;
            VerifyMultiplayerMember(lobbySession->LocalMembers->GetAt(i), membersJson[stream.str()]);
            VerifyMultiplayerMember(lobbySession->Members->GetAt(i), membersJson[stream.str()]);
        }

        // Session name is dynamically created by the Manager and not returned back in the service response.
        VERIFY_ARE_EQUAL_STR(lobbySession->SessionReference->ServiceConfigurationId, ref new Platform::String(L"MockScid"));
        VERIFY_ARE_EQUAL_STR(lobbySession->SessionReference->SessionTemplateName, ref new Platform::String(L"MockLobbySessionTemplateName"));

        VERIFY_ARE_EQUAL(lobbySession->CorrelationId->Data(), resultToVerify[L"correlationId"].as_string());
        if (lobbySession->Host != nullptr)
        {
            VERIFY_ARE_EQUAL(lobbySession->Host->_DeviceToken->Data(), propertiesSystemJson[L"host"].as_string());
        }
        VERIFY_ARE_EQUAL(lobbySession->Properties->Data(), propertiesJson[L"custom"].serialize());
    }

    void VerifyGame(MultiplayerGameSession^ gameSession, web::json::value resultToVerify)
    {
        web::json::value memberInfoJson = utils::extract_json_field(resultToVerify, _T("membersInfo"), false);
        web::json::value propertiesJson = resultToVerify[L"properties"];
        web::json::value propertiesSystemJson = propertiesJson[L"system"];

        web::json::value membersJson = resultToVerify[L"members"];
        uint32 memberCount = memberInfoJson[L"count"].as_integer();
        uint32 memberFirst = memberInfoJson[L"first"].as_integer();
        for (uint32 i = memberFirst; i < memberCount; ++i)
        {
            stringstream_t stream;
            stream << i;
            VerifyMultiplayerMember(gameSession->Members->GetAt(i), membersJson[stream.str()]);
        }

        VERIFY_ARE_EQUAL_STR(gameSession->SessionReference->ServiceConfigurationId, ref new Platform::String(L"MockScid"));
        VERIFY_ARE_EQUAL_STR(gameSession->SessionReference->SessionTemplateName, ref new Platform::String(L"MockGameSessionTemplateName"));

        VERIFY_ARE_EQUAL(gameSession->CorrelationId->Data(), resultToVerify[L"correlationId"].as_string());
        if (gameSession->Host != nullptr)
        {
            VERIFY_ARE_EQUAL(gameSession->Host->_DeviceToken->Data(), propertiesSystemJson[L"host"].as_string());
        }
        VERIFY_ARE_EQUAL(gameSession->Properties->Data(), propertiesJson[L"custom"].serialize());
    }

    void AddLocalUserHelper(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
    {
        AddLocalUserHelper(xboxLiveContext, defaultLobbySessionResponse);
    }

    void AddLocalUserHelper(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext, const string_t jsonLobbySessionResponse)
    {
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto jsonResponse = web::json::value::parse(jsonLobbySessionResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        multiplayerManagerInstance->LobbySession->AddLocalUser(xboxLiveContext->User);
        multiplayerManagerInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Health", L"89", (Platform::Object^) 1);
        multiplayerManagerInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Skill", L"17", (Platform::Object^) 2);
        multiplayerManagerInstance->LobbySession->SetLocalMemberConnectionAddress(xboxLiveContext->User, L"AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7", (Platform::Object^) 3);

        bool userAdded = false;
        int localUserPropWritten = 0;
        while (!userAdded || localUserPropWritten != 3)
        {
            auto events = multiplayerManagerInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    userAdded = true;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberConnectionAddressWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 3 );
                    localUserPropWritten++;
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, jsonResponse);
    }

    void AddLocalUserHelperWithSyncUpdate(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
    {
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext, defaultLobbySessionResponse);
    }

    void AddLocalUserHelperWithSyncUpdate(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext, const string_t jsonLobbySessionResponse)
    {
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto jsonResponse = web::json::value::parse(jsonLobbySessionResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->LobbySession->AddLocalUser(xboxLiveContext->User);
        mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Health", L"89", (Platform::Object^) 1);
        mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Skill", L"17", (Platform::Object^) 2);
        mpInstance->LobbySession->SetLocalMemberConnectionAddress(xboxLiveContext->User, L"AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7", (Platform::Object^) 3);
        mpInstance->LobbySession->SetProperties(L"Map", L"1", (Platform::Object^) 4);
        mpInstance->LobbySession->SetSynchronizedProperties(L"Map", L"2", (Platform::Object^) 5);

        bool userAdded = false;
        int localUserPropWritten = 0, propWritten = 0, syncPropWritten = 0, eventValue = 0;
        int contextId = 6;
        while (!userAdded || localUserPropWritten != 3 || propWritten != 4 || syncPropWritten != 4)
        {
            auto events = mpInstance->DoWork();
            if (contextId < 12)
            {
                mpInstance->LobbySession->SetProperties(L"Map", L"3", (Platform::Object^) contextId++);
                mpInstance->LobbySession->SetSynchronizedProperties(L"Map", L"4", (Platform::Object^) contextId++);
            }

            for (auto ev : events)
            {
                VERIFY_IS_TRUE(ev->SessionType == MultiplayerSessionType::LobbySession);

                if (ev->Context != nullptr)
                {
                    int context = safe_cast<int>(ev->Context);
                    TEST_LOG(FormatString(L" [MPM] AddLocalUserHelperWithSyncUpdate - Event type: %s - Context: eventValue = %s:%s", ev->EventType.ToString()->Data(), context.ToString()->Data(), eventValue.ToString()->Data()).c_str());
                    VERIFY_IS_TRUE(context == eventValue);
                }
                else
                {
                    TEST_LOG(FormatString(L" [MPM] AddLocalUserHelperWithSyncUpdate - Event type: %s - EventValue = %s", ev->EventType.ToString()->Data(), eventValue.ToString()->Data()).c_str());
                }

                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    userAdded = true;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberConnectionAddressWriteCompleted)
                {
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::SessionPropertyWriteCompleted)
                {
                    propWritten++;
                }

                if (ev->EventType == MultiplayerEventType::SessionSynchronizedPropertyWriteCompleted)
                {
                    syncPropWritten++;
                }

                eventValue++;
            }
        }

        VerifyLobby(mpInstance->LobbySession, jsonResponse);
    }

    void AddMultipleLocalUserHelper(IVectorView<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts)
    {
        AddMultipleLocalUserHelper(xboxLiveContexts, defaultMultipleLocalUsersLobbyResponse);
    }

    void AddMultipleLocalUserHelper(IVectorView<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts, const string_t jsonLobbySessionResponse)
    {
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto jsonResponse = web::json::value::parse(jsonLobbySessionResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        for (auto xboxLiveContext : xboxLiveContexts)
        {
            multiplayerManagerInstance->LobbySession->AddLocalUser(xboxLiveContext->User);
            multiplayerManagerInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Health", L"89", (Platform::Object^) 1);
            multiplayerManagerInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Skill", L"17", (Platform::Object^) 2);
            multiplayerManagerInstance->LobbySession->SetLocalMemberConnectionAddress(xboxLiveContext->User, L"AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7", (Platform::Object^) 3);
        }
        
        size_t usersAdded = 0;
        size_t localUserPropWritten = 0;
        while (usersAdded != xboxLiveContexts->Size || localUserPropWritten != (xboxLiveContexts->Size * 3) )
        {
            auto events = multiplayerManagerInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    usersAdded++;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberConnectionAddressWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 3);
                    localUserPropWritten++;
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, jsonResponse);
    }

    void RemoveLocalUserHelper(Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
    {
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        auto jsonResponse = web::json::value::parse(emptyJson);
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);
        multiplayerManagerInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);

        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            auto events = multiplayerManagerInstance->DoWork();
            for (auto ev : events)
            {
                TEST_LOG(FormatString(L" [MPM] RemoveLocalUserHelper - Event type: %s", ev->EventType.ToString()->Data()).c_str());
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_TRUE(!clientDisconnected);
                    clientDisconnected = true;
                    
                }
            }
        }

        VERIFY_IS_TRUE(multiplayerManagerInstance->LobbySession->LocalMembers->Size == 0);
        VERIFY_IS_TRUE(multiplayerManagerInstance->GameSession == nullptr);
    }

    void JoinLobbyWithValidHandleId(
        Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext,
        Windows::Foundation::Uri^ url = nullptr,
        bool checkForInvalidArg = false,
        bool checkForLogicErr = false)
    {
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto tlobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyNoHandleResponseJson, DefaultLobbyHttpResponse());
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            tlobbyNoHandleResponse
        };

        // set up http response set
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        if (url != nullptr)
        {
            // This is a way to bypass the ProtocolActivation URL
            std::vector<xbox_live_user_t> users;
            users.push_back(user_context::user_convert(xboxLiveContext->User));
            auto clientManager = multiplayerManagerInstance->GetCppObj()->_Get_multiplayer_client_manager();
            auto result = clientManager->join_lobby(url, users);
            if (checkForInvalidArg)
            {
                VERIFY_IS_TRUE(result.err() == xbox_live_error_code::invalid_argument);
                return;
            }
            else if (checkForLogicErr)
                VERIFY_IS_TRUE(result.err() == xbox_live_error_code::logic_error);
            else
                VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
        }
        else
        {
            multiplayerManagerInstance->JoinLobby(ref new Platform::String(L"TestHandleId"), xboxLiveContext->User);
        }

        bool lobbyJoined = false;
        while (!lobbyJoined)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                    if (checkForLogicErr)
                    {
                        VERIFY_IS_TRUE(ev->ErrorCode == E_UNEXPECTED);

                        auto joinLobbyEventArgs = static_cast<JoinLobbyCompletedEventArgs^>(ev->EventArgs);
                        VERIFY_ARE_EQUAL_STR(joinLobbyEventArgs->InvitedXboxUserId->Data(), L"Xuid");
                        return;
                    }
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, lobbyNoHandleResponseJson);
        VERIFY_IS_TRUE(multiplayerManagerInstance->GameSession == nullptr);
    }

    DEFINE_TEST_CASE(TestAddLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAddLocalUser);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestRemoveLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRemoveLocalUser);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        RemoveLocalUserHelper(xboxLiveContext);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestReAddAfterRemovingLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestReAddAfterRemovingLocalUser);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        RemoveLocalUserHelper(xboxLiveContext);
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestDeleteLocalMemberProperties)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestDeleteLocalMemberProperties);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        // Set up initial http responses
        auto tlobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyNoHandleResponseJson, DefaultLobbyHttpResponse());
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { defaultLobbyNoCustomMemberPropsResponseJson };

        // set up http response set
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);


        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->LobbySession->DeleteLocalMemberProperties(xboxLiveContext->User, L"Health", (Platform::Object^) 1);
        mpInstance->LobbySession->DeleteLocalMemberProperties(xboxLiveContext->User, L"Skill", (Platform::Object^) 2);

        int localUserPropWritten = 0, eventValue = 1;
        while (localUserPropWritten != 2)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    int context = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(context == eventValue);
                    localUserPropWritten++;
                }
                eventValue++;
            }
        }

        auto member = mpInstance->LobbySession->Members->GetAt(0);
        VERIFY_ARE_EQUAL_STR(member->Properties, "{}");
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSetSynchronizedLobbyProperties)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetSynchronizedLobbyProperties);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            sessionChangeNum4Response,       // change #4
            sessionChangeNum6Response        // change #6
        };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->LobbySession->SetSynchronizedProperties(L"Map", L"MyTestMap", (Platform::Object^) 1);
        mpInstance->LobbySession->SetSynchronizedProperties(L"GameMode", L"MyTestGameMode", (Platform::Object^) 2);
        mpInstance->LobbySession->GetCppObj()->_Set_host(nullptr);
        mpInstance->LobbySession->SetSynchronizedHost(mpInstance->LobbySession->LocalMembers->GetAt(0), (Platform::Object^) 3);

        int syncPropWritten = 0;
        int syncHostWritten = 0;
        while (syncPropWritten != 2 || syncHostWritten != 1)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                VERIFY_IS_TRUE(ev->SessionType == MultiplayerSessionType::LobbySession);
                if (ev->EventType == MultiplayerEventType::SessionSynchronizedPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    syncPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::SynchronizedHostWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 3);
                    syncHostWritten++;
                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 4);

        auto writeJson = web::json::value::parse(syncPropertiesJson);
        web::json::value propertyJson = writeJson[L"properties"];
        VERIFY_ARE_EQUAL(mpInstance->LobbySession->Properties->Data(), propertyJson[L"custom"].serialize());
        VERIFY_ARE_EQUAL_STR(mpInstance->LobbySession->Host->_DeviceToken->Data(), L"TestHostDeviceToken");

        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSetSynchronizedGameProperties)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetSynchronizedGameProperties);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            sessionChangeNum4Response,       // change #4
            sessionChangeNum6Response        // change #6
        };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->GameSession->SetSynchronizedProperties(L"Map", L"MyTestMap", (Platform::Object^) 1);
        mpInstance->GameSession->SetSynchronizedProperties(L"GameMode", L"MyTestGameMode", (Platform::Object^) 2);
        mpInstance->GameSession->GetCppObj()->_Set_host(nullptr);
        mpInstance->GameSession->SetSynchronizedHost(mpInstance->GameSession->Members->GetAt(0), (Platform::Object^) 3);

        int syncPropWritten = 0;
        int syncHostWritten = 0;
        while (syncPropWritten != 2 || syncHostWritten != 1)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::SessionSynchronizedPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    VERIFY_IS_TRUE(ev->SessionType == MultiplayerSessionType::GameSession);
                    syncPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::SynchronizedHostWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 3);
                    VERIFY_IS_TRUE(ev->SessionType == MultiplayerSessionType::GameSession);
                    syncHostWritten++;
                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 4);

        auto writeJson = web::json::value::parse(syncPropertiesJson);
        web::json::value propertyJson = writeJson[L"properties"];
        VERIFY_ARE_EQUAL(mpInstance->GameSession->Properties->Data(), propertyJson[L"custom"].serialize());
        VERIFY_ARE_EQUAL_STR(mpInstance->GameSession->Host->_DeviceToken, "TestHostDeviceToken");

        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestLeaveGame)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveGame);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            updatedLobbyNoHandleResponse        // clear_game_session_from_lobby
        }; 
        
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionEmptyJsonResponse };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->LeaveGame();

        auto propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool leaveGameCompleted = false, isStopAdvertisingGameDone = false; 
        while (!leaveGameCompleted || !isStopAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::LeaveGameCompleted)
                {
                    leaveGameCompleted = true;
                }
            }

            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, updatedLobbyNoHandleResponseJson);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestInviteUsers)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestInviteUsers);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        Vector<Platform::String^>^ xuids = ref new Vector<Platform::String^>();
        xuids->Append(L"1234");
        xuids->Append(L"5678");
#pragma warning(suppress: 6387)
        mpInstance->LobbySession->InviteUsers(xboxLiveContext->User, xuids->GetView(), nullptr, nullptr);

        bool inviteSent = false;
        while (!inviteSent)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::InviteSent)
                {
                    inviteSent = true;
                }
            }
        }

        DestructManager(xboxLiveContext);
    }

    /*  Join Lobby Tests:
        1. Test with valid handleId with no transfer handle
        2. Test with invalid handleId
        3. Test with valid handleId & valid transfer handle (so that you can join the game).
        4. Test with valid handleId & invalid transfer handle (don't create the game but clear the game session)
        5. Test switching lobbies
    */
    DEFINE_TEST_CASE(TestJoinLobbyWithValidHandleId)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithValidHandleId);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinLobbyWithValidHandleId(xboxLiveContext);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithValidHandleIdWithEventArgs_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithValidHandleIdWithEventArgs_1);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://inviteHandleAccept?invitedXuid=TestXboxUserId&handle=TestHandleId"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithValidHandleIdWithEventArgs_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithValidHandleIdWithEventArgs_2);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://activityHandleJoin?joinerXuid=TestXboxUserId&handle=TestHandleId"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidArgs_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithInvalidArgs_1);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://inviteHandleAccept?invitedXuid=Xuid&handle=TestHandleId"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url, false, true);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidArgs_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithInvalidArgs_2);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://activityHandleJoin?joinerXuid=Xuid&handle=TestHandleId"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url, false, true);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithTournamentSessionRef_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithTournamentSessionRef_1);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://tournament?action=joinGame&joinerXuid=TestXboxUserId&scid=MockScid&templateName=MockLobbySessionTemplateName&name=MockSessionName"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url, false, false);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithTournamentSessionRef_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithTournamentSessionRef_2);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto url = ref new Windows::Foundation::Uri(ref new Platform::String(L"ms-xbl-multiplayer://tournament?action=testAction&joinerXuid=TestXboxUserId&scid=MockScid&templateName=MockLobbySessionTemplateName&name=MockSessionName"));
        JoinLobbyWithValidHandleId(xboxLiveContext, url, true, false);       // wrong action param passed
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidHandleId)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithInvalidHandleId);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto tlobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyNoHandleResponseJson, 404, DefaultLobbyHttpResponse());
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            tlobbyNoHandleResponse
        };

        // set up http response set
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        multiplayerManagerInstance->JoinLobby(ref new Platform::String(L"TestHandleId"), xboxLiveContext->User);

        bool lobbyJoined = false;
        while (!lobbyJoined)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                    VERIFY_IS_TRUE(ev->ErrorCode == HTTP_E_STATUS_NOT_FOUND);
                }
            }
        }

        VERIFY_IS_TRUE(multiplayerManagerInstance->LobbySession->LocalMembers->Size == 0);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithValidTransferHandle)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithValidTransferHandle);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto tlobbyCompletedHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyCompletedHandleResponseJson, DefaultLobbyHttpResponse());
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson, DefaultGameHttpResponse());

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            tlobbyCompletedHandleResponse,       // commit_lobby_changes_helper (join via handleId)
            tlobbyCompletedHandleResponse,       // commit_lobby_changes_helper (set_activity)
        };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList =
        {
            gameSessionResponse                 // join_game_for_all_local_members_helper
        };

        // set up http response set
        responses[_T("/handles/TestGameSessionTransferHandle/session")] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        multiplayerManagerInstance->JoinLobby(ref new Platform::String(L"TestHandleId"), xboxLiveContext->User);

        bool lobbyJoined = false;
        bool gameJoined = false;
        while (!lobbyJoined || !gameJoined)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                }

                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    gameJoined = true;
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, lobbyCompletedHandleResponseJson);
        VerifyGame(multiplayerManagerInstance->GameSession, defaultGameSessionResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidTransferHandle)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinLobbyWithInvalidTransferHandle);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        
        auto tlobbyCompletedHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyCompletedHandleResponseJson, DefaultLobbyHttpResponse());
        auto gameSessionResponse404 = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson, 404, DefaultGameHttpResponse());
        
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            tlobbyCompletedHandleResponse,       // commit_lobby_changes_helper (join via handleId)
            tlobbyCompletedHandleResponse,       // commit_lobby_changes_helper (set_activity)
            updatedLobbyNoHandleResponse        // clear_game_session_from_lobby
        };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList =
        {
            gameSessionResponse404,             // join_game_for_all_local_members_helper
        };

        // set up http response set
        responses[_T("/handles/TestGameSessionTransferHandle/session")] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        multiplayerManagerInstance->JoinLobby(ref new Platform::String(L"TestHandleId"), xboxLiveContext->User);

        bool lobbyJoined = false;
        bool gameJoined = false;
        bool isStopAdvertisingGameDone = false;
        auto propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        auto customPropertyJson = propertyJson[L"properties"];
        while (!lobbyJoined || !gameJoined || !isStopAdvertisingGameDone)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                }

                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    gameJoined = true;
                    VERIFY_IS_TRUE(ev->ErrorCode == HTTP_E_STATUS_NOT_FOUND);
                }
            }

            if (utils::str_icmp(multiplayerManagerInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, updatedLobbyNoHandleResponseJson);
        VERIFY_IS_TRUE(multiplayerManagerInstance->GameSession == nullptr);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSwitchingLobbies)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSwitchingLobbies);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        // Join a new lobby
        JoinLobbyWithValidHandleId(xboxLiveContext);
        DestructManager(xboxLiveContext);
    }

    /*
        1. Test With Transfer Handle
        2. Test with transfer handle but failed to join game. So create a new game
        3. Test without transfer handle i.e. create a new game
        4. Test without transfer handle but failed to create new game
        5. Test without transfer handle i.e. create a new game but failed to advertise game session (TODO)
        6. Test without transfer handle - failed to write "pending~" with 412 with no transfer handle written (TODO)
            : retry writing, or both clients will end up creating it's own session.
    */

    void JoinGameFromLobbyMultipleUsersHelper(IVectorView<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts)
    {
        AddMultipleLocalUserHelper(xboxLiveContexts);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(defaultMultipleLocalUsersGameResponseJson, DefaultGameHttpResponse());
        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse /* join_game_for_all_local_members */ };

        // set up http response set
        responses[_T("/handles/TestGameSessionTransferHandle/session")] = gameResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        multiplayerManagerInstance->JoinGameFromLobby(GAME_SESSION_TEMPLATE_NAME);

        bool isDone = false;
        while (!isDone)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, defaultMultipleLocalUsersLobbyResponseJson);
        VerifyGame(multiplayerManagerInstance->GameSession, defaultMultipleLocalUsersGameResponseJson);
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyWithTransferHandle)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestJoinGameFromLobbyWithTransferHandle);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithCompletedTransferHandleResponse);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson, DefaultGameHttpResponse());

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse /* join_game_for_all_local_members */ };

        // set up http response set
        responses[_T("/handles/TestGameSessionTransferHandle/session")] = gameResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        multiplayerManagerInstance->JoinGameFromLobby(GAME_SESSION_TEMPLATE_NAME);

        bool isDone = false;
        while (!isDone)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, lobbyCompletedHandleResponseJson);
        VerifyGame(multiplayerManagerInstance->GameSession, defaultGameSessionResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyWithTransferHandleMultipleUsers)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestJoinGameFromLobbyWithTransferHandleMultipleUsers);
        InitializeManager(2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts->GetView());
        DestructManager(xboxLiveContexts->GetView());
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyFailedToJoin)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameFromLobbyFailedToJoin);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, defaultLobbySessionResponse);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto gameSessionResponseJson = defaultGameSessionResponseJson;
        auto gameSessionResponse404 = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, 404, DefaultGameHttpResponse());
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, DefaultGameHttpResponse());

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList =
        {
            gameSessionResponse404,                 // Failed to join game via transfer handle
            gameSessionResponse                     // join_game_helper -> join_game_for_all_local_members
        };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,         // create_game_from_lobby
            gameSessionResponse,                    // join_game_helper -> join_game_for_all_local_members (because it creates a new guid)
            lobbyCompletedHandleResponse            // advertise_game_session
        };

        // set up http response set
        responses[_T("/handles/TestGameSessionTransferHandle/session")] = gameResponseStruct;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        multiplayerManagerInstance->JoinGameFromLobby(GAME_SESSION_TEMPLATE_NAME);

        bool isGameJoined = false;
        bool isAdvertisingGameDone = false;
        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        while (!isGameJoined || !isAdvertisingGameDone)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    isGameJoined = true;
                }
            }

            if (isGameJoined && utils::str_icmp(multiplayerManagerInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isAdvertisingGameDone = true;
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, lobbyCompletedHandleResponseJson);
        VerifyGame(multiplayerManagerInstance->GameSession, gameSessionResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyNoHandleCreateNewGame)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameFromLobbyNoHandleCreateNewGame);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson);
        std::shared_ptr<HttpResponseStruct> joinLobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        joinLobbyResponseStruct->responseList = 
        { 
            lobbyWithPendingHandleResponse,     // create_game_from_lobby
            gameSessionResponse,                // join_game_helper -> join_game_for_all_local_members
            lobbyCompletedHandleResponse        // advertise_game_session
        };

        // set up http response set
        responses[defaultMpsdUri] = joinLobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        multiplayerManagerInstance->JoinGameFromLobby(GAME_SESSION_TEMPLATE_NAME);

        bool isDone = false;
        bool isAdvertisingGameDone = false;
        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        while (!isDone || !isAdvertisingGameDone)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }

            if (utils::str_icmp(multiplayerManagerInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isAdvertisingGameDone = true;
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, lobbyCompletedHandleResponseJson);
        VerifyGame(multiplayerManagerInstance->GameSession, defaultGameSessionResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyNoHandleFailedToCreateNewGame)
    {
        // When MPM fails to create a new game, it simply reports the failure back to the title.
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameFromLobbyNoHandleFailedToCreateNewGame);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto gameSessionResponse404 = StockMocks::CreateMockHttpCallResponse(defaultGameSessionResponseJson, 404, DefaultGameHttpResponse());

        std::shared_ptr<HttpResponseStruct> joinLobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        joinLobbyResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,      // create_game_from_lobby
            gameSessionResponse404,              // join_game_for_all_local_members_helper
            updatedLobbyNoHandleResponse         // clear_game_session_from_lobby
        };

        // set up http response set
        responses[defaultMpsdUri] = joinLobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        mpInstance->JoinGameFromLobby(GAME_SESSION_TEMPLATE_NAME);

        bool isDone = false, isStopAdvertisingGameDone = false;
        auto propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        auto customPropertyJson = propertyJson[L"properties"];
        while (!isDone || !isStopAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                    VERIFY_IS_TRUE(ev->ErrorCode == HTTP_E_STATUS_NOT_FOUND);
                }
            }

            if (isDone && utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, updatedLobbyNoHandleResponseJson);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        DestructManager(xboxLiveContext);
    }

    void JoinGameHelper(
        Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext,
        Windows::Foundation::Collections::IVector<Platform::String^>^ xboxUserIds = nullptr
        )
    {
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        // Set up initial http responses
        auto gameSessionResponseJson = defaultGameSessionResponseJson;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, DefaultGameHttpResponse());
        auto gameSessionWithXuidsResponseJson = defaultGameSessionWithXuidsResponseJson;
        auto gameSessionWithXuidsResponse = StockMocks::CreateMockHttpCallResponse(gameSessionWithXuidsResponseJson, DefaultGameHttpResponse());
        auto tlobbyNoHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyNoHandleResponseJson, DefaultLobbyHttpResponse());
        auto tlobbyCompletedHandleResponse = StockMocks::CreateMockHttpCallResponse(lobbyCompletedHandleResponseJson, DefaultLobbyHttpResponse());

        /*
            join_game_helper -> join_game_for_all_local_members
            advertise_game_session -> commit_pending_lobby_changes -> commit_lobby_changes_helper (write_session)
            set_local_member_properties_to_remote_session
            commit_lobby_changes_helper (set_activity)
            advertise_game_session -> set_transfer_handle
            advertise_game_session -> write_session (transfer handle)
        */

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        if (xboxUserIds != nullptr)
        {
            gameResponseStruct->responseList =
            {
                gameSessionWithXuidsResponse,     // join_game_helper -> join_game_for_all_local_members
                gameSessionWithXuidsResponse      // set_local_member_properties_to_remote_session
            };
        }
        else
        {
            gameResponseStruct->responseList =
            {
                gameSessionResponse,                // join_game_helper -> join_game_for_all_local_members
                gameSessionResponse                 // set_local_member_properties_to_remote_session
            };
        }

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            tlobbyNoHandleResponse,              // advertise_game_session -> commit_pending_lobby_changes -> commit_lobby_changes_helper (write_session)
            tlobbyNoHandleResponse,              // commit_lobby_changes_helper (set_activity)
            tlobbyNoHandleResponse,              // advertise_game_session -> set_transfer_handle
            lobbyCompletedHandleResponse        // advertise_game_session -> write_session (transfer handle)
        };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;

        mpInstance->LobbySession->AddLocalUser(xboxLiveContext->User);

        mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Health", L"89", (Platform::Object^) 1);
        mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"Skill", L"17", (Platform::Object^) 2);
        mpInstance->LobbySession->SetLocalMemberConnectionAddress(xboxLiveContext->User, L"AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7", (Platform::Object^) 3);
        if (xboxUserIds != nullptr)
        {
            mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, xboxUserIds->GetView());
        }
        else
        {
#pragma warning(suppress: 6387)
            mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr);
        }

        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool lobbyJoined = false;
        bool gameJoined = false;
        bool isAdvertisingGameDone = false;
        int localUserPropWritten = 0;
        while (!lobbyJoined || !gameJoined || !isAdvertisingGameDone || localUserPropWritten != 3)
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    lobbyJoined = true;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberConnectionAddressWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 3);
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::JoinGameCompleted)
                {
                    gameJoined = true;
                }
            }

            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, lobbyCompletedHandleResponseJson);
        if (xboxUserIds != nullptr)
        {
            VerifyGame(mpInstance->GameSession, gameSessionWithXuidsResponseJson);
        }
        else
        {
            VerifyGame(mpInstance->GameSession, gameSessionResponseJson);
        }

        m_mockXboxSystemFactory->reinit();
    }

    DEFINE_TEST_CASE(TestJoinGame)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGame);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestJoinGameWithXuids)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGame);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Vector<Platform::String^>^ initiatorIds = ref new Vector<Platform::String^>();
        initiatorIds->Append("TestXboxUserId");
        initiatorIds->Append("TestXboxUserId_2");
        initiatorIds->Append("TestXboxUserId_3");
        JoinGameHelper(xboxLiveContext, initiatorIds);
        DestructManager(xboxLiveContext);
    }

    enum class CallingPatternType
    {
        Sync,
        Async,
        ReverseSync,
        ReverseAsync,
        Combination,
        ReverseCombination
    };

    void TestLeaveMultiplayerHelper(CallingPatternType type)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionEmptyJsonResponse };
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { lobbySessionEmptyJsonResponse };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        bool leaveGameCompleted = false;

        if (type == CallingPatternType::Sync)
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
        }
        else if (type == CallingPatternType::ReverseSync)
        {
            try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
        }
        else if (type == CallingPatternType::Async)
        {
            create_task([mpInstance, xboxLiveContext]()
            {
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });

            create_task([mpInstance, &leaveGameCompleted]()
            {
                try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
            });
        }
        else if (type == CallingPatternType::ReverseAsync)
        {
            create_task([mpInstance, &leaveGameCompleted]()
            {
                try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
            });
            create_task([mpInstance, xboxLiveContext]()
            {
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });
        }
        else if(type == CallingPatternType::Combination)
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            create_task([mpInstance, &leaveGameCompleted]()
            {
                try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
            });
        }
        else
        {
            try { mpInstance->LeaveGame(); } catch(Platform::Exception^ ex) { leaveGameCompleted = true; }
            create_task([mpInstance, xboxLiveContext]()
            {
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });
        }

        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !leaveGameCompleted || !clientDisconnected)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                    TEST_LOG(L"TestLeaveMultiplayerHelper - user removed.");
                }

                if (ev->EventType == MultiplayerEventType::LeaveGameCompleted)
                {
                    leaveGameCompleted = true;
                    TEST_LOG(L"TestLeaveMultiplayerHelper - leave game completed.");
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_TRUE(!clientDisconnected);
                    clientDisconnected = true;
                    TEST_LOG(L"TestLeaveMultiplayerHelper - client disconnected from multiplayer service.");
                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 0);
        VERIFY_IS_TRUE(mpInstance->LobbySession->Members->Size == 0);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_Master)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestLeaveMultiplayer_Master);
        {
            TEST_LOG(L"TestLeaveMultiplayerHelper - Sync.");
            TestLeaveMultiplayerHelper(CallingPatternType::Sync);

            TEST_LOG(L"TestLeaveMultiplayerHelper - Async.");
            TestLeaveMultiplayerHelper(CallingPatternType::Async);

            TEST_LOG(L"TestLeaveMultiplayerHelper - Combination.");
            TestLeaveMultiplayerHelper(CallingPatternType::Combination);

            TEST_LOG(L"TestLeaveMultiplayerHelper - ReverseSync.");
            TestLeaveMultiplayerHelper(CallingPatternType::ReverseSync);

            TEST_LOG(L"TestLeaveMultiplayerHelper - ReverseAsync.");
            TestLeaveMultiplayerHelper(CallingPatternType::ReverseAsync);

            TEST_LOG(L"TestLeaveMultiplayerHelper - ReverseCombination.");
            TestLeaveMultiplayerHelper(CallingPatternType::ReverseCombination);
        }
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_1);
        TestLeaveMultiplayerHelper(CallingPatternType::Sync);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_2);
        TestLeaveMultiplayerHelper(CallingPatternType::Async);
    }

     DEFINE_TEST_CASE(TestLeaveMultiplayer_3)
     {
         DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_3);
         TestLeaveMultiplayerHelper(CallingPatternType::Combination);
     }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_4)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_4);
        TestLeaveMultiplayerHelper(CallingPatternType::ReverseSync);
    }
    DEFINE_TEST_CASE(TestLeaveMultiplayer_5)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_5);
        TestLeaveMultiplayerHelper(CallingPatternType::ReverseAsync);
    }
    DEFINE_TEST_CASE(TestLeaveMultiplayer_6)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveMultiplayer_6);
        TestLeaveMultiplayerHelper(CallingPatternType::ReverseCombination);
    }

    DEFINE_TEST_CASE(TestMultipleLocalUsers_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultipleLocalUsers_1);
        InitializeManager(2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);
        AddMultipleLocalUserHelper(xboxLiveContexts->GetView());

        auto multiplayerManagerInstance = MultiplayerManager::SingletonInstance;
        create_task([multiplayerManagerInstance, xboxLiveContext2]()
        {
            multiplayerManagerInstance->LobbySession->RemoveLocalUser(xboxLiveContext2->User);
        });

        bool userRemoved = false;
        while (!userRemoved)
        {
            auto events = multiplayerManagerInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    // Should not have fired as we still have one more user.
                    VERIFY_IS_TRUE(false);
                }
            }
        }

        VerifyLobby(multiplayerManagerInstance->LobbySession, web::json::value::parse(defaultMultipleLocalUsersLobbyResponse));
        DestructManager(xboxLiveContexts->GetView());
    }

    // Add multiple users while removing a user (on diff threads)
    DEFINE_TEST_CASE(TestMultipleLocalUsers_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultipleLocalUsers_2);
        InitializeManager(3);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");
        auto xboxLiveContext3 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext3->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_3");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);
        AddMultipleLocalUserHelper(xboxLiveContexts->GetView());

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto jsonResponse = web::json::value::parse(multipleLocalUsersLobbyResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        create_task([mpInstance, xboxLiveContext3]()
        {
            mpInstance->LobbySession->AddLocalUser(xboxLiveContext3->User);
            mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext3->User, L"Health", L"89", (Platform::Object^) 1);
            mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext3->User, L"Skill", L"17", (Platform::Object^) 2);
        });

        create_task([mpInstance, xboxLiveContext2]()
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext2->User);
        });

        bool userRemoved = false;
        bool userAdded = false;
        int localUserPropWritten = 0;
        while (!userRemoved || !userAdded || localUserPropWritten != 2)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    userAdded = true;
                }

                if (ev->EventType == MultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    int pContext = safe_cast<int>(ev->Context);
                    VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
                    localUserPropWritten++;
                }

                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    // Should not have fired as we still have one more user.
                    VERIFY_IS_TRUE(false);
                }
            }
        }

        VerifyLobby(mpInstance->LobbySession, web::json::value::parse(multipleLocalUsersLobbyResponse));

        xboxLiveContexts->Append(xboxLiveContext3);
        DestructManager(xboxLiveContexts->GetView());
    }

    // Constantly add/remove users
    DEFINE_TEST_CASE(TestMultipleLocalUsers_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultipleLocalUsers_3);
        InitializeManager(5);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");
        auto xboxLiveContext3 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext3->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_3");
        auto xboxLiveContext4 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext4->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_4");
        auto xboxLiveContext5 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext5->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_5");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);
        AddMultipleLocalUserHelper(xboxLiveContexts->GetView());

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto jsonResponse = web::json::value::parse(multipleLocalUsersLobbyResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        create_task([mpInstance, xboxLiveContext3, xboxLiveContext4]()
        {
            mpInstance->LobbySession->AddLocalUser(xboxLiveContext3->User);
            mpInstance->LobbySession->AddLocalUser(xboxLiveContext4->User);
        });

        create_task([mpInstance, xboxLiveContext2]()
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext2->User);
        });

        create_task([mpInstance, xboxLiveContext5]()
        {
            mpInstance->LobbySession->AddLocalUser(xboxLiveContext5->User);
        });

        int userRemoved = 0, userAdded = 0, eventCount = 0;

        // Since the mocked session only has 2 members, you will get into a scenario where the user was removed before he could have been added,
        // giving you 2 extra added events. However, due to a race between adding & removing (being called on diff threads) this happens only sometimes.
        while (userRemoved != 3 || userAdded < 3)   
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (eventCount == 0)
                {
                    eventCount++;
                    create_task([mpInstance, xboxLiveContext4]()
                    {
                        mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext4->User);
                    });
                }
                else if (eventCount == 1)
                {
                    eventCount++;
                    create_task([mpInstance, xboxLiveContext5]()
                    {
                        mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext5->User);
                    });
                }

                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    userAdded++;
                }

                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved++;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    // Should not have fired as we still have one more user.
                    VERIFY_IS_TRUE(false);
                }
            }
        }

        VerifyLobby(mpInstance->LobbySession, web::json::value::parse(multipleLocalUsersLobbyResponse));

        xboxLiveContexts->Append(xboxLiveContext3);
        xboxLiveContexts->Append(xboxLiveContext4);
        xboxLiveContexts->Append(xboxLiveContext5);
        DestructManager(xboxLiveContexts->GetView());
    }

    void TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType type)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);
        
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto gameSessionResponseJson = defaultGameSessionResponseJson;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, DefaultGameHttpResponse());

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse };
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { lobbySessionEmptyJsonResponse };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        if (type == CallingPatternType::Sync)
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
            catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
        }
        else if (type == CallingPatternType::ReverseSync)
        {
            try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
            catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
        }
        else if (type == CallingPatternType::Async)
        {
            create_task([mpInstance, xboxLiveContext]()
            {
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });

            create_task([mpInstance]()
            {
#pragma warning(suppress: 6387)
                try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
                catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
            });
        }
        else if (type == CallingPatternType::ReverseAsync)
        {
            create_task([mpInstance]()
            {
#pragma warning(suppress: 6387)
                try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
                catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
            });
            create_task([mpInstance, xboxLiveContext]()
            {
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });
        }
        else if (type == CallingPatternType::Combination)
        {
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            create_task([mpInstance]()
            {
#pragma warning(suppress: 6387)
                try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
                catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
            });
        }
        else
        {
#pragma warning(suppress: 6387)
            try { mpInstance->JoinGame(ref new Platform::String(L"MockGameSessionName"), GAME_SESSION_TEMPLATE_NAME, nullptr); }
            catch (Platform::Exception^ ex) { TEST_LOG(L"JoinGame exception thrown."); }
            create_task([mpInstance, xboxLiveContext]()
            {
                Sleep(10);
                mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User);
            });
        }

        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_TRUE(!clientDisconnected);
                    clientDisconnected = true;
                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 0);
        VERIFY_IS_TRUE(mpInstance->LobbySession->Members->Size == 0);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_1)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestJoinGameWhileRemovingLocalUser_1);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Sync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameWhileRemovingLocalUser_2);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Async);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameWhileRemovingLocalUser_3);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Combination);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_4)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameWhileRemovingLocalUser_4);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseSync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_5)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameWhileRemovingLocalUser_5);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseAsync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_6)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestJoinGameWhileRemovingLocalUser_6);
        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseCombination);
    }

    DEFINE_TEST_CASE(TestSubscsriptionLostEvent)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubscsriptionLostEvent);

        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto jsonResponse = web::json::value::parse(emptyJson);
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse);
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        clientManager->on_multiplayer_subscriptions_lost();

        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                TEST_LOG(FormatString(L" [MPM] RemoveLocalUserHelper - Event type: %s", ev->EventType.ToString()->Data()).c_str());
                if (ev->EventType == MultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }

                if (ev->EventType == MultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_TRUE(!clientDisconnected);
                    clientDisconnected = true;

                }
            }
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 0);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
    }

    /*
        multiplayer_session_writer: Back-to-back session writes
        1. Write change #1 followed with chagne # 2 == final result #2
        2. Write change #2 followed with chagne # 1 == final result #2
    */

    void TestBackToBackSessionWriteLogicHelper(std::shared_ptr<HttpResponseStruct> responseStruct, uint64_t maxChangeNumber)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultMpsdUri] = responseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);

        int isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == maxChangeNumber);
        VerifyLobby(mpInstance->LobbySession, lobbyCompletedHandleResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestBackToBackSessionWriteLogic_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBackToBackSessionWriteLogic_1);

        std::shared_ptr<HttpResponseStruct> responseStruct = std::make_shared<HttpResponseStruct>();
        responseStruct->responseList =
        {
            defaultLobbyResponse,               // change #1
            lobbyWithPendingHandleResponse,     // change #2
            lobbyCompletedHandleResponse        // change #3
        };

        TestBackToBackSessionWriteLogicHelper(responseStruct, 3);
    }

    DEFINE_TEST_CASE(TestBackToBackSessionWriteLogic_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBackToBackSessionWriteLogic_2);

        std::shared_ptr<HttpResponseStruct> responseStruct = std::make_shared<HttpResponseStruct>();
        responseStruct->responseList =
        {
            lobbyCompletedHandleResponse,       // change #3
            lobbyWithPendingHandleResponse,     // change #2
            defaultLobbyResponse                // change #1
        };

        TestBackToBackSessionWriteLogicHelper(responseStruct, 3);
    }

    /*
        multiplayer_session_writer: Multiple Taps:
        (fix existing code such that you do only 1 GET for multiple changed events)
        1. call multiple on_session_changed with same change #s (#3, #2, #1, etc); ensure we only do 1 GET
        2. call multiple on_session_changed with updated change #s (#1, #2, #3, etc); ensure we do multiple GETs
    */

    void LogSessionWriterState(const std::shared_ptr<multiplayer_session_writer>& sessionWriter, uint64_t sessionChangeNum, multiplayer_session_change_event_args eventArgs)
    {
        LOGS_DEBUG << "[MPM] LogSessionWriterState: "
            << " Writes in progress: " << sessionWriter->is_write_in_progress()
            << " Sess change #: " << sessionChangeNum
            << " Taps change #: " << sessionWriter->tap_change_number()
            << " Args change #: " << eventArgs.change_number();
    }

    void MultipleTapsHelper(std::shared_ptr<HttpResponseStruct> getResponseStruct, std::vector<uint64_t> tapChangeNumberList, uint64_t maxTapChangeNumber)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::get_current_session] = getResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();

        multiplayer_session_change_event_args eventArgs;
        for (auto changeNumber : tapChangeNumberList)
        {
            eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", changeNumber);
            LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
            sessionWriter->on_session_changed(eventArgs);
        }

        int isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == maxTapChangeNumber);
        VerifyLobby(mpInstance->LobbySession, lobbyCompletedHandleResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestMultipleTaps_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultipleTaps_1);

        // Set up initial http responses
        auto defaultLobbyResponse404 = StockMocks::CreateMockHttpCallResponse(defaultLobbySessionResponseJson, 404, DefaultLobbyHttpResponse());

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            lobbyCompletedHandleResponse,       // change #3
            lobbyWithPendingHandleResponse,     // change #2
            defaultLobbyResponse404             // change #1
        };

        std::vector<uint64_t> tapChangeNUmberList = {3, 2, 1};
        MultipleTapsHelper(getResponseStruct, tapChangeNUmberList, 3);
    }

    DEFINE_TEST_CASE(TestMultipleTaps_2)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestMultipleTaps_2);

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,     // change #2
            lobbyCompletedHandleResponse        // change #3
        };

        std::vector<uint64_t> tapChangeNUmberList = {1, 2, 3};
        MultipleTapsHelper(getResponseStruct, tapChangeNUmberList, 3);
    }

    /*
        multiplayer_session_writer:
        Write Session + Taps (write_session & on_session_changed)
        1. call multiple different writes with shoulder taps in between; ensure that the fianl session # is correct.
        Note: MPM currently does not optimize to ensure that only 1 GET is in flight for multiple shoulder taps.
        2. call write (ch. #6) followed with multiple different writes with shoulder taps; ensure that GET is never called.
        3. call write (ch. #2, #4) followed with shoulder tap (ch. #6); ensure that GET is called.
    */
    void TestMultipleWriteSessionWithTapsHelper(
        std::shared_ptr<HttpResponseStruct> writeResponseStruct,
        std::shared_ptr<HttpResponseStruct> getResponseStruct,
        uint64_t maxChangeNumberToTest,
        string_t handleId = string_t()
        )
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        responses[xbox_live_api::get_current_session] = getResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 2);
        LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
        sessionWriter->on_session_changed(eventArgs);
        pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> asyncOp;
        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        auto eventArgs2 = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 3);
        LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
        sessionWriter->on_session_changed(eventArgs2);
        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        auto eventArgs3 = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 5);
        LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
        sessionWriter->on_session_changed(eventArgs3);
        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        int isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        LOGS_DEBUG_IF(mpInstance->LobbySession->GetCppObj()->_Change_number() != maxChangeNumberToTest)
            << "[MPM] maxChangeNumberToTest: " << maxChangeNumberToTest
            << " curr change #: " << mpInstance->LobbySession->GetCppObj()->_Change_number();

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == maxChangeNumberToTest);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_1)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestWriteSessionWithTaps_1);
        
        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,  // change #2
            sessionChangeNum4Response,       // change #4
            sessionChangeNum6Response        // change #6
        };

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            sessionChangeNum3Response,       // change #3
            sessionChangeNum5Response        // change #5
        };

        TestMultipleWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 6);
        // Test write_session_by_handle
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;

        TestMultipleWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 6, _T("TestHandleId"));
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;
    }

    // The getResponseStruct is used for shoulderTaps.
    void TestWriteSessionWithTapsHelper(
        std::shared_ptr<HttpResponseStruct> writeResponseStruct, 
        std::shared_ptr<HttpResponseStruct> getResponseStruct,
        uint64_t maxChangeNumberForTap,
        uint64_t maxChangeNumberToTest,
        bool waitForFirstWriteToFinish,
        string_t handleId = string_t()
        )
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        responses[xbox_live_api::get_current_session] = getResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> asyncOp;
        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        if (waitForFirstWriteToFinish)
            asyncOp.get();
        else
            asyncOp;
        
        auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 2);
        LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
        sessionWriter->on_session_changed(eventArgs);

        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        auto eventArgs2 = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 3);
        LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
        sessionWriter->on_session_changed(eventArgs2);

        if (handleId.empty())
        {
            asyncOp = sessionWriter->write_session(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing);
        }
        else
        {
            asyncOp = sessionWriter->write_session_by_handle(primaryContext, mpsdLobbySession, multiplayer::multiplayer_session_write_mode::update_existing, handleId);
        }

        concurrency::event fireEvent;
        create_task([this, sessionWriter, mpsdLobbySession, maxChangeNumberForTap, &fireEvent]()
        {
            auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", maxChangeNumberForTap);
            LogSessionWriterState(sessionWriter, mpsdLobbySession->change_number(), eventArgs);
            sessionWriter->on_session_changed(eventArgs);
            fireEvent.set();
        });

        int doWorkCount = 0;
        while (doWorkCount < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            doWorkCount++;
            if (doWorkCount == 5)
            {
                // If it hasn't fired yet, wait for it.
                fireEvent.wait();
            }
        }

        LOGS_DEBUG_IF(mpInstance->LobbySession->GetCppObj()->_Change_number() != maxChangeNumberToTest) 
            << "[MPM] maxChangeNumberToTest: " << maxChangeNumberToTest
            << " curr change #: " << mpInstance->LobbySession->GetCppObj()->_Change_number();

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == maxChangeNumberToTest);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionWithTaps_2);
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            sessionChangeNum6Response,        // change #6
            sessionChangeNum4Response,        // change #4
            lobbyWithPendingHandleResponse    // change #2
        };

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            sessionChangeNum8Response       // change #8
        };

        TestWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 5, 6, true);
        // Test write_session_by_handle
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;

        TestWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 5, 6, true, _T("TestHandleId"));
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionWithTaps_3);
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            sessionChangeNum6Response,        // change #6
            sessionChangeNum4Response,        // change #4
            lobbyWithPendingHandleResponse    // change #2
        };

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            sessionChangeNum8Response       // change #8
        };

        TestWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 8, 8, false);
        // Test write_session_by_handle
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;

        TestWriteSessionWithTapsHelper(writeResponseStruct, getResponseStruct, 8, 8, false, _T("TestHandleId"));
        m_mockXboxSystemFactory->reinit();
        writeResponseStruct->counter = 0;
        getResponseStruct->counter = 0;
    }

    // multiplayer_session_writer:
    // Fire multiple resync events; ensure the session is correctly updated;
    DEFINE_TEST_CASE(TestResync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestResync);
        InitializeManager();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 1);

        std::shared_ptr<HttpResponseStruct> getResponseStruct = std::make_shared<HttpResponseStruct>();
        getResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,     // change #2
            sessionChangeNum3Response,          // change #3
            sessionChangeNum5Response           // change #5
        };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::get_current_session] = getResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        int isDone = 0;
        // Fire multiple resync events every few secs
        while (isDone < 5)
        {
            sessionWriter->on_resync_message_received();
            Sleep(10);
            isDone++;
        }

        // Sleep for a multiplayer_session_writer::TIME_PER_CALL_MS
        Sleep(1000);
        
        isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 3);
        DestructManager(xboxLiveContext);
    }

    // multiplayer_session_writer:
    DEFINE_TEST_CASE(TestSessionWriterLeaveRemoteSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSessionWriterLeaveRemoteSession);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdGameSession = clientManager->latest_pending_read()->game_client()->session();
        auto gameClientSessionWriter = clientManager->latest_pending_read()->game_client()->session_writer();
        auto primaryContext = gameClientSessionWriter->get_primary_context();

        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            sessionChangeNum3Response,          // change #3
            sessionChangeNum5Response,          // change #5
            gameSessionEmptyJsonResponse
        };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        gameClientSessionWriter->leave_remote_session(mpsdGameSession).get();
        int isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        // Since updateLatest is false, this should not update the latest session.
        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);
        DestructManager(xboxLiveContext, true);
    }

    void TestDeepCopyIfUpdated_1(bool isLobbyTest)
    {
        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        if (isLobbyTest)
        {
            writeResponseStruct->responseList =
            {
                lobbyWithPendingHandleResponse,  // change #2
                sessionChangeNum4Response,       // change #4
                sessionChangeNum6Response        // change #6
            };
        }
        else
        {
            writeResponseStruct->responseList =
            {
                sessionChangeNum2Response,      // change #2
                sessionChangeNum4Response,      // change #4
                sessionChangeNum6Response       // change #6
            };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        std::shared_ptr<multiplayer_session> mpsdSession;
        std::shared_ptr<multiplayer_session_writer> sessionWriter;
        std::shared_ptr<xbox_live_context_impl> primaryContext;

        if (isLobbyTest)
        {
            mpsdSession = clientManager->latest_pending_read()->lobby_client()->session();
            sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }
        else
        {
            mpsdSession = clientManager->latest_pending_read()->game_client()->session();
            sessionWriter = clientManager->latest_pending_read()->game_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());

        // Because I have not called DoWork() yet, it should not update the MM instances.
        if (isLobbyTest)
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 1);
        else
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        // Test deep_copy_if_updated
        clientManager->last_pending_read()->deep_copy_if_updated(*clientManager->latest_pending_read());
        if (isLobbyTest)
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->lobby_client()->lobby()->_Change_number() == 1);     // no DoWork() so the actual lobby obj is still stale
            VERIFY_IS_TRUE(sessionWriter->session()->change_number() == 2);
        }
        else
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->game_client()->game()->_Change_number() == 1);     // no DoWork() so the actual lobby obj is still stale
            VERIFY_IS_TRUE(sessionWriter->session()->change_number() == 2);
        }

        VERIFY_IS_TRUE(clientManager->is_update_avaialable());

        // Because I have not called DoWork() yet, it should not update the MM instances.
        if (isLobbyTest)
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 1);
        else
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        mpInstance->DoWork();
        if (isLobbyTest)
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->lobby_client()->lobby()->_Change_number() == 4);     // called DoWork(), should update
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 4);
        }
        else
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->game_client()->game()->_Change_number() == 4);       // called DoWork(), should update
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 4);
        }
    }

    void TestDeepCopyIfUpdated_2(bool isLobbyTest)
    {
        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        if (isLobbyTest)
        {
            writeResponseStruct->responseList =
            {
                lobbyWithPendingHandleResponse,  // change #2
                sessionChangeNum4Response,       // change #4
                sessionChangeNum6Response        // change #6
            };
        }
        else
        {
            writeResponseStruct->responseList =
            {
                sessionChangeNum2Response,      // change #2
                sessionChangeNum4Response,      // change #4
                sessionChangeNum6Response       // change #6
            };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        std::shared_ptr<multiplayer_session> mpsdSession;
        std::shared_ptr<multiplayer_session_writer> sessionWriter;
        std::shared_ptr<xbox_live_context_impl> primaryContext;

        if (isLobbyTest)
        {
            mpsdSession = clientManager->latest_pending_read()->lobby_client()->session();
            sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }
        else
        {
            mpsdSession = clientManager->latest_pending_read()->game_client()->session();
            sessionWriter = clientManager->latest_pending_read()->game_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }

        for (uint32_t count = 1; count <= 3; ++count)
        {
            sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
            VERIFY_IS_TRUE(clientManager->is_update_avaialable());
            mpInstance->DoWork();
            if (isLobbyTest)
                VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == (count * 2));
            else
                VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == (count * 2));

            VERIFY_IS_TRUE(!clientManager->is_update_avaialable());
        }

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(!clientManager->is_update_avaialable());
    }

    void TestDeepCopyIfUpdated_3(bool isLobbyTest)
    {
        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        if (isLobbyTest)
        {
            writeResponseStruct->responseList =
            {
                lobbyWithPendingHandleResponse,  // change #2
                sessionChangeNum4Response,       // change #4
                sessionChangeNum6Response,       // change #6
                sessionChangeNum6Response        // change #6
            };
        }
        else
        {
            writeResponseStruct->responseList =
            {
                sessionChangeNum2Response,      // change #2
                sessionChangeNum4Response,      // change #4
                sessionChangeNum6Response,      // change #6
                sessionChangeNum6Response       // change #6
            };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        std::shared_ptr<multiplayer_session> mpsdSession;
        std::shared_ptr<multiplayer_session_writer> sessionWriter;
        std::shared_ptr<xbox_live_context_impl> primaryContext;

        if (isLobbyTest)
        {
            mpsdSession = clientManager->latest_pending_read()->lobby_client()->session();
            sessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }
        else
        {
            mpsdSession = clientManager->latest_pending_read()->game_client()->session();
            sessionWriter = clientManager->latest_pending_read()->game_client()->session_writer();
            primaryContext = sessionWriter->get_primary_context();
        }

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        mpInstance->DoWork();
        if (isLobbyTest)
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->lobby_client()->lobby()->_Change_number() == 4);
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 4);
        }
        else
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->game_client()->game()->_Change_number() == 4);
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 4);
        }

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        clientManager->last_pending_read()->deep_copy_if_updated(*clientManager->latest_pending_read());
        if (isLobbyTest)
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->lobby_client()->lobby()->_Change_number() == 4);     // no DoWork() so the actual lobby obj is still stale
            VERIFY_IS_TRUE(sessionWriter->session()->change_number() == 6);
        }
        else
        {
            VERIFY_IS_TRUE(clientManager->last_pending_read()->game_client()->game()->_Change_number() == 4);     // no DoWork() so the actual lobby obj is still stale
            VERIFY_IS_TRUE(sessionWriter->session()->change_number() == 6);
        }
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());

        // Even though I manually deep copied the objects, this will update the MPM session objects as is_update_avaialable() will return true.
        mpInstance->DoWork();
        if (isLobbyTest)
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 6);
        else
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 6);

        sessionWriter->write_session(primaryContext, mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        VERIFY_IS_TRUE(!clientManager->is_update_avaialable());     // should not update as the session change # was the same
        mpInstance->DoWork();
        if (isLobbyTest)
            VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == 6);
        else
            VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 6);
    }

    void TestUpdateSession(bool isLobbyTest)
    {
        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        if (isLobbyTest)
        {
            writeResponseStruct->responseList =
            {
                lobbyWithPendingHandleResponse,  // change #2
                sessionChangeNum4Response,       // change #4
                sessionChangeNum6Response        // change #6
            };
        }
        else
        {
            writeResponseStruct->responseList =
            {
                sessionChangeNum2Response,      // change #2
                sessionChangeNum4Response,      // change #4
                sessionChangeNum6Response       // change #6
            };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        std::shared_ptr<xbox_live_context_impl> primaryContext;

        if (isLobbyTest)
            primaryContext = clientManager->latest_pending_read()->lobby_client()->session_writer()->get_primary_context();
        else
            primaryContext = clientManager->latest_pending_read()->game_client()->session_writer()->get_primary_context();

        auto mpsdSession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));

        for (uint32_t count = 1; count <= 3; ++count)
        {
            auto result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
            if (isLobbyTest)
                clientManager->latest_pending_read()->lobby_client()->update_session(result.payload());
            else
                clientManager->latest_pending_read()->game_client()->update_session(result.payload());
            VERIFY_IS_TRUE(clientManager->is_update_avaialable());
            mpInstance->DoWork();
            if (isLobbyTest)
                VERIFY_IS_TRUE(mpInstance->LobbySession->GetCppObj()->_Change_number() == (count * 2));
            else
                VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == (count * 2));

            VERIFY_IS_TRUE(!clientManager->is_update_avaialable());
        }
    }

    /*
        multiplayer_lobby_client:
    */
    DEFINE_TEST_CASE(TestLobbyDeepCopyIfUpdated_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLobbyDeepCopyIfUpdated_1);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_1(true);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestLobbyDeepCopyIfUpdated_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLobbyDeepCopyIfUpdated_2);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_2(true);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestLobbyDeepCopyIfUpdated_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLobbyDeepCopyIfUpdated_3);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_3(true);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestLobbyUpdateSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLobbyUpdateSession);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);
        TestUpdateSession(true);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestPendingLobbyChanges)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPendingLobbyChanges);
        InitializeManager(2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");
        AddLocalUserHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto sessionWriter = lobbyClient->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        mpInstance->LobbySession->AddLocalUser(xboxLiveContext2->User);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->LobbySession->RemoveLocalUser(primaryContext->user());
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->LobbySession->SetLocalMemberProperties(primaryContext->user(), L"Health", L"89", (Platform::Object^) 1);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->LobbySession->SetLocalMemberConnectionAddress(primaryContext->user(), L"TestConnectionAddress", (Platform::Object^) 1);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->LobbySession->DeleteLocalMemberProperties(primaryContext->user(), L"Health", (Platform::Object^) 1);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->LobbySession->SetSynchronizedHost(mpInstance->LobbySession->LocalMembers->GetAt(0), (Platform::Object^) 1);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        mpInstance->SetJoinInProgress(Joinability::JoinableByFriends);
        VERIFY_IS_TRUE(lobbyClient->is_pending_lobby_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        lobbyClient->clear_pending_queue();

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);
        DestructManager(xboxLiveContexts->GetView());
    }

    DEFINE_TEST_CASE(TestAdvertiseGameSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAdvertiseGameSession);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        auto gameSessionResponseJson = defaultGameSessionResponseJson;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, DefaultGameHttpResponse());
        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            gameSessionResponse,                // to create the game session below
            lobbyNoHandleResponse,              // advertise_game_session -> set_transfer_handle
            lobbyCompletedHandleResponse        // advertise_game_session -> write_session (transfer handle)
        };

        // set up http response set
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto primaryContext = clientManager->latest_pending_read()->game_client()->session_writer()->get_primary_context();

        auto mpsdSession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));
        auto result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->game_client()->update_session(result.payload());
        mpInstance->DoWork();
        VERIFY_IS_TRUE(mpInstance->GameSession != nullptr);

        clientManager->latest_pending_read()->lobby_client()->advertise_game_session();

        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool isAdvertisingGameDone = false;
        while (!isAdvertisingGameDone )
        {
            auto events = MultiplayerManager::SingletonInstance->DoWork();
            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, lobbyCompletedHandleResponseJson);
        VerifyGame(mpInstance->GameSession, gameSessionResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestStopAdvertisingGameSession)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestStopAdvertisingGameSession);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        auto gameSessionResponseJson = defaultGameSessionResponseJson;
        auto gameSessionResponse = StockMocks::CreateMockHttpCallResponse(gameSessionResponseJson, DefaultGameHttpResponse());
        auto tgameSessionResponseDiffXuid = StockMocks::CreateMockHttpCallResponse(gameSessionResponseDiffXuidJson, DefaultGameHttpResponse());
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            gameSessionResponse,                // to create the game session below
            tgameSessionResponseDiffXuid,        // to create the game session below
            updatedLobbyNoHandleResponse        // clear_game_session_from_lobby
        };

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionEmptyJsonResponse };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto primaryContext = clientManager->latest_pending_read()->game_client()->session_writer()->get_primary_context();
        auto mpsdSession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));
        
        
        // Should not clear the game session as you are not the last person to leave the session.
        auto result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->lobby_client()->stop_advertising_game_session(result);
        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        VERIFY_IS_TRUE(utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0);

        // Should clear the game session as no lobby members exist in the game.
        result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->lobby_client()->stop_advertising_game_session(result);
        propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        customPropertyJson = propertyJson[L"properties"];
        bool isStopAdvertisingGameDone = false;
        while (!isStopAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, updatedLobbyNoHandleResponseJson);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestRemoveStaleXboxLiveContextFromMap)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestRemoveStaleXboxLiveContextFromMap);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionEmptyJsonResponse };
        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { lobbySessionEmptyJsonResponse };

        // set up http response set
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();

        auto xboxLiveContextMap = lobbyClient->get_local_user_map();
        for (auto xboxLiveContext2 : xboxLiveContextMap)
        {
            auto localUser = xboxLiveContext2.second;
            if (localUser != nullptr)
            {
                localUser->set_lobby_state(multiplayer_local_user_lobby_state::remove);
            }
        }
        lobbyClient->remove_stale_xbox_live_context_from_map();
        VERIFY_IS_TRUE(lobbyClient->get_local_user_map().size() == 0);
        VERIFY_IS_TRUE(lobbyClient->get_primary_context() == nullptr);
        DestructManager(xboxLiveContext);

        // Since you are accessing low level funs directly, is_update_avaialable returns false and the local objects fail updating.
        // No need to check GameSession/LobbySession == nullptr
    }

    DEFINE_TEST_CASE(TestTransferHandleState)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestTransferHandleState);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> writeResponseStruct = std::make_shared<HttpResponseStruct>();
        writeResponseStruct->responseList =
        {
            lobbyWithPendingHandleResponse,     // change #2
            lobbyCompletedHandleResponse,       // change #3
            updatedLobbyNoHandleResponse        // clear_game_session_from_lobby
        };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> responses;
        responses[xbox_live_api::write_session_using_subpath] = writeResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto primaryContext = lobbyClient->session_writer()->get_primary_context();
        auto mpsdSession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));

        VERIFY_IS_TRUE(lobbyClient->is_transfer_handle_state(_T("completed")));
        VERIFY_IS_TRUE(utils::str_icmp(lobbyClient->get_transfer_handle(), _T("TestGameSessionTransferHandle")) == 0);

        auto result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->lobby_client()->update_session(result.payload());
        VERIFY_IS_TRUE(lobbyClient->is_transfer_handle_state(_T("pending")));
        VERIFY_IS_TRUE(utils::str_icmp(lobbyClient->get_transfer_handle(), _T("TestXboxUserId")) == 0);

        result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->lobby_client()->update_session(result.payload());
        VERIFY_IS_TRUE(lobbyClient->is_transfer_handle_state(_T("completed")));
        VERIFY_IS_TRUE(utils::str_icmp(lobbyClient->get_transfer_handle(), _T("TestGameSessionTransferHandle")) == 0);

        result = primaryContext->multiplayer_service().write_session(mpsdSession, multiplayer::multiplayer_session_write_mode::update_existing).get();
        clientManager->latest_pending_read()->lobby_client()->update_session(result.payload());
        VERIFY_IS_TRUE(!lobbyClient->is_transfer_handle_state(_T("completed")));
        VERIFY_IS_TRUE(!lobbyClient->is_transfer_handle_state(_T("pending")));
        VERIFY_IS_TRUE(utils::str_icmp(lobbyClient->get_transfer_handle(), string_t()) == 0);
        DestructManager(xboxLiveContext);
    }

    /*
        multiplayer_game_client:
    */
    DEFINE_TEST_CASE(TestGameDeepCopyIfUpdated_1)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestGameDeepCopyIfUpdated_1);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_1(false);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestGameDeepCopyIfUpdated_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGameDeepCopyIfUpdated_2);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_2(false);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestGameDeepCopyIfUpdated_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGameDeepCopyIfUpdated_3);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        TestDeepCopyIfUpdated_3(false);
        DestructManager(xboxLiveContext, true);
    }

    DEFINE_TEST_CASE(TestGameUpdateSession)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(TestGameUpdateSession);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);
        TestUpdateSession(false);
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestPendingGameChanges)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPendingGameChanges);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto gameClient = clientManager->latest_pending_read()->game_client();
        auto sessionWriter = gameClient->session_writer();
        auto primaryContext = sessionWriter->get_primary_context();

        mpInstance->GameSession->SetProperties(L"Map", L"1", (Platform::Object^) 1);
        VERIFY_IS_TRUE(gameClient->is_pending_game_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        gameClient->clear_pending_queue();

        mpInstance->GameSession->SetSynchronizedProperties(L"Map", L"2", (Platform::Object^) 1);
        VERIFY_IS_TRUE(gameClient->is_pending_game_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        gameClient->clear_pending_queue();

        mpInstance->GameSession->SetSynchronizedHost(mpInstance->LobbySession->LocalMembers->GetAt(0), (Platform::Object^) 1);
        VERIFY_IS_TRUE(gameClient->is_pending_game_changes());
        VERIFY_IS_TRUE(clientManager->is_update_avaialable());
        gameClient->clear_pending_queue();
        DestructManager(xboxLiveContext, true);
    }

    void TestLeaveRemoteSessionWithEmptyGameSession(std::shared_ptr<http_call_response> gameSessionResponse, bool removeStaleUsers)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto gameClient = clientManager->latest_pending_read()->game_client();
        auto mpsdGameSession = gameClient->session();
        auto gameClientSessionWriter = gameClient->session_writer();
        auto primaryContext = gameClientSessionWriter->get_primary_context();

        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList =
        {
            updatedLobbyNoHandleResponse        // clear_game_session_from_lobby
        };

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        if (removeStaleUsers)
        {
            auto xboxLiveContextMap = lobbyClient->get_local_user_map();
            for (auto xboxLiveContext2 : xboxLiveContextMap)
            {
                auto localUser = xboxLiveContext2.second;
                if (localUser != nullptr)
                {
                    localUser->set_lobby_state(multiplayer_local_user_lobby_state::remove);
                }
            }

            gameClient->remove_stale_users_from_remote_session();
        }
        else
        {
            gameClient->leave_remote_session(mpsdGameSession, true, true);
        }

        bool isStopAdvertisingGameDone = false;
        auto propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        auto customPropertyJson = propertyJson[L"properties"];
        while (!isStopAdvertisingGameDone)
        {
            mpInstance->DoWork();
            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VerifyLobby(mpInstance->LobbySession, updatedLobbyNoHandleResponseJson);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        DestructManager(xboxLiveContext);
    }

    void RemoveStaleUsersFromRemoteSession(
        std::shared_ptr<http_call_response> gameSessionResponse,
        web::json::value gameSessionResultToVerify
        )
    {
        InitializeManager(2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts->GetView());

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto gameClient = clientManager->latest_pending_read()->game_client();
        auto mpsdGameSession = gameClient->session();
        auto gameClientSessionWriter = gameClient->session_writer();
        auto primaryContext = gameClientSessionWriter->get_primary_context();

        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionResponse };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { updatedLobbyNoHandleResponse };

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto xboxLiveContextMap = lobbyClient->get_local_user_map();
        for (auto xboxLiveContext3 : xboxLiveContextMap)
        {
            auto localUser = xboxLiveContext3.second;
            if (localUser != nullptr)
            {
                localUser->set_lobby_state(multiplayer_local_user_lobby_state::remove);
                break;
            }
        }

        gameClient->remove_stale_users_from_remote_session();

        int isDone = 0;
        while (isDone < 10)
        {
            mpInstance->DoWork();
            Sleep(10);
            isDone++;
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 2);
        VerifyLobby(mpInstance->LobbySession, defaultMultipleLocalUsersLobbyResponseJson);
        VERIFY_IS_TRUE(mpInstance->GameSession->Members->Size == 1);
        VerifyGame(mpInstance->GameSession, gameSessionResultToVerify);
        DestructManager(xboxLiveContexts->GetView());
    }

    void LeaveRemoteSession()
    {
        InitializeManager(2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        xboxLiveContext2->User->_User_impl()->_Set_xbox_user_id("TestXboxUserId_2");

        Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>^ xboxLiveContexts = ref new Platform::Collections::Vector<Microsoft::Xbox::Services::XboxLiveContext^>();
        xboxLiveContexts->Append(xboxLiveContext);
        xboxLiveContexts->Append(xboxLiveContext2);

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts->GetView());

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto gameClient = clientManager->latest_pending_read()->game_client();
        auto mpsdGameSession = gameClient->session();
        auto gameClientSessionWriter = gameClient->session_writer();
        auto primaryContext = gameClientSessionWriter->get_primary_context();

        VERIFY_IS_TRUE(mpInstance->GameSession->GetCppObj()->_Change_number() == 1);

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { gameSessionEmptyJsonResponse };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = 
        { 
            updatedMultipleLocalUsersLobbyNoHandleResponse  // clear_game_session_from_lobby
        };

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        gameClient->leave_remote_session(mpsdGameSession, true, true);

        auto propertyJson = web::json::value::parse(propertiesNoTransferHandleJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool leaveGameCompleted = false, isStopAdvertisingGameDone = false;
        while (!leaveGameCompleted || !isStopAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::LeaveGameCompleted)
                {
                    leaveGameCompleted = true;
                }
            }

            if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }
        }

        VERIFY_IS_TRUE(mpInstance->LobbySession->LocalMembers->Size == 2);
        VerifyLobby(mpInstance->LobbySession, updatedMultipleLocalUsersLobbyWithNoTransferHandleResponseJson);
        VERIFY_IS_TRUE(mpInstance->GameSession == nullptr);
        DestructManager(xboxLiveContexts->GetView());
    }

    DEFINE_TEST_CASE(TestLeaveRemoteSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLeaveRemoteSession);
        LeaveRemoteSession();
    }

    DEFINE_TEST_CASE(TestRemoveStaleUsersFromRemoteSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRemoveStaleUsersFromRemoteSession);
        RemoveStaleUsersFromRemoteSession(gameSessionDiffXuidResponse, gameSessionResponseDiffXuidJson);
    }

    /*
        multiplayer_client_pending_reader:
    */

    DEFINE_TEST_CASE(TestClientPendingReaderHelperMethods)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestClientPendingReaderHelperMethods);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        JoinGameHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto lobbyClient = clientManager->latest_pending_read()->lobby_client();
        auto gameClient = clientManager->latest_pending_read()->game_client();

        VERIFY_IS_TRUE(clientManager->latest_pending_read()->is_lobby(lobbyClient->session()->session_reference()));
        VERIFY_IS_TRUE(!clientManager->latest_pending_read()->is_lobby(multiplayer_session_reference()));
        VERIFY_IS_TRUE(clientManager->latest_pending_read()->is_game(gameClient->session()->session_reference()));
        VERIFY_IS_TRUE(!clientManager->latest_pending_read()->is_game(multiplayer_session_reference()));
        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestErrorHandling)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestErrorHandling);
        InitializeManager();
        auto mpInstance = MultiplayerManager::SingletonInstance;

        VERIFY_NO_THROW(
            mpInstance->DoWork()
            );

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->AddLocalUser(nullptr),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->RemoveLocalUser(nullptr),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->RemoveLocalUser(xboxLiveContext->User),
            E_UNEXPECTED
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetLocalMemberProperties(nullptr, L"Health", L"89", (Platform::Object^) 1),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetLocalMemberProperties(xboxLiveContext->User, L"", L"89", (Platform::Object^) 2),
            E_UNEXPECTED
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->DeleteLocalMemberProperties(nullptr, L"Health", (Platform::Object^) 1),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->DeleteLocalMemberProperties(xboxLiveContext->User, L"", (Platform::Object^) 1),
            E_UNEXPECTED
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetLocalMemberConnectionAddress(nullptr, L"AQAI1Fy6", (Platform::Object^) 1),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetLocalMemberConnectionAddress(xboxLiveContext->User, L"AQAI1Fy6", (Platform::Object^) 1),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetProperties(L"", L"89", (Platform::Object^) 1),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetSynchronizedProperties(L"", L"89", (Platform::Object^) 1),
            E_UNEXPECTED
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->SetSynchronizedHost(nullptr, (Platform::Object^) 1),
            E_INVALIDARG
            );

#pragma warning(suppress: 4973)
        VERIFY_THROWS_HR_CX(
            mpInstance->JoinGame(ref new Platform::String(L""), GAME_SESSION_TEMPLATE_NAME),
            E_INVALIDARG
            );

#pragma warning(suppress: 4973)
        VERIFY_THROWS_HR_CX(
            mpInstance->JoinGame(ref new Platform::String(L"TestSessionName"), GAME_SESSION_TEMPLATE_NAME),
            E_UNEXPECTED
            );

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        const string_t badResponse = testResponseJsonFromFile[L"badResponse"].serialize();
        auto jsonResponse = web::json::value::parse(badResponse);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(jsonResponse, 400);
        mpInstance->LobbySession->AddLocalUser(xboxLiveContext->User);

        VERIFY_THROWS_HR_CX(
            mpInstance->LobbySession->AddLocalUser(xboxLiveContext->User),
            E_UNEXPECTED
            );  // User already added

        bool isDone = false;
        while (!isDone)
        {
            auto events = mpInstance->DoWork();
            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::UserAdded)
                {
                    VERIFY_IS_TRUE(ev->ErrorCode == HTTP_E_STATUS_BAD_REQUEST);
                    isDone = true;
                }
            }
        }

        DestructManager(xboxLiveContext, true);
    }

    /*
        Matchmaking Tests:
    */

    enum class MatchCallingPatternType
    {
        Completed,
        Canceled,
        CanceledByService,
        RemoteClientFailedToJoin,       // initialization stage = failed as the remote client failed to join the target session
        ExpiredByNextTimer,             // Didn't get any shoulder taps after searching.
        ExpiredByService,               // Service could not find anything to match with.
        RemoteClientFailedToUploadQoS   // initialization stage = failed as the remote client failed to upload qos data
    };

    void FindMatchNoQoSHelper(MatchCallingPatternType matchCallingPattern)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> matchTicketResponseStruct = std::make_shared<HttpResponseStruct>();
        matchTicketResponseStruct->responseList = { matchTicketResponse };

        std::shared_ptr<HttpResponseStruct> transferHandleResponseStruct = std::make_shared<HttpResponseStruct>();
        transferHandleResponseStruct->responseList = { transferHandleResponse };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        if (matchCallingPattern == MatchCallingPatternType::Completed)
        {
            gameResponseStruct->responseList = { matchJoin_1_Response, matchJoin_2_Response };
        }
        else if (matchCallingPattern == MatchCallingPatternType::RemoteClientFailedToJoin)
        {
            gameResponseStruct->responseList = { matchJoin_1_Response, matchRemoteClientFailedToJoin_Response };
        }
        else
        {
            // Forcing the timer to expire, and then do a get
            gameResponseStruct->responseList = { matchJoin_1_Response };
        }

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        if (matchCallingPattern == MatchCallingPatternType::ExpiredByNextTimer || matchCallingPattern == MatchCallingPatternType::ExpiredByService)
        {
            lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusExpiredByServiceResponse };
        }
        else
        {
            lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusFoundResponse, matchStatusFoundWithTransHandleResponse };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> matchResponses;
        matchResponses[xbox_live_api::create_match_ticket] = matchTicketResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(matchResponses);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> transferHandleResponses;
        transferHandleResponses[xbox_live_api::set_transfer_handle] = transferHandleResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(transferHandleResponses, false);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto lobbySessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();

        auto timeSpan = Windows::Foundation::TimeSpan();
        timeSpan.Duration = 1 * TICKS_PER_SECOND;
        mpInstance->FindMatch(HOPPER_NAME_NO_QOS, ref new Platform::String(), timeSpan);

        // Match ticket response
        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // Create Transfer handle
        // LB: PUT Transfer handle

        clientManager->match_client()->disable_next_timer(true); // force the timer to avoid getting into expiry state.

        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool matchFound = false, isAdvertisingGameDone = false, searchingTapTriggered = false, foundTapTriggered = false, waitingForClientsTapTriggered = false;
        while (!matchFound || !isAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            if (mpInstance->MatchStatus == MatchStatus::Searching && !searchingTapTriggered)
            {
                searchingTapTriggered = true;
                auto matchTicketeJson = web::json::value::parse(matchTicket);
                VERIFY_ARE_EQUAL_TIMESPAN_TO_SECONDS( mpInstance->EstimatedMatchWaitTime, matchTicketeJson[_T("waitTime")].as_number().to_uint64() );

                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 4);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!foundTapTriggered && mpInstance->LobbySession->GetCppObj()->_Change_number() == 4)
            {
                foundTapTriggered = true;
                if (matchCallingPattern == MatchCallingPatternType::ExpiredByNextTimer)
                {
                    TEST_LOG(L"FindMatchNoQoSHelper - setting next timer to false.");
                    clientManager->match_client()->disable_next_timer(false);
                }
                else
                {
                    // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                    auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 6);
                    lobbySessionWriter->on_session_changed(eventArgs);
                }
            }

            if (!waitingForClientsTapTriggered && mpInstance->MatchStatus == MatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;
                if (matchCallingPattern == MatchCallingPatternType::RemoteClientFailedToJoin)
                {
                    clientManager->match_client()->disable_next_timer(false);
                }
                else if (matchCallingPattern == MatchCallingPatternType::Completed)
                {
                    // Force a tap to simulate 2nd user joining.
                    auto eventArgs = multiplayer_session_change_event_args(clientManager->match_client()->session()->session_reference(), L"", 3);
                    clientManager->on_session_changed(eventArgs);
                }
            }

            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    auto findMatchCompleted = static_cast<FindMatchCompletedEventArgs^>(ev->EventArgs);
                    LOGS_DEBUG << " [MPM] MatchStatus: " << findMatchCompleted->MatchStatus.ToString()->Data();

                    if (matchCallingPattern == MatchCallingPatternType::Completed)
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Completed);
                    else if (matchCallingPattern == MatchCallingPatternType::RemoteClientFailedToJoin)
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Failed);
                    else if (matchCallingPattern == MatchCallingPatternType::ExpiredByNextTimer)
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Expired);
                    else if (matchCallingPattern == MatchCallingPatternType::ExpiredByService)
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Expired);
                }
            }

            if (matchFound)
            {
                if (matchCallingPattern == MatchCallingPatternType::Completed)
                {
                    if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
                    {
                        isAdvertisingGameDone = true;
                    }
                }
                else
                {
                    isAdvertisingGameDone = true;
                }
            }
        }

        DestructManager(xboxLiveContext);
    }

    void FindMatchWithQoSHelper(MatchCallingPatternType callingPattern)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> matchTicketResponseStruct = std::make_shared<HttpResponseStruct>();
        matchTicketResponseStruct->responseList = { matchTicketResponse };

        std::shared_ptr<HttpResponseStruct> transferHandleResponseStruct = std::make_shared<HttpResponseStruct>();
        transferHandleResponseStruct->responseList = { transferHandleResponse };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
        {
            gameResponseStruct->responseList = { matchJoin_1_Response, matchMeasuringResponse, matchMeasuringWithQoSResponse, matchRemoteClientFailedToUploadQoSResponse };
        }
        else if(callingPattern == MatchCallingPatternType::Completed)
        {
            gameResponseStruct->responseList = { matchJoin_1_Response, matchMeasuringResponse, matchMeasuringWithQoSResponse, matchMeasuringWithQoSCompleteResponse };
        }

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusFoundResponse, matchStatusFoundWithTransHandleResponse };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> matchResponses;
        matchResponses[xbox_live_api::create_match_ticket] = matchTicketResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(matchResponses);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> transferHandleResponses;
        transferHandleResponses[xbox_live_api::set_transfer_handle] = transferHandleResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(transferHandleResponses, false);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto lobbySessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();

        auto timeSpan = Windows::Foundation::TimeSpan();
        timeSpan.Duration = 1 * TICKS_PER_SECOND;
        mpInstance->FindMatch(HOPPER_NAME_NO_QOS, ref new Platform::String(), timeSpan);

        // Match ticket response
        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // GS: Trigger set qos
        // title: set qos metrics
        // match completed
        // Create Transfer handle
        // LB: PUT Transfer handle

        clientManager->match_client()->disable_next_timer(true); // force the timer to avoid getting into expiry state.

        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool matchFound = false, isAdvertisingGameDone = false, searchingTapTriggered = false, foundTapTriggered = false, waitingForClientsToJoinTapTriggered = false, waitingForClientsToUploadQoSTapTriggered = false;
        while (!matchFound || !isAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            if (mpInstance->MatchStatus == MatchStatus::Searching && !searchingTapTriggered)
            {
                searchingTapTriggered = true;
                auto matchTicketeJson = web::json::value::parse(matchTicket);
                VERIFY_ARE_EQUAL_TIMESPAN_TO_SECONDS(mpInstance->EstimatedMatchWaitTime, matchTicketeJson[_T("waitTime")].as_number().to_uint64());

                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 4);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!foundTapTriggered && mpInstance->LobbySession->GetCppObj()->_Change_number() == 4)
            {
                foundTapTriggered = true;

                // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 6);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (mpInstance->MatchStatus == MatchStatus::WaitingForRemoteClientsToJoin && !waitingForClientsToJoinTapTriggered)
            {
                waitingForClientsToJoinTapTriggered = true;

                // Force a tap to simulate 2nd user joining.
                auto eventArgs = multiplayer_session_change_event_args(clientManager->match_client()->session()->session_reference(), L"", 3);
                clientManager->on_session_changed(eventArgs);
            }

            if (mpInstance->MatchStatus == MatchStatus::WaitingForRemoteClientsToUploadQos && !waitingForClientsToUploadQoSTapTriggered)
            {
                waitingForClientsToUploadQoSTapTriggered = true;
                if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
                {
                    clientManager->match_client()->disable_next_timer(false);
                }
                else
                {
                    // Force a tap to simulate 2nd user uploading QoS.
                    auto eventArgs = multiplayer_session_change_event_args(clientManager->match_client()->session()->session_reference(), L"", 6);
                    clientManager->on_session_changed(eventArgs);
                }
            }

            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::PerformQosMeasurements)
                {
                    auto measurments = ref new Vector<MultiplayerQualityOfServiceMeasurements^>();
                    mpInstance->SetQualityOfServiceMeasurements(measurments->GetView());
                }

                if (ev->EventType == MultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    auto findMatchCompleted = static_cast<FindMatchCompletedEventArgs^>(ev->EventArgs);
                    LOGS_DEBUG << " [MPM] MatchStatus: " << findMatchCompleted->MatchStatus.ToString()->Data();

                    if (callingPattern == MatchCallingPatternType::Completed)
                    {
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Completed);
                    }
                    else if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
                    {
                        VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Failed);
                    }
                }
            }

            if (matchFound)
            {
                if (callingPattern == MatchCallingPatternType::Completed)
                {
                    if (utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
                    {
                        isAdvertisingGameDone = true;
                    }
                }
                else
                {
                    isAdvertisingGameDone = true;
                }
            }
        }

        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSCompleted)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchNoQoSCompleted);
        FindMatchNoQoSHelper(MatchCallingPatternType::Completed);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSRemoteClientFailedToJoin)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchNoQoSRemoteClientFailedToJoin);
        FindMatchNoQoSHelper(MatchCallingPatternType::RemoteClientFailedToJoin);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSExpiredByNextTimer)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestFindMatchNoQoSExpiredByNextTimer);
        FindMatchNoQoSHelper(MatchCallingPatternType::ExpiredByNextTimer);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSExpiredByService)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchNoQoSExpiredByService);
        FindMatchNoQoSHelper(MatchCallingPatternType::ExpiredByService);
    }

    DEFINE_TEST_CASE(TestFindMatchWithQoSCompleted)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchWithQoSCompleted);
        FindMatchWithQoSHelper(MatchCallingPatternType::Completed);
    }

    DEFINE_TEST_CASE(TestFindMatchWithQoSRemoteClientFailedToUploadQoS)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchWithQoSRemoteClientFailedToUploadQoS);
        FindMatchWithQoSHelper(MatchCallingPatternType::RemoteClientFailedToUploadQoS);
    }

    void FindMatchNoQoSRemoteClientJoiningMatchSessionHelper()
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> matchTicketResponseStruct = std::make_shared<HttpResponseStruct>();
        matchTicketResponseStruct->responseList = { matchTicketResponse };

        std::shared_ptr<HttpResponseStruct> transferHandleResponseStruct = std::make_shared<HttpResponseStruct>();
        transferHandleResponseStruct->responseList = { transferHandleResponse };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { matchJoin_1_Response, matchJoin_2_Response };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusFoundResponse, matchStatusFoundWithTransHandleResponse };

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> matchResponses;
        matchResponses[xbox_live_api::create_match_ticket] = matchTicketResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(matchResponses);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> transferHandleResponses;
        transferHandleResponses[xbox_live_api::set_transfer_handle] = transferHandleResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(transferHandleResponses, false);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto lobbySessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();

        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // Create Transfer handle
        // LB: PUT Transfer handle

        auto propertyJson = web::json::value::parse(classPropertiesJson);
        auto customPropertyJson = propertyJson[L"properties"];
        bool matchFound = false, isAdvertisingGameDone = false, searchingTapTriggered = false, foundTapTriggered = false, waitingForClientsTapTriggered = false;
        while (!matchFound || !isAdvertisingGameDone)
        {
            auto events = mpInstance->DoWork();
            if (!searchingTapTriggered)
            {
                searchingTapTriggered = true;
                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 4);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!foundTapTriggered && mpInstance->LobbySession->GetCppObj()->_Change_number() == 4)
            {
                foundTapTriggered = true;
                clientManager->match_client()->disable_next_timer(true); // force the timer to avoid getting into expiry state.

                // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 6);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!waitingForClientsTapTriggered && mpInstance->MatchStatus == MatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;

                // Force a tap to simulate 2nd user joining.
                auto eventArgs = multiplayer_session_change_event_args(clientManager->match_client()->session()->session_reference(), L"", 3);
                clientManager->on_session_changed(eventArgs);
            }

            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    auto findMatchCompleted = static_cast<FindMatchCompletedEventArgs^>(ev->EventArgs);
                    LOGS_DEBUG << " [MPM] MatchStatus: " << findMatchCompleted->MatchStatus.ToString()->Data();

                    VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Completed);
                }
            }

            if (matchFound && utils::str_icmp(mpInstance->LobbySession->Properties->Data(), customPropertyJson[L"custom"].serialize()) == 0)
            {
                isAdvertisingGameDone = true;
            }
        }

        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSRemoteClientJoiningMatchSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchNoQoSRemoteClientJoiningMatchSession);
        FindMatchNoQoSRemoteClientJoiningMatchSessionHelper();
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSInvalidArg)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestFindMatchNoQoSInvalidArg);
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto timeSpan = Windows::Foundation::TimeSpan();
        timeSpan.Duration = 1 * TICKS_PER_SECOND;
        VERIFY_THROWS_HR_CX(mpInstance->FindMatch(HOPPER_NAME_NO_QOS, ref new Platform::String(), timeSpan), E_UNEXPECTED);

        DestructManager(xboxLiveContext);
    }

    void CancelMatchHelper(MatchCallingPatternType callingPattern)
    {
        InitializeManager();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        AddLocalUserHelper(xboxLiveContext, lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::shared_ptr<HttpResponseStruct> matchTicketResponseStruct = std::make_shared<HttpResponseStruct>();
        matchTicketResponseStruct->responseList = { matchTicketResponse };

        std::shared_ptr<HttpResponseStruct> transferHandleResponseStruct = std::make_shared<HttpResponseStruct>();
        transferHandleResponseStruct->responseList = { transferHandleResponse };

        std::shared_ptr<HttpResponseStruct> gameResponseStruct = std::make_shared<HttpResponseStruct>();
        gameResponseStruct->responseList = { matchJoin_1_Response, matchJoin_2_Response };

        std::shared_ptr<HttpResponseStruct> lobbyResponseStruct = std::make_shared<HttpResponseStruct>();
        if (callingPattern == MatchCallingPatternType::CanceledByService)
        {
            lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusCanceledByServiceResponse};
        }
        else
        {
            lobbyResponseStruct->responseList = { matchStatusSearchingResponse, matchStatusFoundResponse, matchStatusCanceledByServiceResponse };
        }

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> matchResponses;
        matchResponses[xbox_live_api::create_match_ticket] = matchTicketResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(matchResponses);

        std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> transferHandleResponses;
        transferHandleResponses[xbox_live_api::set_transfer_handle] = transferHandleResponseStruct;
        m_mockXboxSystemFactory->add_http_api_state_response(transferHandleResponses, false);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[defaultGameHttpHeaderUri] = gameResponseStruct;
        responses[defaultMpsdUri] = lobbyResponseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto mpInstance = MultiplayerManager::SingletonInstance;
        auto clientManager = mpInstance->GetCppObj()->_Get_multiplayer_client_manager();
        auto mpsdLobbySession = clientManager->latest_pending_read()->lobby_client()->session();
        auto lobbySessionWriter = clientManager->latest_pending_read()->lobby_client()->session_writer();

        auto timeSpan = Windows::Foundation::TimeSpan();
        timeSpan.Duration = 1 * TICKS_PER_SECOND;
        mpInstance->FindMatch(HOPPER_NAME_NO_QOS, ref new Platform::String(), timeSpan);

        clientManager->match_client()->disable_next_timer(true); // force the timer to avoid getting into expiry state.

        bool matchFound = false, searchingTapTriggered = false, foundTapTriggered = false, waitingForClientsTapTriggered = false;
        while (!matchFound)
        {
            auto events = mpInstance->DoWork();
            if (mpInstance->MatchStatus == MatchStatus::Searching && !searchingTapTriggered)
            {
                searchingTapTriggered = true;
                auto matchTicketeJson = web::json::value::parse(matchTicket);
                VERIFY_ARE_EQUAL_TIMESPAN_TO_SECONDS(mpInstance->EstimatedMatchWaitTime, matchTicketeJson[_T("waitTime")].as_number().to_uint64());

                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 4);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!foundTapTriggered && mpInstance->LobbySession->GetCppObj()->_Change_number() == 4)
            {
                foundTapTriggered = true;

                // For CanceledByService, this will cause a get with status = canceled.
                auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 6);
                lobbySessionWriter->on_session_changed(eventArgs);
            }

            if (!waitingForClientsTapTriggered && mpInstance->MatchStatus == MatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;
                if (callingPattern == MatchCallingPatternType::Canceled)
                {
                    mpInstance->CancelMatch();

                    // This will trigger a shoulder tap with canceled as the status from the service after deleting the match ticket.
                    auto eventArgs = multiplayer_session_change_event_args(mpsdLobbySession->session_reference(), L"", 8);
                    lobbySessionWriter->on_session_changed(eventArgs);
                }
            }

            for (auto ev : events)
            {
                if (ev->EventType == MultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    auto findMatchCompleted = static_cast<FindMatchCompletedEventArgs^>(ev->EventArgs);
                    LOGS_DEBUG << " [MPM] MatchStatus: " << findMatchCompleted->MatchStatus.ToString()->Data();

                    VERIFY_IS_TRUE(findMatchCompleted->MatchStatus == MatchStatus::Canceled);
                }
            }
        }

        DestructManager(xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestCancelMatch)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCancelMatch);
        CancelMatchHelper(MatchCallingPatternType::Canceled);
    }

    DEFINE_TEST_CASE(TestCancelMatchByService)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCancelMatchByService);
        CancelMatchHelper(MatchCallingPatternType::CanceledByService);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END