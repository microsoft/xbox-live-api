// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"SocialManager"
#include "UnitTestIncludes.h"
#include "RtaTestHelper.h"

#include "social_manager_internal.h"
#include "xsapi/services.h"
#include "SocialManager_WinRT.h"
#include "SocialUserGroupLoadedEventArgs_WinRT.h"
#include "MockSocialManager.h"
#include "SocialManagerHelper.h"
#include "Event_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::presence;
using namespace xbox::services::real_time_activity;
using namespace xbox::services::social::manager;
using namespace Microsoft::Xbox::Services::Social::Manager;
using namespace Microsoft::Xbox::Services::Presence;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const web::json::value defaultPeoplehubTemplate =
web::json::value::parse(LR"(
{
    "xuid": "10000",
    "isFavorite": false,
    "isFollowingCaller": true,
    "isFollowedByCaller": true,
    "isIdentityShared": false,
    "displayName": "TestGamerTag",
    "realName": "",
    "displayPicRaw": "http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png",
    "useAvatar": false,
    "gamertag": "TestGamerTag",
    "gamerScore": "9001",
    "presenceState": "Offline",
    "presenceText": "Offline",
    "presenceDevices": null,
    "isBroadcasting": false,
    "titleHistory": 
    {
        "TitleName": "Forza Horizon 2",
        "TitleId": "1234",
        "LastTimePlayed": "2015-01-26T22:54:54.6630000Z"
    },
    "suggestion": null,
    "multiplayerSummary": {
        "InMultiplayerSession": 0,
        "InParty": 0
    },
    "recentPlayer": null,
    "follower": null,
    "preferredColor": {
        "primaryColor": "193e91",
        "secondaryColor": "2458cf",
        "tertiaryColor": "122e6b"
    },
    "titlePresence": null,
    "titleSummaries": null,
    "presenceDetails": [{
        "IsBroadcasting": false,
        "Device": "PC",
        "State": "Active",
        "TitleId": "1234"
    }]
    }
)");

const web::json::value peoplehubOnlinePresenceTemplate =
web::json::value::parse(LR"(
{
    "xuid": "2533274810838148",
    "state": "Online",
    "devices": [
        {
            "type": "PC",
            "titles": [
                {
                    "id": "1234",
                    "name": "awesomeGame",
                    "lastModified": "2013-02-01T00:00:00Z",
                    "state": "active",
                    "placement": "Full",
                    "activity": {
                        "richPresence": "Home"
                    }
                }
            ]
        }
    ]
}
)");

const web::json::value peoplehubOfflinePresenceTemplate =
web::json::value::parse(LR"(
{
    "xuid": "2533274810838148",
    "state": "Offline",
    "devices": []
}
)");


const web::json::value devicePresenceResponse =
web::json::value::parse(LR"(
[{
    "xuid": "1",
    "state": "Online",
    "devices": [
        {
            "type": "PC",
            "titles": [
                {
                    "id": "1234",
                    "name": "awesomeGame",
                    "lastModified": "2013-02-01T00:00:00Z",
                    "state": "active",
                    "placement": "Full",
                    "activity": {
                        "richPresence": "Home"
                    }
                }
            ]
        }
    ]
}])");

const std::wstring presenceOnlineRtaMessageSubscribeComplete = LR"({"xuid":"2814613569642996","state":"Online","devices":[{"type":"MCapensis","titles":[{"id":"1234","name":"Default Title","placement":"Full","state":"Active", "activity": {"richPresence":"Home"}, "lastModified":"2016-09-30T00:15:35.5994615Z"}]}]})";
const std::wstring presenceOfflineRtaMessageSubscribeComplete = LR"({"xuid":"2814613569642996","state":"Offline"})";

const std::wstring devicePresenceRtaMessageEvent = LR"(PC:false)";
const std::wstring devicePresenceRtaMessagePCEvent = LR"(PC:true)";
const std::wstring devicePresenceRtaMessageXboxEvent = LR"(MCapensis:true)";
const std::wstring devicePresenceOfflineRtaMessageXboxEvent = LR"(MCapensis:false)";

const std::wstring titlePresenceMessage = LR"(ended)";

const std::wstring socialRelationshipAddedMessage = LR"({"NotificationType":"Added","Xuids":["10000"]})";
const std::wstring socialRelationshipRemovedMessage = LR"({"NotificationType":"Removed","Xuids":["1"]})";
const std::wstring socialRelationshipChangedMessage = LR"({"NotificationType":"Changed","Xuids":["1"]})";

const std::wstring errorString = LR"(3)";

const string_t rtaResyncMessage = LR"(
    [4]
)";

const uint32_t m_numUsers = 100;
struct SocialManagerInitializationStruct
{
    web::json::value initialPeoplehubJson;
    SocialManager^ socialManager;
    std::vector<SocialEvent^> socialEvents;
};

string_t EventTypeToString(SocialEventType eventType)
{
    switch (eventType)
    {
    case SocialEventType::UsersAddedToSocialGraph: return _T("UsersAddedToSocialGraph");
    case SocialEventType::UsersRemovedFromSocialGraph: return _T("UsersRemovedFromSocialGraph");
    case SocialEventType::PresenceChanged: return _T("PresenceChanged");
    case SocialEventType::ProfilesChanged: return _T("ProfilesChanged");
    case SocialEventType::SocialRelationshipsChanged: return _T("SocialRelationshipsChanged");
    case SocialEventType::SocialUserGroupUpdated: return _T("SocialUserGroupUpdated");
    case SocialEventType::LocalUserAdded: return _T("LocalUserAdded");
    case SocialEventType::LocalUserRemoved: return _T("LocalUserRemoved");
    case SocialEventType::SocialUserGroupLoaded: return _T("SocialUserGroupLoaded");
    default: return _T("Unknown");
    }

    return _T("");
}

void LogSocialManagerEvents(_In_ Windows::Foundation::Collections::IVectorView<SocialEvent^>^ changeList)
{
    int i = 1;
    for (auto evt : changeList)
    {
        std::wstringstream ss;
        ss << L"SocialManager Event [" << i << L"]: ";
        ss << EventTypeToString(evt->EventType);
        TEST_LOG(ss.str().c_str());
        i++;
    }
}

void AppendToPendingEvents(
    _In_ Windows::Foundation::Collections::IVectorView<SocialEvent^>^ changeList,
    _In_ SocialManagerInitializationStruct& socialManagerInitializationStruct
)
{
    for (auto evt : changeList)
    {
        socialManagerInitializationStruct.socialEvents.push_back(evt);
    }
}

DEFINE_TEST_CLASS(SocialManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SocialManagerTests)

    static std::vector<Platform::String^> GenerateUserList()
    {
        std::vector<Platform::String^> userList;
        userList.reserve(1000);
        for (uint32_t i = 1; i < m_numUsers; ++i)
        {
            Platform::String^ stream;
            stream += i;
            userList.push_back(stream);
        }

        return userList;
    }

    std::vector<Platform::String^> USER_LIST = GenerateUserList();

    web::json::value GenerateInitialPeoplehubJSON()
    {
        web::json::value jsonArray = web::json::value::array();
        for (uint32_t i = 1; i < m_numUsers; ++i)
        {
            stringstream_t stream;
            stream << i;
            auto jsonBlob = defaultPeoplehubTemplate;
            jsonBlob[L"xuid"] = web::json::value::string(stream.str());
            jsonArray[i - 1] = jsonBlob;
        }

        web::json::value returnObject;
        returnObject[L"people"] = jsonArray;
        return returnObject;
    }

    web::json::value GenerateInitialPresenceJSON(bool useOnline)
    {
        web::json::value jsonArray = web::json::value::array();
        for (uint32_t i = 1; i < m_numUsers; ++i)
        {
            stringstream_t stream;
            stream << i;
            auto jsonBlob = useOnline ? peoplehubOnlinePresenceTemplate : peoplehubOfflinePresenceTemplate;
            jsonBlob[L"xuid"] = web::json::value::string(stream.str());
            jsonArray[i - 1] = jsonBlob;
        }

        web::json::value returnObject;
        returnObject = jsonArray;
        return returnObject;
    }

    std::queue<WebsocketMockResponse> GenerateMockEvents(const std::vector<Platform::String^>& uriList, const string_t& presenceWebsocketResponse, real_time_activity_message_type eventType)
    {
        std::queue<WebsocketMockResponse> responseQueue;
        for (auto uri : uriList)
        {
            WebsocketMockResponse mockResponse = { uri->Data(), presenceWebsocketResponse, eventType, false };
            responseQueue.push(mockResponse);
        }

        return responseQueue;
    }

    void SetDevicePresenceHTTPMock()
    {
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto responseStruct = GetPresenceResponseStruct(devicePresenceResponse);
        responseStruct->fRequestPostFuncInternal = [](std::shared_ptr<http_call_response_internal>& initialCallResponse, const xsapi_internal_string& requestBody)
        {
            std::error_code errc;
            auto jsonRequest = web::json::value::parse(utils::string_t_from_internal_string(requestBody), errc);
            if (errc)
            {
                return;
            }
            auto userArr = jsonRequest[L"users"];
            web::json::value newResponse = web::json::value::array();
            auto jsonResponseTemplate = initialCallResponse->response_body_json();
            for (uint32_t i = 0; i < userArr.size(); ++i)
            {
                auto user = userArr[i];
                auto responseTemplate = jsonResponseTemplate[0];
                responseTemplate[L"xuid"] = user;
                newResponse[i] = responseTemplate;
            }

            initialCallResponse->set_response_body(newResponse);
        };
        responses[_T("https://userpresence.mockenv.xboxlive.com")] = responseStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses);
    }

    void InitializeSubscriptions(const std::vector<Platform::String^> userList, const pplx::task_completion_event<void>& tce)
    {
        SetDevicePresenceHTTPMock();

        std::vector<Platform::String^> uriList;
        for (auto user : userList)
        {
            user = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/devices";
            uriList.push_back(user);
        }

        for (auto user : userList)
        {
            Platform::String^ stream = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/titles/1234";
            uriList.push_back(stream);
        }
        auto websocketResponseQueue = GenerateMockEvents(uriList, presenceOnlineRtaMessageSubscribeComplete, real_time_activity_message_type::subscribe);
        WebsocketMockResponse mockResponse = { L"http://social.xboxlive.com/users/xuid(TestXboxUserId)/friends", web::json::value::null().serialize(), real_time_activity_message_type::subscribe, false };
        websocketResponseQueue.push(mockResponse);
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(websocketResponseQueue, tce);
    }

    void InitializeSubscriptionsWithErrors(const std::vector<Platform::String^> userList, const pplx::task_completion_event<void>& tce)
    {
        std::vector<Platform::String^> uriList;
        for (auto user : userList)
        {
            user = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/devices";
            uriList.push_back(user);
        }

        for (auto user : userList)
        {
            Platform::String^ stream = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/titles/1234";
            uriList.push_back(stream);
        }
        auto websocketResponseQueue = GenerateMockEvents(uriList, errorString, real_time_activity_message_type::subscribe);
        WebsocketMockResponse mockResponse = { L"http://social.xboxlive.com/users/xuid(TestXboxUserId)/friends", _T("3"), real_time_activity_message_type::subscribe, true };
        websocketResponseQueue.push(mockResponse);
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(websocketResponseQueue, tce);
    }

    void TestDevicePresenceChange(const std::vector<Platform::String^> userList, const pplx::task_completion_event<void>& tce, const string_t& tdevicePresenceResponse)
    {
        std::vector<Platform::String^> uriList;
        for (auto user : userList)
        {
            Platform::String^ stream = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/devices";
            uriList.push_back(stream);
        }

        auto mockEvents = GenerateMockEvents(uriList, tdevicePresenceResponse, real_time_activity_message_type::change_event);
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(mockEvents, tce);
    }

    void TestTitlePresenceChange(const std::vector<Platform::String^> userList, const pplx::task_completion_event<void>& tce)
    {
        std::vector<Platform::String^> uriList;
        for (auto& user : userList)
        {
            Platform::String^ stream = L"https://userpresence.xboxlive.com/users/xuid(" + user + L")/titles/1234";
            uriList.push_back(stream);
        }

        auto mockEvents = GenerateMockEvents(uriList, web::json ::value::string(titlePresenceMessage).serialize(), real_time_activity_message_type::change_event);
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(mockEvents, tce);
    }

    void TestSocialGraphChange(const std::vector<Platform::String^> userList, const pplx::task_completion_event<void>& tce)
    {
        std::queue<WebsocketMockResponse> responseQueue;
        auto jsonVal = web::json::value::parse(socialRelationshipRemovedMessage);

        responseQueue.push({ L"http://social.xboxlive.com/users/xuid(TestXboxUserId)/friends", socialRelationshipRemovedMessage, real_time_activity_message_type::change_event, false });
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(responseQueue, tce);
    }

    std::shared_ptr<HttpResponseStruct> GetPeoplehubResponseStruct(const web::json::value& initJSON, int errorNum = 200)
    {
        // Set up initial http responses
        auto peoplehubResponse2 = StockMocks::CreateMockHttpCallResponseInternal(initJSON, errorNum);
        std::shared_ptr<HttpResponseStruct> peoplehubResponseStruct = std::make_shared<HttpResponseStruct>();
        peoplehubResponseStruct->responseListInternal = { peoplehubResponse2 };
        return peoplehubResponseStruct;
    }

    std::shared_ptr<HttpResponseStruct> GetPresenceResponseStruct(const web::json::value& initJSON, int errorNum = 200)
    {
        auto presenceResponse = StockMocks::CreateMockHttpCallResponseInternal(initJSON, errorNum);
        std::shared_ptr<HttpResponseStruct> presenceResponseStruct = std::make_shared<HttpResponseStruct>();
        presenceResponseStruct->responseListInternal = { presenceResponse };
        return presenceResponseStruct;
    }

    SocialManagerInitializationStruct InitializeSocialManager(const std::shared_ptr<xbox_live_context>& xboxLiveContext, bool initPresenceOnline)
    {
        UNREFERENCED_PARAMETER(initPresenceOnline);
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        auto peoplehubResponseJson = GenerateInitialPeoplehubJSON();

        auto peoplehubResponseStruct = GetPeoplehubResponseStruct(peoplehubResponseJson);

        // set up http response set
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = peoplehubResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto socialManager = SocialManager::SingletonInstance;
        socialManager->LogState();
        socialManager->AddLocalUser(xboxLiveContext->user(), SocialManagerExtraDetailLevel::NoExtraDetail);

        std::vector<SocialEvent^> socialEvents;

        TEST_LOG(L"Calling socialManager->AddLocalUser");
        TEST_LOG(L"Looking for LocalUserAdded");
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            for (auto evt : changeList)
            {
                socialEvents.push_back(evt);

                if (evt->EventType == SocialEventType::LocalUserAdded)
                {
                    TEST_LOG(L"Found LocalUserAdded");
                    shouldLoop = false;
                    break;
                }
            }
        } while (shouldLoop);

        SocialManagerInitializationStruct socialManagerInitializationStruct = { peoplehubResponseJson, socialManager, socialEvents };
        return socialManagerInitializationStruct;
    }

    SocialManagerInitializationStruct InitializeSocialManagerWithError(const std::shared_ptr<xbox_live_context>& xboxLiveContext)
    {
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        auto peoplehubResponseJson = GenerateInitialPeoplehubJSON();

        auto peoplehubResponseStruct = GetPeoplehubResponseStruct(peoplehubResponseJson, 404);

        // set up http response set
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = peoplehubResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto socialManager = SocialManager::SingletonInstance;
        socialManager->AddLocalUser(xboxLiveContext->user(), SocialManagerExtraDetailLevel::NoExtraDetail);

        std::vector<SocialEvent^> socialEvents;

        bool shouldLoop = true;
        do
        {
            auto changeList = socialManager->DoWork();
            LogSocialManagerEvents(changeList);

            for (auto evt : changeList)
            {
                socialEvents.push_back(evt);

                if (evt->EventType == SocialEventType::LocalUserAdded)
                {
                    shouldLoop = false;
                    VERIFY_ARE_EQUAL_INT(evt->ErrorCode, xbox::services::utils::convert_xbox_live_error_code_to_hresult(xbox_live_error_code::http_status_404_not_found));
                    break;
                }
            }
        } while (shouldLoop);

        SocialManagerInitializationStruct socialManagerInitializationStruct = { peoplehubResponseJson, socialManager, socialEvents };
        return socialManagerInitializationStruct;
    }

    SocialManagerInitializationStruct Initialize(std::shared_ptr<xbox_live_context> xboxLiveContext, bool initPresenceOnline, int httpErrorCode = 200)
    {
        UNREFERENCED_PARAMETER(httpErrorCode);
        LOG_DEBUG("Initalizing");
        auto socialManagerInitializationStruct = InitializeSocialManager(xboxLiveContext, initPresenceOnline);

        pplx::task_completion_event<void> tce;
        InitializeSubscriptions(USER_LIST, tce);
        create_task(tce).wait();

        LOG_DEBUG("Subs initialized");
        socialManagerInitializationStruct.socialEvents.clear();
        
        while (true)  // todo: find other way to confirm subs
        {
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            size_t totalSize = 0;
            size_t numEvents = 0;
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    assert(evt->UsersAffected->Size != 0);
                    totalSize += evt->UsersAffected->Size;
                    ++numEvents;
                }
            }

            if (totalSize == USER_LIST.size())
            {
                break;
            }
        }

        socialManagerInitializationStruct.socialEvents.clear();
        LOG_DEBUG("Ending Initalizion");
        return socialManagerInitializationStruct;
    }

    void Cleanup(
        _In_ SocialManagerInitializationStruct& socialManagerInitializationStruct,
        _In_ const std::shared_ptr<xbox_live_context>& xblContext,
        _In_ uint32_t overrideCount = 0
        )
    {
        TEST_LOG(L"Cleanup");
        socialManagerInitializationStruct.socialEvents.clear();
        socialManagerInitializationStruct.socialManager->RemoveLocalUser(xblContext->user());
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == overrideCount);
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::LocalUserRemoved)
                {
                    TEST_LOG(L"Found LocalUserRemoved");
                    shouldLoop = false;
                    break;
                }
            }

        } while (shouldLoop);
        Sleep(100);
        VERIFY_IS_TRUE(xblContext->real_time_activity_service()->_Subscription_Count() == 0);
    }

    DEFINE_TEST_CASE(TestSocialManagerInitialize)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerInitialize);

        //while (true)
        {
            m_mockXboxSystemFactory->reinit();
            auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
            auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
            Cleanup(socialManagerInitializationStruct, xboxLiveContext);
        }
    }

    // Verifies that properties of multiple xbox social user are set correctly
    DEFINE_TEST_CASE(TestSocialManagerXboxSocialUser)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerXboxSocialUser);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        bool wasFound = false;
        do
        {
            auto events = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(events);
            AppendToPendingEvents(events, socialManagerInitializationStruct);

            for (auto evt : events)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    wasFound = true;
                    break;
                }
            }
        } while (!wasFound);

        for (auto user : socialUserGroup->Users)
        {
            // profile tests
            VERIFY_ARE_EQUAL_STR(L"TestGamerTag", user->DisplayName);
            VERIFY_ARE_EQUAL_STR(L"http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user->DisplayPicUrlRaw);
            VERIFY_IS_TRUE(user->IsFollowedByCaller);
            VERIFY_IS_TRUE(user->IsFollowingUser);
            VERIFY_ARE_EQUAL_STR(L"9001", user->Gamerscore);
            VERIFY_ARE_EQUAL_STR(L"TestGamerTag", user->Gamertag);
            VERIFY_IS_FALSE(user->IsFavorite);
            VERIFY_IS_FALSE(user->UseAvatar);

            // preferred color tests
            VERIFY_ARE_EQUAL_STR(L"193e91", user->PreferredColor->PrimaryColor);
            VERIFY_ARE_EQUAL_STR(L"2458cf", user->PreferredColor->SecondaryColor);
            VERIFY_ARE_EQUAL_STR(L"122e6b", user->PreferredColor->TertiaryColor);

            // presence record tests
            VERIFY_IS_TRUE(user->PresenceRecord->PresenceTitleRecords->Size == 1);
            VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
            VERIFY_IS_TRUE(user->PresenceRecord->IsUserPlayingTitle(1234));
            VERIFY_IS_TRUE(user->PresenceRecord->PresenceTitleRecords->GetAt(0)->IsTitleActive);
            VERIFY_IS_TRUE(!user->PresenceRecord->PresenceTitleRecords->GetAt(0)->IsBroadcasting);
            VERIFY_IS_TRUE(user->PresenceRecord->PresenceTitleRecords->GetAt(0)->DeviceType == PresenceDeviceType::PC);
            VERIFY_ARE_EQUAL_STR(L"Home", user->PresenceRecord->PresenceTitleRecords->GetAt(0)->PresenceText);
            
            // title history tests
            VERIFY_IS_TRUE(user->TitleHistory->HasUserPlayed);
            auto str = DateTimeToString(user->TitleHistory->LastTimeUserPlayed);
            VERIFY_IS_TRUE(utils::str_icmp(str, L"2015-01-26T22:54:54.6630Z") == 0);
        }

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests all basic features of social manager. Initialization, destruction, verification of social user group, and changes to device presence and social graph changes
    DEFINE_TEST_CASE(TestSocialManagerInitialization)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerInitialization);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManagerInitializationStruct.socialManager->GetCppObj());

        socialManagerInitializationStruct.socialManager->DoWork();
        auto group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );
        auto events = socialManagerInitializationStruct.socialManager->DoWork();
        LogSocialManagerEvents(events);
        AppendToPendingEvents(events, socialManagerInitializationStruct);

        bool wasFound = false;
        for (auto evt : socialManagerInitializationStruct.socialEvents)
        {
            if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
            {
                wasFound = true;

                auto socialUserGroupLoaded = static_cast<SocialUserGroupLoadedEventArgs^>(evt->EventArgs);
                VERIFY_IS_TRUE(socialUserGroupLoaded->SocialUserGroup->SocialUserGroupType == SocialUserGroupType::FilterType);
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(socialUserGroupLoaded->SocialUserGroup->LocalUser->XboxUserId->Data(), xboxLiveContext->xbox_live_user_id().c_str());

            }
        }

        VERIFY_IS_TRUE(wasFound);

        auto socialUsers = group->Users;
        while (socialUsers->Size != socialManagerInitializationStruct.initialPeoplehubJson[L"people"].as_array().size())
        {
            socialManagerInitializationStruct.socialManager->DoWork();
        }
        VERIFY_ARE_EQUAL_INT(socialManagerInitializationStruct.initialPeoplehubJson[L"people"].as_array().size(), socialUsers->Size);

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
                }

            }
        }

        auto& localGraph = socialManagerCppMock->local_graphs().at(_T("TestXboxUserId"));
        localGraph->print_debug_info();
        localGraph->clear_debug_counters();

        pplx::task_completion_event<void> tce;

        TestTitlePresenceChange(USER_LIST, tce);

        create_task(tce).wait();
        tce = pplx::task_completion_event<void>();

        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        while (true)
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);
            size_t currSize = 0;
            for (auto& evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    currSize += evt->UsersAffected->Size;
                }
            }

            localGraph->print_debug_info();
            if (currSize == group->UsersTrackedBySocialUserGroup->Size)
            {
                break;
            }
        }

        socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_IS_FALSE(user->PresenceRecord->IsUserPlayingTitle(1234));
                }

            }
        }

        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessageEvent).serialize());

        create_task(tce).wait();
        tce = pplx::task_completion_event<void>();

        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        socialManagerInitializationStruct.socialEvents.clear();
        std::unordered_map<Platform::String^, uint32_t> userMap;
        while (true)
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);
            size_t currSize = 0;
            for (auto evt : changeList)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    currSize += evt->UsersAffected->Size;
                    for (auto user : evt->UsersAffected)
                    {
                        auto iter = userMap.find(user);
                        if (iter != userMap.end())
                        {
                            ++iter->second;
                        }
                        else
                        {
                            userMap[user] = 0;
                        }

                        for (auto userEntry : group->Users)
                        {
                            if (user == userEntry->XboxUserId)
                            {
                                VERIFY_IS_TRUE(userEntry->PresenceRecord->UserState == UserPresenceState::Offline);
                            }
                        }
                    }
                }
            }

            if (userMap.size() == group->UsersTrackedBySocialUserGroup->Size)
            {
                break;
            }
        }

        socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_ARE_EQUAL_INT(user->PresenceRecord->UserState, UserPresenceState::Offline);
                }
            }
        }

        TestSocialGraphChange(USER_LIST, tce);

        create_task(tce).wait();
        tce = pplx::task_completion_event<void>();

        socialManagerInitializationStruct.socialManager->DoWork();
        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        socialManagerInitializationStruct.socialManager->DoWork();
        socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests to verify that multiple local users can be added, initialized correctly, and do not conflict in data
    DEFINE_TEST_CASE(TestSocialManagerMultipleLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerMultipleLocalUser);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto xboxLiveContext1 = GetMockXboxLiveContext_Cpp();
        xboxLiveContext1->user()->_User_impl()->_Set_xbox_user_id("T0");
        auto socialManagerInitializationStruct1 = Initialize(xboxLiveContext1, true);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManagerInitializationStruct1.socialManager->GetCppObj());

        VERIFY_IS_TRUE(socialManagerInitializationStruct1.socialManager->LocalUsers->Size == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 1);
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 2);

        socialManagerInitializationStruct.socialManager->DoWork();
        auto group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        socialManagerInitializationStruct.socialManager->DoWork();
        auto socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        VERIFY_ARE_EQUAL_INT(socialManagerInitializationStruct.initialPeoplehubJson[L"people"].as_array().size(), socialUsers->Size);

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
                }
            }
        }

        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        pplx::task_completion_event<void> tce;

        socialManagerInitializationStruct.socialEvents.clear();
        TestTitlePresenceChange(USER_LIST, tce);

        while (true)
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);
            size_t currSize = 0;
            for (auto& evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    currSize += evt->UsersAffected->Size;
                }
            }

            if (currSize == group->UsersTrackedBySocialUserGroup->Size)
            {
                break;
            }
        }
        create_task(tce).wait();
        tce = pplx::task_completion_event<void>();

        socialManagerInitializationStruct.socialManager->DoWork();
        socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_IS_FALSE(user->PresenceRecord->IsUserPlayingTitle(1234));
                }

            }
        }

        socialManagerInitializationStruct.socialEvents.clear();
        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessageEvent).serialize());
        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        while (true)
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);
            size_t currSize = 0;
            for (auto& evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    currSize += evt->UsersAffected->Size;
                }
            }

            if (currSize == group->UsersTrackedBySocialUserGroup->Size)
            {
                break;
            }
        }

        create_task(tce).wait();
        tce = pplx::task_completion_event<void>();

        group = socialManagerInitializationStruct1.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        socialManagerInitializationStruct.socialEvents.clear();
        socialUsers = group->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        for (auto& userStr : USER_LIST)
        {
            for (auto user : socialUsers)
            {
                if (userStr == user->XboxUserId)
                {
                    VERIFY_ARE_EQUAL_INT(user->PresenceRecord->UserState, UserPresenceState::Offline);
                }

            }
        }
        socialManagerInitializationStruct.socialEvents.clear();
        tce = pplx::task_completion_event<void>();
        TestSocialGraphChange(USER_LIST, tce);
        create_task(tce).wait();

        bool isFound = false;
        while (!isFound)
        {
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::UsersRemovedFromSocialGraph)
                {
                    isFound = true;
                }
            }
        }

        socialManagerInitializationStruct.socialManager->DoWork();
        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        VERIFY_ARE_EQUAL_UINT(group->Users->Size, USER_LIST.size() - 1);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext, 1);
        Cleanup(socialManagerInitializationStruct, xboxLiveContext1);
    }

    // Tests to verify that multiple local users can create groups and have groups update properly
    DEFINE_TEST_CASE(TestSocialManagerMultipleLocalUserWithGroups)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerMultipleLocalUserWithGroups);
        m_mockXboxSystemFactory->reinit();
        auto mockSockets = m_mockXboxSystemFactory->AddMultipleMockWebSocketClients(2);
        const std::wstring rtaConnectionIdJson =
        LR"(
        {
            "ConnectionId": "d01a8c1b-2f83-4e03-9278-3048b480928f"
        }
        )";
        SetMultipleClientWebSocketRTAAutoResponser(mockSockets, rtaConnectionIdJson, -1, false);
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto xboxLiveContext1 = GetMockXboxLiveContext_Cpp();
        xboxLiveContext1->user()->_User_impl()->_Set_xbox_user_id("T0");
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        auto socialManagerInitializationStruct1 = Initialize(xboxLiveContext1, false);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManagerInitializationStruct1.socialManager->GetCppObj());
        std::vector<Platform::String^> stdVec = { _T("1"), _T("2"), _T("3") };
        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>({ _T("1"), _T("2"), _T("3") });
        Platform::Collections::Vector<Platform::String^>^ vec1 = ref new Platform::Collections::Vector<Platform::String^>({ _T("4"), _T("5"), _T("6") });
        auto socialManager = socialManagerInitializationStruct.socialManager;
        auto groupA = socialManager->CreateSocialUserGroupFromList(
            xboxLiveContext->user(),
            vec->GetView()
            );

        auto groupB = socialManager->CreateSocialUserGroupFromList(
            xboxLiveContext1->user(),
            vec1->GetView()
            );

        bool shouldLoop = true;
        uint32_t counter = 0;
        do
        {
            auto changeList = socialManager->DoWork();
            LogSocialManagerEvents(changeList);

            for (auto evt : changeList)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    ++counter;
                    if (counter == 2)
                    {
                        shouldLoop = false;
                    }
                }
            }
        } while (shouldLoop);
        
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 2);

        uint32_t i = 0;
        for (auto user : groupA->Users)
        {
            VERIFY_IS_TRUE(utils::str_icmp(user->XboxUserId->Data(), vec->GetAt(i)->Data()) == 0);
            ++i;
        }

        i = 0;
        for (auto user : groupB->Users)
        {
            VERIFY_IS_TRUE(utils::str_icmp(user->XboxUserId->Data(), vec1->GetAt(i)->Data()) == 0);
            ++i;
        }
        
        VERIFY_IS_TRUE(groupB->Users->Size == 3);
        socialManager->DestroySocialUserGroup(groupB);
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 1);
        auto changeList = socialManager->DoWork();
        
        i = 0;
        VERIFY_IS_TRUE(groupB->Users->Size == 0);
        groupB = nullptr;
        for (auto user : groupA->Users)
        {
            VERIFY_IS_TRUE(utils::str_icmp(user->XboxUserId->Data(), vec->GetAt(i)->Data()) == 0);
            ++i;
        }

        VERIFY_IS_TRUE(groupA->Users->Size == 3);
        socialManager->DestroySocialUserGroup(groupA);
        VERIFY_IS_TRUE(groupA->Users->Size == 0);
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 0);

        groupA = socialManager->CreateSocialUserGroupFromList(
            xboxLiveContext->user(),
            vec->GetView()
            );

        groupB = socialManager->CreateSocialUserGroupFromList(
            xboxLiveContext1->user(),
            vec->GetView()
            );

        shouldLoop = true;
        counter = 0;
        do
        {
            auto changeList2 = socialManager->DoWork();
            LogSocialManagerEvents(changeList2);

            for (auto evt : changeList2)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    ++counter;
                    if (counter == 2)
                    {
                        shouldLoop = false;
                    }
                }
            }
        } while (shouldLoop);

        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 2);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 2);
        VERIFY_IS_TRUE(groupA->Users->Size == 3);
        socialManager->DestroySocialUserGroup(groupB);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 1);

        VERIFY_IS_TRUE(groupB->Users->Size == 0);
        VERIFY_IS_TRUE(groupA->Users->Size == 3);
        i = 0;
        for (auto user : groupA->Users)
        {
            VERIFY_IS_TRUE(utils::str_icmp(user->XboxUserId->Data(), vec->GetAt(i)->Data()) == 0);
            VERIFY_IS_TRUE(user->PresenceRecord->PresenceTitleRecords->Size == 1);
            ++i;
        }

        i = 0;
        for (auto user : groupB->Users)
        {
            VERIFY_IS_TRUE(utils::str_icmp(user->XboxUserId->Data(), vec->GetAt(i)->Data()) == 0);
            VERIFY_IS_TRUE(user->PresenceRecord->PresenceTitleRecords->Size == 1);
            ++i;
        }
        
        Cleanup(socialManagerInitializationStruct, xboxLiveContext, 1);
        groupB = socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext1->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        pplx::task_completion_event<void> tce;
        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessageEvent).serialize());
        create_task(tce).wait();
        shouldLoop = true;
        socialManagerInitializationStruct1.socialEvents.clear();
        socialManagerInitializationStruct1.socialEvents.clear();
        while (true)
        {
            AppendToPendingEvents(socialManagerInitializationStruct1.socialManager->DoWork(), socialManagerInitializationStruct1);
            size_t totalSize = 0;
            size_t numEvents = 0;
            for (auto evt : socialManagerInitializationStruct1.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    VERIFY_IS_TRUE(utils::str_icmp(evt->User->XboxUserId->Data(), xboxLiveContext1->user()->XboxUserId->Data()) == 0);
                    totalSize += evt->UsersAffected->Size;
                    ++numEvents;
                }
            }

            if (totalSize == USER_LIST.size())
            {
                break;
            }
        }

        VERIFY_IS_TRUE(groupB->Users->GetAt(0)->PresenceRecord->PresenceTitleRecords->Size == 0);

        Cleanup(socialManagerInitializationStruct1, xboxLiveContext1);
    }

    // Tests race condition in adding then removing a local user before adding is complete
    DEFINE_TEST_CASE(TestSocialManagerAddRemoveLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerAddRemoveCallback);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        socialManagerInitializationStruct.socialManager->RemoveLocalUser(xboxLiveContext->user());
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 0);
        socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 1);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests device presence change tap
    DEFINE_TEST_CASE(TestSocialManagerPresenceCallback)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerPresenceCallback);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);

        std::queue<WebsocketMockResponse> responseQueue;
        responseQueue.push({ L"https://userpresence.xboxlive.com/users/xuid(1)/devices", web::json::value::string(devicePresenceRtaMessagePCEvent).serialize(), real_time_activity_message_type::change_event, false });
        pplx::task_completion_event<void> tce;

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        auto devicePresenceResponseCopy = devicePresenceResponse;
        devicePresenceResponseCopy[0][L"state"] = web::json::value::string(L"Offline");
        auto presenceResponse = StockMocks::CreateMockHttpCallResponseInternal(devicePresenceResponseCopy);
        auto presenceResponseStruct = std::make_shared<HttpResponseStruct>();
        presenceResponseStruct->responseListInternal = { presenceResponse };

        responses[_T("https://userpresence.mockenv.xboxlive.com")] = presenceResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(responseQueue, tce);

        create_task(tce).wait();
        socialManagerInitializationStruct.socialEvents.clear();
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    shouldLoop = false;
                    break;
                }
            }
        } while (shouldLoop);

        std::list<PresenceDeviceType> deviceTypeList = { PresenceDeviceType::XboxOne, PresenceDeviceType::PC };
        auto coreSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::AllOnline,
            RelationshipFilter::Friends
            );
        socialManagerInitializationStruct.socialManager->DoWork();
        for (auto user : coreSocialUserGroup->Users)
        {
            for (auto presenceTitleRecord : user->PresenceRecord->PresenceTitleRecords)
            {
                for (auto& deviceType : deviceTypeList)
                {
                    if (deviceType == presenceTitleRecord->DeviceType)
                    {
                        deviceTypeList.remove(deviceType);
                        break;
                    }
                }
            }
        }

        VERIFY_IS_TRUE(deviceTypeList.size() == 1);
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(coreSocialUserGroup);
        coreSocialUserGroup = nullptr;

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests users added tap
    DEFINE_TEST_CASE(TestSocialManagerUsersAddedCallback)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUsersAddedCallback);
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        m_mockXboxSystemFactory->reinit();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        web::json::value returnObject;
        web::json::value jsonArray = web::json::value::array();
        jsonArray[0] = defaultPeoplehubTemplate;
        returnObject[L"people"] = jsonArray;
        auto relationshipAddedResponse = StockMocks::CreateMockHttpCallResponseInternal(returnObject);
        auto relationshipAddedStruct = std::make_shared<HttpResponseStruct>();
        relationshipAddedStruct->responseListInternal = { relationshipAddedResponse };

        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = relationshipAddedStruct;
        m_mockXboxSystemFactory->add_http_state_response(responses, false);

        pplx::task_completion_event<void> tce;
        std::queue<WebsocketMockResponse> responseQueue;
        responseQueue.push({ L"http://social.xboxlive.com/users/xuid(TestXboxUserId)/friends", socialRelationshipAddedMessage, real_time_activity_message_type::change_event, false });
        TEST_LOG(L"PreRTA");
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(responseQueue, tce);
        create_task(tce).wait();
        TEST_LOG(L"PostRTA");

        bool shouldLoop = true;
        std::chrono::milliseconds maxDeltaTime(std::chrono::milliseconds(2500));
        auto startTime = std::chrono::system_clock::now();

        TEST_LOG(L"Looking for UsersAddedToSocialGraph");
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::UsersAddedToSocialGraph)
                {
                    TEST_LOG(L"Found UsersAddedToSocialGraph");
                    shouldLoop = false;
                    break;
                }
            }

            auto currentTime = std::chrono::system_clock::now();
            auto curDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            if (curDeltaTime.count() > maxDeltaTime.count())
            {
                TEST_LOG(L"Abort - test took too long");
                VERIFY_IS_TRUE(false);
            }

        } while (shouldLoop);

        TEST_LOG(L"clear_states");
        m_mockXboxSystemFactory->clear_states();
        auto coreGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        socialManagerInitializationStruct.socialManager->DoWork();

        bool isInGraph = false;
        for (auto users : coreGroup->Users)
        {
            if (users->XboxUserId == _T("9"))
            {
                isInGraph = true;
            }
        }
        VERIFY_IS_TRUE(isInGraph);

        TEST_LOG(L"DestroySocialUserGroup");
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(coreGroup);
        coreGroup = nullptr;

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests behavior of user group from filter
    DEFINE_TEST_CASE(TestSocialManagerSocialUserGroupFromFilter)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerSocialUserGroupFromFilter);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(xboxLiveContext->user(), PresenceFilter::AllOnline, RelationshipFilter::Friends);
        auto users = socialUserGroup->Users;
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(socialUserGroup);
        socialUserGroup = nullptr;

        socialManagerInitializationStruct.socialManager->DoWork();
        for (auto user : users)
        {
            VERIFY_IS_TRUE(user->IsFollowedByCaller);
            VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
        }

        socialManagerInitializationStruct.socialEvents.clear();

        socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(xboxLiveContext->user(), PresenceFilter::TitleOnline, RelationshipFilter::Friends);
        socialManagerInitializationStruct.socialManager->DoWork();
        users = socialUserGroup->Users;
        for (auto user : users)
        {
            VERIFY_IS_TRUE(user->IsFollowedByCaller);
            VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
        }

        socialManagerInitializationStruct.socialEvents.clear();
        pplx::task_completion_event<void> tce;
        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessageEvent).serialize());
        create_task(tce).wait();

        std::unordered_map<Platform::String^, Platform::String^> map;
        while (true)
        {
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            for (auto& evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    for (auto user : evt->UsersAffected)
                    {
                        map[user] = user;
                    }
                }
            }

            if (map.size() == USER_LIST.size())
            {
                break;
            }
        }
        users = socialUserGroup->Users;
        VERIFY_IS_TRUE(users->Size == 0);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    std::shared_ptr<HttpResponseStruct> CreateSocialGroupFromListResponse(Platform::Collections::Vector<Platform::String^>^ vec, int errorCode = 200)
    {
        web::json::value jsonArray = web::json::value::array();
        for (uint32_t i = 0; i < vec->Size; ++i)
        {
            auto jsonBlob = defaultPeoplehubTemplate;
            jsonBlob[L"xuid"] = web::json::value::string(vec->GetAt(i)->Data());
            jsonArray[i] = jsonBlob;
        }

        web::json::value returnObject;
        returnObject[L"people"] = jsonArray;
        return GetPeoplehubResponseStruct(returnObject, errorCode);
    }

    // Tests behavior of user group from list
    DEFINE_TEST_CASE(TestSocialManagerSocialUserGroupFromList)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerSocialUserGroupFromList);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        std::vector<Platform::String^> stringVec = { _T("100001"), _T("100002"), _T("100003") };
        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>(stringVec);
        
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(vec);
        responses[_T("https://userpresence.mockenv.xboxlive.com")] = GetPresenceResponseStruct(peoplehubOnlinePresenceTemplate);

        m_mockXboxSystemFactory->add_http_state_response(responses);
        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), vec->GetView());

        socialManagerInitializationStruct.socialEvents.clear();
        TEST_LOG(L"Looking for SocialUserGroupLoaded");
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    TEST_LOG(L"Found SocialUserGroupLoaded");
                    auto socialUserGroupLoaded = static_cast<SocialUserGroupLoadedEventArgs^>(evt->EventArgs);
                    VERIFY_IS_TRUE(socialUserGroupLoaded->SocialUserGroup->SocialUserGroupType == SocialUserGroupType::UserListType);
                    VERIFY_ARE_EQUAL_STR_IGNORE_CASE(socialUserGroupLoaded->SocialUserGroup->LocalUser->XboxUserId->Data(), xboxLiveContext->xbox_live_user_id().c_str());

                    for (auto user : evt->UsersAffected)
                    {
                        bool userFound = false;
                        for (const auto& stringUser : vec)
                        {
                            if (user == stringUser)
                            {
                                userFound = true;
                            }
                        }

                        VERIFY_IS_TRUE(userFound);
                    }

                    shouldLoop = false;
                    break;
                }
            }
        } while (shouldLoop);

        VERIFY_IS_TRUE(socialUserGroup->UsersTrackedBySocialUserGroup->Size == vec->Size);
        auto users = socialUserGroup->Users;

        VERIFY_ARE_EQUAL_INT(vec->Size, users->Size);
        socialManagerInitializationStruct.socialEvents.clear();
        task_completion_event<void> tce;
        InitializeSubscriptions(stringVec, tce);
        while (true)
        {
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            size_t totalSize = 0;
            size_t numEvents = 0;
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    totalSize += evt->UsersAffected->Size;
                    ++numEvents;
                }
            }

            if (totalSize == stringVec.size()) // todo: fix
            {
                break;
            }
        }

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSocialUserGroupFromListLarge)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialUserGroupFromListLarge);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        std::vector<Platform::String^> stringVec;
        for (uint32_t i = 0; i < 100; ++i)
        {
            stringstream_t str;
            str << _T("10000") << i;
            auto cStr = str.str();
            stringVec.push_back(ref new Platform::String(cStr.c_str()));
        }

        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>(stringVec);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(vec);
        m_mockXboxSystemFactory->add_http_state_response(responses);
        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), vec->GetView());

        bool shouldLoop = true;
        socialManagerInitializationStruct.socialEvents.clear();
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    TEST_LOG(L"Found SocialUserGroupLoaded");
                    auto socialUserGroupLoaded = static_cast<SocialUserGroupLoadedEventArgs^>(evt->EventArgs);
                    VERIFY_IS_TRUE(socialUserGroupLoaded->SocialUserGroup->SocialUserGroupType == SocialUserGroupType::UserListType);
                    VERIFY_ARE_EQUAL_STR_IGNORE_CASE(socialUserGroupLoaded->SocialUserGroup->LocalUser->XboxUserId->Data(), xboxLiveContext->xbox_live_user_id().c_str());

                    VERIFY_ARE_EQUAL_UINT(vec->Size, socialUserGroupLoaded->SocialUserGroup->UsersTrackedBySocialUserGroup->Size);
                    //VERIFY_ARE_EQUAL(vec->Size, socialUserGroupLoaded->SocialUserGroup->Users->Size);
                    for (const auto& user : vec)
                    {
                        bool userFound = false;
                        for (auto stringUser : socialUserGroupLoaded->SocialUserGroup->UsersTrackedBySocialUserGroup)
                        {
                            if (user == stringUser)
                            {
                                userFound = true;
                            }
                        }

                        VERIFY_IS_TRUE(userFound);
                    }

                    shouldLoop = false;

                    for (auto user : socialUserGroupLoaded->SocialUserGroup->Users)
                    {
                        VERIFY_ARE_EQUAL_STR(user->Gamertag, _T("TestGamerTag"));
                    }
                    break;
                }
            }
        } while (shouldLoop);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests refresh for RTA resync
    DEFINE_TEST_CASE(TestSocialManagerResync)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerResync);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        auto group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        socialManagerInitializationStruct.socialManager->DoWork();
        auto coreUserGroup = group->Users;
        VERIFY_ARE_EQUAL_INT(group->UsersTrackedBySocialUserGroup->Size, group->Users->Size);
        string_t gamertagStr = coreUserGroup->GetAt(0)->Gamertag->Data();
        auto testStr = _T("TestGamerTag");
        VERIFY_IS_TRUE(utils::str_icmp(gamertagStr, testStr) == 0);
        string_t testTag = _T("TagChanged");

        auto peoplehubResponseJson = GenerateInitialPeoplehubJSON();
        auto responseStruct = GetPeoplehubResponseStruct(peoplehubResponseJson, 200);
        peoplehubResponseJson[L"people"][0][L"gamertag"] = web::json::value::string(testTag);

        responseStruct->responseListInternal[0]->set_response_body(peoplehubResponseJson);

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = responseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        m_mockXboxSystemFactory->GetMockWebSocketClient()->recieve_message(rtaResyncMessage);
        VERIFY_IS_TRUE(group->Users->GetAt(0)->PresenceRecord->PresenceTitleRecords->GetAt(0)->PresenceText == L"Home");

        bool shouldLoop = true;
        bool foundProfileChange = false;
        bool foundPresenceChange = false;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::ProfilesChanged)
                {
                    foundProfileChange = true;
                }
                else if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    foundPresenceChange = true;
                    VERIFY_IS_TRUE(group->Users->GetAt(0)->PresenceRecord->PresenceTitleRecords->GetAt(0)->PresenceText->IsEmpty());
                }

                if (foundProfileChange && foundPresenceChange)
                {
                    shouldLoop = false;
                }
            }
        } while (shouldLoop);

        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        group = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );
        socialManagerInitializationStruct.socialManager->DoWork();
        coreUserGroup = group->Users;

        for (auto user : coreUserGroup)
        {
            if (user->Gamertag == _T("0"))
            {
                VERIFY_ARE_EQUAL(user->Gamertag->Data(), _T("TagChanged"));
            }
        }
        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(group);
        group = nullptr;

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests updating a social user group that was already created from list
    DEFINE_TEST_CASE(TestSocialManagerUserUpdate)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUserUpdate);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>({ _T("100001"), _T("100002"), _T("100003") });

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(vec);

        m_mockXboxSystemFactory->add_http_state_response(responses);
        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), vec->GetView());

        TEST_LOG(L"Looking for SocialUserGroupLoaded");
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    shouldLoop = false;
                }
            }
        } while (shouldLoop);
        socialManagerInitializationStruct.socialEvents.clear();
        Platform::Collections::Vector<Platform::String^>^ updateVec = ref new Platform::Collections::Vector<Platform::String^>({ _T("100001"), _T("100002"), _T("100004") });

        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(updateVec);
        m_mockXboxSystemFactory->add_http_state_response(responses);
        socialManagerInitializationStruct.socialManager->UpdateSocialUserGroup(
            socialUserGroup,
            updateVec->GetView()
            );

        shouldLoop = true;
        byte foundEvents = 0;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupUpdated)
                {
                    foundEvents |= 0b001;
                    for (auto user : evt->UsersAffected)
                    {
                        bool wasFound = false;
                        for (auto compareUser : updateVec)
                        {
                            if (compareUser == user)
                            {
                                wasFound = true;
                                break;
                            }
                        }
                        VERIFY_IS_TRUE(wasFound);
                    }
                }
                else if(evt->EventType == SocialEventType::UsersAddedToSocialGraph)
                {
                    foundEvents |= 0b010;
                    for (auto user : evt->UsersAffected)
                    {
                        bool wasFound = false;
                        for (auto compareUser : updateVec)
                        {
                            if (utils::str_icmp(compareUser->Data(), _T("100004")) == 0)
                            {
                                wasFound = true;
                                break;
                            }
                        }
                        VERIFY_IS_TRUE(wasFound);
                    }
                }
                else if (evt->EventType == SocialEventType::UsersRemovedFromSocialGraph)
                {
                    foundEvents |= 0b100;
                    for (auto user : evt->UsersAffected)
                    {
                        bool wasFound = true;
                        for (auto compareUser : updateVec)
                        {
                            if (utils::str_icmp(compareUser->Data(), _T("100003")) == 0)
                            {
                                wasFound = false;
                                break;
                            }
                        }
                        VERIFY_IS_TRUE(wasFound);
                    }
                }
            }
        } while (foundEvents != 0b111);
        for (auto user : socialUserGroup->Users)
        {
            bool wasFound = false;
            for (auto compareUser : updateVec)
            {
                if (compareUser == user->XboxUserId)
                {
                    wasFound = true;
                    break;
                }
            }

            VERIFY_IS_TRUE(wasFound);
        }
        for (auto user : socialUserGroup->UsersTrackedBySocialUserGroup)
        {
            bool wasFound = false;
            for (auto compareUser : updateVec)
            {
                if (compareUser == user)
                {
                    wasFound = true;
                    break;
                }
            }

            VERIFY_IS_TRUE(wasFound);
        }
        socialManagerInitializationStruct.socialEvents.clear();
        Platform::Collections::Vector<Platform::String^>^ updateVecSingle = ref new Platform::Collections::Vector<Platform::String^>({ _T("100004") });
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(updateVecSingle);
        m_mockXboxSystemFactory->add_http_state_response(responses);
        socialManagerInitializationStruct.socialManager->UpdateSocialUserGroup(
            socialUserGroup,
            updateVecSingle->GetView()
            );

        shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupUpdated)
                {
                    shouldLoop = false;
                    for (auto user : evt->UsersAffected)
                    {
                        bool wasFound = false;
                        for (auto compareUser : updateVec)
                        {
                            if (compareUser == user)
                            {
                                wasFound = true;
                            }
                        }
                        VERIFY_IS_TRUE(wasFound);
                    }
                }
            }
        } while (shouldLoop);
        VERIFY_IS_TRUE(socialUserGroup->Users->Size == 1);
        VERIFY_IS_TRUE(socialUserGroup->Users->GetAt(0)->XboxUserId == updateVecSingle->GetAt(0));

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSocialManagerErrorWithInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerErrorWithInvalidArgs);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        Platform::Collections::Vector<Platform::String^>^ socialUserList = ref new Platform::Collections::Vector<Platform::String^>({ _T("A"), _T("B"), _T("C") });
        Platform::Collections::Vector<Platform::String^>^ socialUserListLarge = ref new Platform::Collections::Vector<Platform::String^>();
        for (int i = 0; i < 101; ++i)
        {
            stringstream_t str;
            str << i;

            socialUserListLarge->Append(ref new Platform::String(str.str().c_str()));
        }
        Platform::Collections::Vector<Platform::String^>^ socialUserListEmpty = ref new Platform::Collections::Vector<Platform::String^>();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->AddLocalUser(nullptr, SocialManagerExtraDetailLevel::PreferredColorLevel), E_INVALIDARG);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->RemoveLocalUser(nullptr), E_INVALIDARG);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(nullptr, PresenceFilter::All, RelationshipFilter::Favorite), E_INVALIDARG);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(nullptr, socialUserList->GetView()), E_INVALIDARG);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), nullptr), E_INVALIDARG);
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), socialUserListEmpty->GetView()), E_INVALIDARG);
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), socialUserListLarge->GetView()), E_INVALIDARG);
        auto groupList = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), socialUserList->GetView());
        auto groupFilter = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(xboxLiveContext->user(), PresenceFilter::All, RelationshipFilter::Friends);
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->UpdateSocialUserGroup(nullptr, socialUserList->GetView()), E_INVALIDARG);
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->UpdateSocialUserGroup(groupList, socialUserListLarge->GetView()), E_INVALIDARG);
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->UpdateSocialUserGroup(groupFilter, socialUserListLarge->GetView()), E_INVALIDARG);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(nullptr), E_INVALIDARG);

        socialManagerInitializationStruct.socialManager->RemoveLocalUser(xboxLiveContext->user());
        VERIFY_IS_TRUE(socialManagerInitializationStruct.socialManager->LocalUsers->Size == 0);
    }

    DEFINE_TEST_CASE(TestSocialManagerErrorImproperCallingOrder)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerErrorImproperCallingOrder);
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        Platform::Collections::Vector<Platform::String^>^ socialUserList = ref new Platform::Collections::Vector<Platform::String^>({ _T("A"), _T("B"), _T("C") });

        VERIFY_THROWS_HR_CX(SocialManager::SingletonInstance->CreateSocialUserGroupFromFilters(xboxLiveContext->user(), PresenceFilter::All, RelationshipFilter::Favorite), E_UNEXPECTED);
        VERIFY_THROWS_HR_CX(SocialManager::SingletonInstance->CreateSocialUserGroupFromList(xboxLiveContext->user(), socialUserList->GetView()), E_UNEXPECTED);

        SocialManager::SingletonInstance->AddLocalUser(xboxLiveContext->user(), SocialManagerExtraDetailLevel::NoExtraDetail);
        VERIFY_THROWS_HR_CX(SocialManager::SingletonInstance->AddLocalUser(xboxLiveContext->user(), SocialManagerExtraDetailLevel::NoExtraDetail), E_UNEXPECTED);
        auto userGroup = SocialManager::SingletonInstance->CreateSocialUserGroupFromFilters(xboxLiveContext->user(), PresenceFilter::All, RelationshipFilter::Friends);
        SocialManager::SingletonInstance->DestroySocialUserGroup(userGroup);
        VERIFY_THROWS_HR_CX(SocialManager::SingletonInstance->DestroySocialUserGroup(userGroup), E_INVALIDARG);

        SocialManager::SingletonInstance->RemoveLocalUser(xboxLiveContext->user());
        VERIFY_THROWS_HR_CX(SocialManager::SingletonInstance->RemoveLocalUser(xboxLiveContext->user()), E_UNEXPECTED);

        SocialManager::SingletonInstance->DoWork();     // clear out any events that may be in the queue
    }

    DEFINE_TEST_CASE(TestSocialManagerHTTPFailure)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerHTTPFailure);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();

        auto socialManagerInitializationStruct = InitializeSocialManagerWithError(xboxLiveContext);

        SocialManager::SingletonInstance->RemoveLocalUser(xboxLiveContext->user());
        socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>({ _T("5001"), _T("5002"), _T("5003") });

        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(vec, 401);
        m_mockXboxSystemFactory->add_http_state_response(responses);
        auto socialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(xboxLiveContext->user(), vec->GetView());
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    shouldLoop = false;
                    // TODO: ?
                    //VERIFY_ARE_EQUAL(evt->ErrorCode, xbox::services::utils::convert_xbox_live_error_code_to_hresult(xbox_live_error_code::http_status_401_unauthorized));
                }
            }
        } while (shouldLoop);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Verifies internal values from internal social groups
    DEFINE_TEST_CASE(TestSocialManagerCreateDestroyGroup)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerCreateDestroyGroup);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        auto socialManager = socialManagerInitializationStruct.socialManager;
        auto socialUserGroupFilter = socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    shouldLoop = false;
                }
            }
        } while (shouldLoop);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManager->GetCppObj());
        socialManagerCppMock->_Log_state();
        string_t xuid = socialManager->LocalUsers->GetAt(0)->XboxUserId->ToString()->Data();
        VERIFY_IS_TRUE(socialManagerCppMock != nullptr);
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().at(xuid).size() == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->local_graphs().size() == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 1);
        socialManager->DestroySocialUserGroup(socialUserGroupFilter);
        socialManagerCppMock->_Log_state();
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().at(xuid).size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 0);
        socialUserGroupFilter = socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );
        shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    shouldLoop = false;
                }
            }
        } while (shouldLoop);
        socialManagerCppMock->_Log_state();
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().at(xuid).size() == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->local_graphs().size() == 1);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 1);
        socialManager->DestroySocialUserGroup(socialUserGroupFilter);
        socialManagerCppMock->_Log_state();
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().at(xuid).size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 1);
        Cleanup(socialManagerInitializationStruct, xboxLiveContext);

        socialManagerCppMock->_Log_state();
        VERIFY_IS_TRUE(socialManagerCppMock->user_to_view_map().size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->local_graphs().size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->xbox_social_user_groups().size() == 0);
        VERIFY_IS_TRUE(socialManagerCppMock->local_user_list().size() == 0);
    }

    void VerifyUserBuffer(user_buffer& userBuffer, size_t userGroupSize)
    {
        auto xboxSocialUserSize = sizeof(xbox_social_user);
        byte* endPos = userBuffer.buffer + xboxSocialUserSize * userGroupSize;
        for (uint32_t i = 0; i < 5; ++i)
        {
            byte* freeData = userBuffer.freeData.front();
            const byte* endData = endPos + i * xboxSocialUserSize;
            VERIFY_IS_TRUE(freeData == endData);
            userBuffer.freeData.pop();
        }

        VERIFY_IS_TRUE(userBuffer.socialUserEventQueue.size() == 0);
        VERIFY_IS_TRUE(userBuffer.socialUserGraph.size() == userGroupSize);
    }

    // Make sure memory is alloced correctly for the user buffer holder internal structure
    DEFINE_TEST_CASE(TestSocialManagerUserBufferHolder)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUserBufferHolder);
        auto peopleHubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(peoplehubResponse));


        xsapi_internal_vector<xsapi_internal_string> xuids;
        xuids.push_back("1");

        Event^ callComplete = ref new Event();
        user_buffers_holder userBufferHolder;
        size_t userGroupSize;

        peopleHubService.get_social_graph("TestXboxUserId", social_manager_extra_detail_level::preferred_color_level, xuids, 0,
            [&callComplete, &userBufferHolder, &userGroupSize](xbox_live_result<xsapi_internal_vector<xbox_social_user>> userGroup)
        {
            VERIFY_IS_TRUE(!userGroup.err());

            userBufferHolder.initialize(userGroup.payload());
            userGroupSize = userGroup.payload().size();

            VERIFY_IS_TRUE(&userBufferHolder.user_buffer_a() == userBufferHolder.active_buffer());
            VERIFY_IS_TRUE(&userBufferHolder.user_buffer_b() == userBufferHolder.inactive_buffer());

            callComplete->Set();
        });

        callComplete->Wait();

        VerifyUserBuffer(userBufferHolder.user_buffer_a(), userGroupSize);
        VerifyUserBuffer(userBufferHolder.user_buffer_b(), userGroupSize);
    }

    DEFINE_TEST_CASE(TestSocialManagerUserBufferAddUsersWithNoInit)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUserBufferAddUsersWithNoInit);
        auto peopleHubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(peoplehubResponse));

        xsapi_internal_vector<xsapi_internal_string> xuids;
        xuids.push_back("1");

        Event^ callComplete = ref new Event();
        user_buffers_holder userBufferHolder;
        size_t userGroupSize;

        peopleHubService.get_social_graph("TestXboxUserId", social_manager_extra_detail_level::preferred_color_level, xuids, 0,
            [&callComplete, &userBufferHolder, &userGroupSize](xbox_live_result<xsapi_internal_vector<xbox_social_user>> userGroup)
        {
            VERIFY_IS_TRUE(!userGroup.err());

            userBufferHolder.initialize(xsapi_internal_vector<xbox_social_user>());
            userBufferHolder.add_users_to_buffer(userGroup.payload(), *userBufferHolder.inactive_buffer());
            userBufferHolder.add_users_to_buffer(userGroup.payload(), *userBufferHolder.active_buffer());
            userGroupSize = userGroup.payload().size();

            VERIFY_IS_TRUE(&userBufferHolder.user_buffer_a() == userBufferHolder.active_buffer());
            VERIFY_IS_TRUE(&userBufferHolder.user_buffer_b() == userBufferHolder.inactive_buffer());

            callComplete->Set();

        });

        callComplete->Wait();

        VerifyUserBuffer(userBufferHolder.user_buffer_a(), userGroupSize);
        VerifyUserBuffer(userBufferHolder.user_buffer_b(), userGroupSize);
    }

    DEFINE_TEST_CASE(TestSocialManagerUserBufferAddUsersNoData)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUserBufferAddUsersNoData);
        auto peopleHubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(peoplehubResponse));

        user_buffers_holder userBufferHolder;

        userBufferHolder.initialize(xsapi_internal_vector<xbox_social_user>());
        userBufferHolder.add_users_to_buffer(xsapi_internal_vector<xbox_social_user>(), *userBufferHolder.inactive_buffer());
        userBufferHolder.add_users_to_buffer(xsapi_internal_vector<xbox_social_user>(), *userBufferHolder.active_buffer());

        VERIFY_IS_TRUE(&userBufferHolder.user_buffer_a() == userBufferHolder.active_buffer());
        VERIFY_IS_TRUE(&userBufferHolder.user_buffer_b() == userBufferHolder.inactive_buffer());

        VERIFY_IS_TRUE(userBufferHolder.user_buffer_a().socialUserGraph.size() == 0);
        VERIFY_IS_TRUE(userBufferHolder.user_buffer_b().socialUserGraph.size() == 0);

        VERIFY_IS_TRUE(userBufferHolder.user_buffer_a().freeData.size() == 0);
        VERIFY_IS_TRUE(userBufferHolder.user_buffer_b().freeData.size() == 0);
    }

    // Verifies that get_user_copy API (C++ only) works properly in copying the data
    DEFINE_TEST_CASE(TestSocialManagerUserGroupCopy)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerUserGroupCopy);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        auto socialManager = socialManagerInitializationStruct.socialManager;
        auto socialUserGroupFilter = socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    shouldLoop = false;
                }
            }
        } while (shouldLoop);

        auto cppObj = socialUserGroupFilter->GetCppObj();
        std::vector<xbox_social_user> users;
        auto result = cppObj->get_copy_of_users(users);
        VERIFY_IS_TRUE(!result.err());
        for (auto user : users)
        {
            VERIFY_ARE_EQUAL_STR(L"TestGamerTag", user.display_name());
            VERIFY_ARE_EQUAL_STR(L"http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user.display_pic_url_raw());
            VERIFY_IS_TRUE(user.is_followed_by_caller());
            VERIFY_IS_TRUE(user.is_following_user());
            VERIFY_ARE_EQUAL_STR(L"9001", user.gamerscore());
            VERIFY_ARE_EQUAL_STR(L"TestGamerTag", user.gamertag());
            VERIFY_IS_FALSE(user.is_favorite());
            VERIFY_ARE_EQUAL_STR(L"193e91", user.preferred_color().primary_color());
            VERIFY_ARE_EQUAL_STR(L"2458cf", user.preferred_color().secondary_color());
            VERIFY_ARE_EQUAL_STR(L"122e6b", user.preferred_color().tertiary_color());
            VERIFY_IS_TRUE(user.presence_record().presence_title_records().size() == 1);
            VERIFY_IS_FALSE(user.use_avatar());
            VERIFY_IS_TRUE(user.title_history().has_user_played());
            auto str = user.title_history().last_time_user_played().to_string(utility::datetime::date_format::ISO_8601);
            VERIFY_IS_TRUE(utils::str_icmp(str, L"2015-01-26T22:54:54.663Z") == 0);
        }

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Quickly creating and destroying a social user group from list
    DEFINE_TEST_CASE(TestSocialManagerAddRemoveUsersFromList)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerAddRemoveUsersFromList);

        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManagerInitializationStruct.socialManager->GetCppObj());
        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>({ _T("100001"), _T("100002"), _T("100003") });


        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = CreateSocialGroupFromListResponse(vec);

        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto userGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromList(
            xboxLiveContext->user(),
            vec->GetView()
        );

        socialManagerInitializationStruct.socialManager->DestroySocialUserGroup(userGroup);

        bool usersRemovedFound = false;
        bool usersAddedFound = false;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::UsersRemovedFromSocialGraph)
                {
                    usersRemovedFound = true;
                }
                else if (evt->EventType == SocialEventType::SocialUserGroupLoaded)
                {
                    usersAddedFound = true;
                }
            }
        } while (!(usersRemovedFound && usersAddedFound));

        {
            auto localGraphs = socialManagerCppMock->local_graphs();
            auto localGraph = localGraphs[_T("TestXboxUserId")];
            xsapi_internal_vector<std::shared_ptr<social_event_internal>> socialEvents;
            auto changeStruct = localGraph->do_work(socialEvents);
            VERIFY_IS_TRUE(changeStruct.socialUsers->size() == 99);
        }

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Verifies that changes recieved during initialization are handled properly
    DEFINE_TEST_CASE(TestSocialManagerMessageDuringInitialization)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerMessageDuringInitialization);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;

        auto peoplehubResponseJson = GenerateInitialPeoplehubJSON();

        auto peoplehubResponseStruct = GetPeoplehubResponseStruct(peoplehubResponseJson);

        // set up http response set
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = peoplehubResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        auto socialManager = SocialManager::SingletonInstance;
        socialManager->LogState();
        socialManager->AddLocalUser(xboxLiveContext->user(), SocialManagerExtraDetailLevel::NoExtraDetail);
        std::vector<Platform::String^> userList = { "10001" };
        pplx::task_completion_event<void> tce;
        TestSocialGraphChange(userList, tce);
        create_task(tce).wait();
        std::vector<SocialEvent^> socialEvents;

        TEST_LOG(L"Calling socialManager->AddLocalUser");
        TEST_LOG(L"Looking for LocalUserAdded");
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            for (auto evt : changeList)
            {
                socialEvents.push_back(evt);

                if (evt->EventType == SocialEventType::LocalUserAdded)
                {
                    TEST_LOG(L"Found LocalUserAdded");
                    shouldLoop = false;
                    break;
                }
            }
        } while (shouldLoop);
        
        SocialManagerInitializationStruct initStruct;
        initStruct.socialManager = socialManager;
        Cleanup(initStruct, xboxLiveContext);
    }

    // Verifies that social manager will reconnect after an RTA disconnect is hit
    DEFINE_TEST_CASE(TestSocialManagerReinitializeRTA)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerReinitializeRTA);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, false);
        auto socialManagerCppMock = std::dynamic_pointer_cast<MockSocialManager>(socialManagerInitializationStruct.socialManager->GetCppObj());
        socialManagerCppMock->local_graphs().begin()->second->internal_xbox_live_context_impl()->settings()->set_websocket_timeout_window(std::chrono::seconds(1));
        pplx::task_completion_event<void> disconnectTce;
        pplx::task_completion_event<void> reconnectedTce;
        for (auto& graph : socialManagerCppMock->local_graphs())
        {
            graph.second->add_state_handler(
            [&disconnectTce, &reconnectedTce](real_time_activity_connection_state state)
            {
                static bool wasDisconnected = false;
                if (state == real_time_activity_connection_state::disconnected)
                {
                    wasDisconnected = true;
                    disconnectTce.set();
                }
                else if (state == real_time_activity_connection_state::connected && wasDisconnected)
                {
                    reconnectedTce.set();
                }
            });
        }

        for (auto& ws : m_mockXboxSystemFactory->GetMockWebSocketClients())
        {
            ws->m_connectToFail = true;
            ws->m_closeStatus = HCWebSocketCloseStatus_AbnormalClose;
            ws->close();
        }

        pplx::create_task(disconnectTce).wait();

        for (auto& ws : m_mockXboxSystemFactory->GetMockWebSocketClients())
        {
            ws->m_connectToFail = false;
        }

        pplx::create_task(reconnectedTce).wait();

        pplx::task_completion_event<void> tce;
        InitializeSubscriptions(USER_LIST, tce);
        create_task(tce).wait();
        socialManagerInitializationStruct.socialEvents.clear();

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    // Tests all possible cases of filtering working properly
    DEFINE_TEST_CASE(TestSocialManagerFilters)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerFilters);

        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        auto socialUserGroupAll = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        // test title online friends
        VERIFY_IS_TRUE(socialUserGroupAll->Users->Size == USER_LIST.size());

        auto titleOnlineSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::TitleOnline,
            RelationshipFilter::Friends
            );

        socialManagerInitializationStruct.socialManager->DoWork();

        VERIFY_IS_TRUE(titleOnlineSocialUserGroup->Users->Size == USER_LIST.size());
        for (auto user : titleOnlineSocialUserGroup->Users)
        {
            VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
        }

        // test title online favorites
        auto favoriteTitleOnlineSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::TitleOnline,
            RelationshipFilter::Favorite
            );

        socialManagerInitializationStruct.socialManager->DoWork();
        VERIFY_IS_TRUE(favoriteTitleOnlineSocialUserGroup->Users->Size == 0);

        // update to have one favorite in graph
        web::json::value jsonArray = web::json::value::array();
        auto blob = defaultPeoplehubTemplate;
        blob[L"xuid"] = web::json::value::string(_T("1"));
        blob[L"isFavorite"] = web::json::value::boolean(true);
        jsonArray[0] = blob;
        web::json::value fullJSON;
        fullJSON[L"people"] = jsonArray;


        auto peopleHubResponse = StockMocks::CreateMockHttpCallResponseInternal(fullJSON);
        std::shared_ptr<HttpResponseStruct> peopleHubResponseStruct = std::make_shared<HttpResponseStruct>();
        peopleHubResponseStruct->responseListInternal = { peopleHubResponse };
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        // set up http response set
        responses[_T("https://peoplehub.mockenv.xboxlive.com")] = peopleHubResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);

        // update friend to favorite
        pplx::task_completion_event<void> tce;
        std::queue<WebsocketMockResponse> responseQueue;
        responseQueue.push({ L"http://social.xboxlive.com/users/xuid(TestXboxUserId)/friends", socialRelationshipChangedMessage, real_time_activity_message_type::change_event, false });
        m_mockXboxSystemFactory->add_websocket_state_responses_to_all_clients(responseQueue, tce);
        create_task(tce).wait();

        socialManagerInitializationStruct.socialEvents.clear();
        bool shouldLoop = true;
        do
        {
            auto changeList = socialManagerInitializationStruct.socialManager->DoWork();
            LogSocialManagerEvents(changeList);
            AppendToPendingEvents(changeList, socialManagerInitializationStruct);

            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::ProfilesChanged)
                {
                    VERIFY_IS_TRUE(evt->UsersAffected->Size == 1);
                    VERIFY_IS_TRUE(evt->UsersAffected->GetAt(0) == L"1");
                    shouldLoop = false;
                    break;
                }
            }
        } while (shouldLoop);

        VERIFY_IS_TRUE(favoriteTitleOnlineSocialUserGroup->Users->Size == 1);

        // all offline social user group
        auto offlineSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::AllOffline,
            RelationshipFilter::Friends
            );

        VERIFY_IS_TRUE(offlineSocialUserGroup->Users->Size == 0);

        socialManagerInitializationStruct.socialEvents.clear();
        tce = pplx::task_completion_event<void>();
        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessageEvent).serialize());
        create_task(tce).wait();

        size_t totalSize = 0;
        do
        {
            totalSize = 0;
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    totalSize += evt->UsersAffected->Size;
                    for (auto userStr : evt->UsersAffected)
                    {
                        for (auto user : socialUserGroupAll->Users)
                        {
                            if (utils::str_icmp(user->XboxUserId->Data(), userStr->Data()) == 0)
                            {
                                VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Offline);
                                break;
                            }
                        }
                    }
                }
            }
        } while (totalSize != USER_LIST.size());

        VERIFY_IS_TRUE(offlineSocialUserGroup->Users->Size == USER_LIST.size());
        VERIFY_IS_TRUE(favoriteTitleOnlineSocialUserGroup->Users->Size == 0 && titleOnlineSocialUserGroup->Users->Size == 0);

        // title offline social user group
        auto titleOfflineSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::TitleOffline,
            RelationshipFilter::Friends
            );

        socialManagerInitializationStruct.socialManager->DoWork();
        VERIFY_IS_TRUE(titleOfflineSocialUserGroup->Users->Size == USER_LIST.size());
        VERIFY_IS_TRUE(favoriteTitleOnlineSocialUserGroup->Users->Size == 0);
        // all online social user group
        auto allOnlineSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::AllOnline,
            RelationshipFilter::Friends
            );

        SetDevicePresenceHTTPMock();
        socialManagerInitializationStruct.socialManager->DoWork();
        VERIFY_IS_TRUE(allOnlineSocialUserGroup->Users->Size == 0);
        TestDevicePresenceChange(USER_LIST, tce, web::json::value::string(devicePresenceRtaMessagePCEvent).serialize());
        create_task(tce).wait();

        socialManagerInitializationStruct.socialEvents.clear();
        totalSize = 0;
        do
        {
            totalSize = 0;
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    totalSize += evt->UsersAffected->Size;
                    for (auto userStr : evt->UsersAffected)
                    {
                        for (auto user : socialUserGroupAll->Users)
                        {
                            if (utils::str_icmp(user->XboxUserId->Data(), userStr->Data()) == 0)
                            {
                                VERIFY_IS_TRUE(user->PresenceRecord->UserState == UserPresenceState::Online);
                                break;
                            }
                        }
                    }
                }
            }
        } while (totalSize != USER_LIST.size());

        VERIFY_IS_TRUE(allOnlineSocialUserGroup->Users->Size == USER_LIST.size());
        VERIFY_IS_TRUE(titleOfflineSocialUserGroup->Users->Size == 0);
        VERIFY_IS_TRUE(favoriteTitleOnlineSocialUserGroup->Users->Size == 1);
        VERIFY_IS_TRUE(offlineSocialUserGroup->Users->Size == 0);

        auto allTitleSocialUserGroup = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::AllTitle,
            RelationshipFilter::Friends
            );

        VERIFY_IS_TRUE(allTitleSocialUserGroup->Users->Size == USER_LIST.size());

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSocialManagerGetUsersFromXuidList)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerGetUsersFromXuidList);

        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);

        auto socialUserGroupAll = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
            );

        socialManagerInitializationStruct.socialManager->DoWork();

        Platform::Collections::Vector<Platform::String^>^ vec = ref new Platform::Collections::Vector<Platform::String^>({ _T("1"), _T("2"), _T("3") });
        auto users = socialUserGroupAll->GetUsersFromXboxUserIds(vec->GetView());
        VERIFY_IS_TRUE(vec->Size == users->Size);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }

    DEFINE_TEST_CASE(TestSocialManagerRichPresencePolling)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestSocialManagerRichPresencePolling);
        m_mockXboxSystemFactory->reinit();
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto socialManagerInitializationStruct = Initialize(xboxLiveContext, true);
        auto socialUserGroupAll = socialManagerInitializationStruct.socialManager->CreateSocialUserGroupFromFilters(
            xboxLiveContext->user(),
            PresenceFilter::All,
            RelationshipFilter::Friends
        );

        auto presenceJSON = GenerateInitialPresenceJSON(false);
        auto presenceResponse = StockMocks::CreateMockHttpCallResponseInternal(presenceJSON);
        std::shared_ptr<HttpResponseStruct> presenceResponseStruct = std::make_shared<HttpResponseStruct>();
        presenceResponseStruct->responseListInternal = { presenceResponse };
        std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> responses;
        // set up http response set
        responses[_T("https://userpresence.mockenv.xboxlive.com")] = presenceResponseStruct;

        m_mockXboxSystemFactory->add_http_state_response(responses);
        VERIFY_IS_TRUE(socialUserGroupAll->Users->GetAt(0)->PresenceRecord->UserState == UserPresenceState::Online);

        socialManagerInitializationStruct.socialManager->SetRichPresencePollingState(xboxLiveContext->user(), true);
        while (true)
        {
            auto userCount = 0;
            AppendToPendingEvents(socialManagerInitializationStruct.socialManager->DoWork(), socialManagerInitializationStruct);
            for (auto evt : socialManagerInitializationStruct.socialEvents)
            {
                if (evt->EventType == SocialEventType::PresenceChanged)
                {
                    userCount += evt->UsersAffected->Size;
                }
            }

            if (userCount == USER_LIST.size())
            {
                break;
            }
        }

        VERIFY_IS_TRUE(socialUserGroupAll->Users->GetAt(0)->PresenceRecord->UserState == UserPresenceState::Offline);

        socialManagerInitializationStruct.socialManager->SetRichPresencePollingState(xboxLiveContext->user(), false);

        presenceJSON = GenerateInitialPresenceJSON(true);
        presenceResponse = StockMocks::CreateMockHttpCallResponseInternal(presenceJSON);
        presenceResponseStruct = std::make_shared<HttpResponseStruct>();
        presenceResponseStruct->responseListInternal = { presenceResponse };
        // set up http response set
        responses[_T("https://userpresence.mockenv.xboxlive.com")] = presenceResponseStruct;

        socialManagerInitializationStruct.socialManager->DoWork();
        VERIFY_IS_TRUE(socialUserGroupAll->Users->GetAt(0)->PresenceRecord->UserState == UserPresenceState::Offline);

        Cleanup(socialManagerInitializationStruct, xboxLiveContext);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END