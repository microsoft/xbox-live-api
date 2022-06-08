// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

#pragma warning(disable:4996)

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* titlePresenceEndedResponse = "\"Ended\"";
const char* devicePresenceResponse = "\"MoLive:false\"";

const char* richPresenceResponse = R"(
{
    "devicetype": "WindowsOneCore",
    "titleid": 85494077,
    "string1": "Microsoft Solitaire Collection",
    "string2": "Menus"
}
)";

const char* setPresenceRequest = R"(
{
    "state": "active",
    "activity": {
        "richPresence": {
            "scid": "12345675467",
            "id": "20"
        }
    }
}
)";

const char* getPresenceForMultipleUsersRequest = R"(
{
    "users": [
        "12345",
        "56789"
    ],
    "onlineOnly": false,
    "broadcastingOnly": false
}
)";

const char* getPresenceForMultipleUsersOverloadRequest= R"(
{
    "users": [
        "12345",
        "56789"
    ],
    "deviceTypes": [
        "PC",
        "MoLive"
    ],
    "titles": [
        "0",
        "1"
    ],
    "level": "all",
    "onlineOnly": true,
    "broadcastingOnly": true
}
)";

const char* getPresenceForSocialGroupOverloadRequest = R"(
{
    "group": "testGroup",
    "groupXuid": "12345",
    "deviceTypes": [
        "PC",
        "MoLive"
    ],
    "titles": [
        "0",
        "1"
    ],
    "level": "all",
    "onlineOnly": true,
    "broadcastingOnly": false
}
)";

const char* defaultPresenceResponse = R"(
{
    "xuid": "2814671404555632",
    "state": "Online",
    "devices": [
        {
            "type": "PC",
            "titles": [
                {
                    "id": "1563044810",
                    "name": "DefaultTitle",
                    "placement": "Full",
                    "state": "Active",
                    "lastModified": "2015-01-21T23:19:21Z",
                    "activity": {
                        "richPresence": "yes",
                        "broadcast": {
                            "id": "12345",
                            "session": "test0",
                            "provider": "twitch",
                            "viewers": 10,
                            "started": "2013-02-01T16:00:00Z"
                        }
                    }
                }
            ]
        }
    ]
}
)";

const char* defaultMultiplePresenceResponse = R"(
[
    {
        "xuid": "12345",
        "state": "Online",
        "devices": [
            {
                "type": "XboxOne",
                "titles": [
                    {
                        "id": "99467",
                        "name": "awesomeGame",
                        "lastModified": "2013-01-31T16:00:00Z",
                        "state": "active",
                        "placement": "Full",
                        "activity": {
                            "richPresence": "yes",
                            "broadcast": {
                                "id": "12345",
                                "session": "test0",
                                "provider": "twitch",
                                "viewers": 10,
                                "started": "2013-01-31T16:00:00Z"
                            }
                        }
                    }
                ]
            }
        ]
    },
    {
        "xuid": "56789",
        "state": "Online",
        "devices": [
            {
                "type": "PC",
                "titles": [
                    {
                        "id": "99467",
                        "name": "awesomeGame",
                        "lastModified": "2013-01-31T16:00:00Z",
                        "state": "active",
                        "placement": "Full",
                        "activity": {
                            "richPresence": "yes",
                            "broadcast": {
                                "id": "12345",
                                "session": "test0",
                                "provider": "twitch",
                                "viewers": 10,
                                "started": "2013-01-31T16:00:00Z"
                            }
                        }
                    }
                ]
            }
        ]
    }
]
)";

DEFINE_TEST_CLASS(PresenceTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PresenceTests);

    void VerifyBroadcastRecord(XblPresenceBroadcastRecord* broadcastRecord, JsonValue resultToVerify)
    {
        const char* provider = broadcastRecord->provider == XblPresenceBroadcastProvider::Twitch ? "twitch" : "unknown";
        xsapi_internal_string resultStr = resultToVerify["started"].GetString();
        xbox::services::datetime resultDatetime = xbox::services::datetime::from_string(resultStr, xbox::services::datetime::date_format::ISO_8601);

        VERIFY_ARE_EQUAL_STR(provider, resultToVerify["provider"].GetString());
        VERIFY_ARE_EQUAL_STR(broadcastRecord->broadcastId, resultToVerify["id"].GetString());
        VERIFY_ARE_EQUAL_STR(broadcastRecord->session, resultToVerify["session"].GetString());
        VERIFY_ARE_EQUAL_INT(broadcastRecord->viewerCount, resultToVerify["viewers"].GetInt());
        VERIFY_ARE_EQUAL_INT(broadcastRecord->startTime, utils::TimeTFromDatetime(resultDatetime));
    }

    void VerifyPresenceTitleRecord(XblPresenceTitleRecord* record, JsonValue resultToVerify)
    {
        xsapi_internal_string resultStr = resultToVerify["lastModified"].GetString();
        xbox::services::datetime resultDatetime = xbox::services::datetime::from_string(resultStr, xbox::services::datetime::date_format::ISO_8601);
        xsapi_internal_string expectedStr = utils::DatetimeFromTimeT(record->lastModified).to_string(xbox::services::datetime::date_format::ISO_8601);
        UNREFERENCED_PARAMETER(expectedStr);
        VERIFY_ARE_EQUAL_INT(record->titleId, std::stoi(resultToVerify["id"].GetString()));
        VERIFY_ARE_EQUAL_STR(record->titleName, resultToVerify["name"].GetString());
        VERIFY_ARE_EQUAL_INT(record->lastModified, utils::TimeTFromDatetime(resultDatetime));
        
        const char* stateString = resultToVerify["state"].GetString();
        if (utils::str_icmp(stateString, "active") == 0)
        {
            VERIFY_IS_TRUE(record->titleActive);
        }
        else
        {
            VERIFY_IS_FALSE(record->titleActive);
        }

        const char* recordPlacement{ nullptr };
        switch (record->viewState)
        {
            case XblPresenceTitleViewState::FullScreen:
                recordPlacement = "Full";
                break;
            case XblPresenceTitleViewState::Filled:
                recordPlacement = "Fill";
                break;
            case XblPresenceTitleViewState::Snapped:
                recordPlacement = "Snapped";
                break;
            case XblPresenceTitleViewState::Background:
                recordPlacement = "Background";
                break;
            default:
                recordPlacement = "Unknown";
        }

        VERIFY_ARE_EQUAL_STR(recordPlacement, resultToVerify["placement"].GetString());
        VERIFY_ARE_EQUAL_STR(record->richPresenceString, resultToVerify["activity"]["richPresence"].GetString());

        VerifyBroadcastRecord(record->broadcastRecord, resultToVerify["activity"]["broadcast"].GetObjectW());
    }

    void VerifyAndClosePresenceRecord(XblPresenceRecord* record, JsonValue resultToVerify)
    {
        const char* userStateStr{ nullptr };
        switch (record->UserState())
        {
            case XblPresenceUserState::Online:
                userStateStr = "Online";
                break;
            case XblPresenceUserState::Away:
                userStateStr = "Away";
                break;
            case XblPresenceUserState::Offline:
                userStateStr = "Offline";
                break;
            case XblPresenceUserState::Unknown:
            default:
                userStateStr = "Unknown";
                break;
        }

        VERIFY_ARE_EQUAL_UINT(record->Xuid(), atoll(resultToVerify["xuid"].GetString()));
        VERIFY_ARE_EQUAL_STR(userStateStr, resultToVerify["state"].GetString());
        
        uint32_t deviceCounter = 0;
        for (auto& device : resultToVerify["devices"].GetArray())
        {
            auto deviceRecord = record->DeviceRecords()[deviceCounter];
            
            const char* deviceTypeStr{ nullptr };
            switch (deviceRecord.deviceType)
            {
                case XblPresenceDeviceType::WindowsPhone:
                    deviceTypeStr = "WindowsPhone";
                    break;
                case XblPresenceDeviceType::WindowsPhone7:
                    deviceTypeStr = "WindowsPhone7";
                    break;
                case XblPresenceDeviceType::Web:
                    deviceTypeStr = "Web";
                    break;
                case XblPresenceDeviceType::Xbox360:
                    deviceTypeStr = "Xbox360";
                    break;
                case XblPresenceDeviceType::PC:
                    deviceTypeStr = "PC";
                    break;
                case XblPresenceDeviceType::Windows8:
                    deviceTypeStr = "Windows8";
                    break;
                case XblPresenceDeviceType::XboxOne:
                    deviceTypeStr = "XboxOne";
                    break;
                case XblPresenceDeviceType::WindowsOneCore:
                    deviceTypeStr = "WindowsOneCore";
                    break;
                case XblPresenceDeviceType::WindowsOneCoreMobile:
                    deviceTypeStr = "WindowsOneCoreMobile";
                    break;
                case XblPresenceDeviceType::Unknown:
                default:
                    deviceTypeStr = "Unknown";
                    break;
            }

            VERIFY_ARE_EQUAL_STR(deviceTypeStr, device["type"].GetString());

            uint32_t titleCounter = 0;
            for (auto& titleValue : device["titles"].GetArray())
            {
                auto presenceTitleRecord = deviceRecord.titleRecords[titleCounter];
                VerifyPresenceTitleRecord(&presenceTitleRecord, titleValue.GetObjectW());
                ++titleCounter;
            }
            ++deviceCounter;
        }

        XblPresenceRecordCloseHandle(record);
    }

    void VerifyAndCloseRecords(XblPresenceRecordHandle* records)
    {
        uint32_t counter{ 0 };
        JsonDocument document;
        document.Parse(defaultMultiplePresenceResponse);
        for (auto& result : document.GetArray())
        {
            VerifyAndClosePresenceRecord(records[counter], result.GetObjectW());
            ++counter;
        }
    }

    std::shared_ptr<HttpMock> CreatePresenceMock(
        xsapi_internal_string titleId
    )
    {
        auto presenceMock = std::make_shared<HttpMock>("GET", "https://userpresence.xboxlive.com");

        presenceMock->SetMockMatchedCallback(
            [titleId](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument jsonRequest;
                jsonRequest.Parse(requestBody.c_str());

                JsonValue response;
                response.SetArray();
                JsonDocument defaultPresence;
                defaultPresence.Parse(defaultPresenceResponse);
                uint32_t i{ 0 };
                for (auto& user : jsonRequest["users"].GetArray())
                {
                    response[i] = defaultPresence.GetObjectW();
                    response[i]["devices"][0]["titles"][0]["id"] = rapidjson::StringRef(titleId.c_str());
                    response[i]["xuid"] = user;
                    ++i;
                }

                mock->SetResponseBody(response);
            });

        return presenceMock;
    }

    DEFINE_TEST_CASE(TestSetPresenceAsync)
    {
        TEST_LOG(L"Test starting: TestSetPresenceAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << "https://userpresence.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/devices/current/titles/current";
        auto mock = std::make_shared<HttpMock>("POST", url.str(), 200);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument expectedJson, requestJson;
                expectedJson.Parse(setPresenceRequest);
                requestJson.Parse(requestBody.c_str());
                auto& expectedRich = expectedJson["activity"]["richPresence"];
                auto& requestRich = requestJson["activity"]["richPresence"];

                requestWellFormed &= strcmp(expectedJson["state"].GetString(), requestJson["state"].GetString()) == 0;
                requestWellFormed &= strcmp(expectedRich["scid"].GetString(), requestRich["scid"].GetString()) == 0;
                requestWellFormed &= strcmp(expectedRich["id"].GetString(), requestRich["id"].GetString()) == 0;
            }
        );

        XAsyncBlock async{};
        const char* ids[]{""};
        XblPresenceRichPresenceIds presence{ "12345675467", "20", ids, 1};
        VERIFY_SUCCEEDED(XblPresenceSetPresenceAsync(xboxLiveContext.get(), true, &presence, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestGetPresenceAsync)
    {
        TEST_LOG(L"Test starting: TestGetPresenceAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << "https://userpresence.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")?level=all";
        HttpMock mock("GET", url.str(), 200);
        mock.SetResponseBody(defaultPresenceResponse);

        XAsyncBlock async{};
        XblPresenceRecordHandle recordHandle{};
        VERIFY_SUCCEEDED(XblPresenceGetPresenceAsync(xboxLiveContext.get(), xboxLiveContext->Xuid(), &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceResult(&async, &recordHandle));
        
        JsonDocument document;
        document.Parse(defaultPresenceResponse);
        VerifyAndClosePresenceRecord(recordHandle, document.GetObjectW());
    }

    DEFINE_TEST_CASE(TestGetPresenceForMultipleUsersAsync)
    {
        TEST_LOG(L"Test starting: TestGetPresenceForMultipleUsersAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_string url{ "https://userpresence.xboxlive.com/users/batch" };
        auto mock = std::make_shared<HttpMock>("GET", url, 200);
        mock->SetResponseBody(defaultMultiplePresenceResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument expectedJson, requestJson;
                expectedJson.Parse(getPresenceForMultipleUsersRequest);
                requestJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedJson["onlineOnly"].GetBool() == expectedJson["onlineOnly"].GetBool();
                requestWellFormed &= expectedJson["broadcastingOnly"].GetBool() == expectedJson["broadcastingOnly"].GetBool();

                auto counter{ 0 };
                auto requestXuids = requestJson["users"].GetArray();
                for (auto& xuid : expectedJson["users"].GetArray())
                {
                    requestWellFormed &= strcmp(xuid.GetString(), requestXuids[counter].GetString()) == 0;
                    ++counter;
                }
            }
        );

        XAsyncBlock async{};
        uint64_t xuids[]{ 12345, 56789 };
        size_t recordCount{};
        XblPresenceRecordHandle recordHandles[2];
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersAsync(xboxLiveContext.get(), xuids, 2, nullptr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersResultCount(&async, &recordCount));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersResult(&async, recordHandles, recordCount));

        VerifyAndCloseRecords(recordHandles);
    }

    DEFINE_TEST_CASE(TestGetPresenceForMultipleUsersOverloadAsync)
    {
        TEST_LOG(L"Test starting: TestGetPresenceForMultipleUsersOverloadAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_string url{ "https://userpresence.xboxlive.com/users/batch" };
        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultMultiplePresenceResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument expectedJson, requestJson;
                expectedJson.Parse(getPresenceForMultipleUsersRequest);
                requestJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedJson["onlineOnly"].GetBool() == requestJson["onlineOnly"].GetBool();
                requestWellFormed &= expectedJson["broadcastingOnly"].GetBool() == requestJson["broadcastingOnly"].GetBool();

                auto counter{ 0 };
                auto requestXuids = requestJson["users"].GetArray();
                for (auto& xuid : expectedJson["users"].GetArray())
                {
                    requestWellFormed &= strcmp(xuid.GetString(), requestXuids[counter].GetString()) == 0;
                    ++counter;
                }
            }
        );
        
        XAsyncBlock async{};
        uint64_t xuids[]{ 12345, 56789 };
        uint64_t titles[]{ 0, 1 };
        size_t recordCount{};
        XblPresenceRecordHandle recordHandles[2];
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersAsync(xboxLiveContext.get(), xuids, 2, nullptr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersResultCount(&async, &recordCount));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForMultipleUsersResult(&async, recordHandles, recordCount));

        VerifyAndCloseRecords(recordHandles);
    }

    DEFINE_TEST_CASE(TestGetPresenceForSocialGroupAsync)
    {
        TEST_LOG(L"Test starting: TestGetPresenceForSocialGroupAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << "https://userpresence.xboxlive.com/users/batch";
        HttpMock mock("GET", url.str(), 200);
        mock.SetResponseBody(defaultMultiplePresenceResponse);

        XAsyncBlock async{};
        uint64_t ownerXuid = 12345;
        size_t recordCount{};
        XblPresenceRecordHandle recordHandles[2];
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupAsync(xboxLiveContext.get(), "testGroup", &ownerXuid, nullptr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupResultCount(&async, &recordCount));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupResult(&async, recordHandles, recordCount));

        VerifyAndCloseRecords(recordHandles);
    }

    DEFINE_TEST_CASE(TestGetPresenceForSocialGroupOverloadAsync)
    {
        TEST_LOG(L"Test starting: TestGetPresenceForSocialGroupOverloadAsync");
        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_string url{ "https://userpresence.xboxlive.com/users/batch" };
        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultMultiplePresenceResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument expectedJson, requestJson;
                expectedJson.Parse(getPresenceForMultipleUsersRequest);
                requestJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedJson["onlineOnly"].GetBool() == requestJson["onlineOnly"].GetBool();
                requestWellFormed &= expectedJson["broadcastingOnly"].GetBool() == requestJson["broadcastingOnly"].GetBool();
            }
        );

        XAsyncBlock async{};
        uint64_t ownerXuid = 12345;
        size_t recordCount{};
        XblPresenceRecordHandle recordHandles[2];
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupAsync(xboxLiveContext.get(), "testGroup", &ownerXuid, nullptr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupResultCount(&async, &recordCount));
        VERIFY_SUCCEEDED(XblPresenceGetPresenceForSocialGroupResult(&async, recordHandles, recordCount));

        VerifyAndCloseRecords(recordHandles);
    }

    struct TitlePresenceChangedHandler
    {
        TitlePresenceChangedHandler(std::shared_ptr<XblContext> xblContext) noexcept
            : m_xblContext{ std::move(xblContext) }
        {
            m_token = XblPresenceAddTitlePresenceChangedHandler(m_xblContext.get(), func, this);
            VERIFY_ARE_NOT_EQUAL(0, m_token);
        };

        ~TitlePresenceChangedHandler() noexcept
        {
            VERIFY_SUCCEEDED(XblPresenceRemoveTitlePresenceChangedHandler(m_xblContext.get(), m_token));
        }

        Event rtaTapReceived;
        uint64_t xuid{ 0 };
        uint32_t titleId{ 0 };
        XblPresenceTitleState state{ XblPresenceTitleState::Unknown };

    private:
        static void CALLBACK func(void* context, uint64_t xuid, uint32_t titleId, XblPresenceTitleState titleState)
        {
            auto pThis{ static_cast<TitlePresenceChangedHandler*>(context) };
            pThis->xuid = xuid;
            pThis->titleId = titleId;
            pThis->state = titleState;
            pThis->rtaTapReceived.Set();
        }
    
        std::shared_ptr<XblContext> m_xblContext;
        XblFunctionContext m_token{ 0 };
    };

    // Bug 39348459: XSAPI Unit Test: TestRTATitlePresence failing
    //DEFINE_TEST_CASE(TestRTATitlePresence)
    //{
    //    TEST_LOG(L"Test starting: TestRTATitlePresence");
    //    TestEnvironment env{};
    //    auto xboxLiveContext = env.CreateMockXboxLiveContext();
    //    auto& mockRtaService{ MockRealTimeActivityService::Instance() };

    //    const uint64_t xuid = 1234;
    //    const uint32_t titleId = 1563044810;
    //    const char titlePresenceUri[]{ "https://userpresence.xboxlive.com/users/xuid(1234)/titles/1563044810" };

    //    mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
    //    {
    //        if (uri == titlePresenceUri)
    //        {
    //            mockRtaService.CompleteSubscribeHandshake(n, defaultPresenceResponse);
    //        }
    //    });

    //    VERIFY_SUCCEEDED(XblPresenceTrackUsers(xboxLiveContext.get(), &xuid, 1));
    //    VERIFY_SUCCEEDED(XblPresenceTrackAdditionalTitles(xboxLiveContext.get(), &titleId, 1));

    //    TitlePresenceChangedHandler handler{ xboxLiveContext };
    //    // Wait for subscription complete before sending change event
    //    handler.rtaTapReceived.Wait();

    //    mockRtaService.RaiseEvent(titlePresenceUri, titlePresenceEndedResponse);
    //    handler.rtaTapReceived.Wait();

    //    VERIFY_ARE_EQUAL_INT(xuid, handler.xuid);
    //    VERIFY_ARE_EQUAL_INT(titleId, handler.titleId);
    //    VERIFY_IS_TRUE(handler.state == XblPresenceTitleState::Ended);
    //}

    struct DevicePresenceChangedHandler
    {
        DevicePresenceChangedHandler(std::shared_ptr<XblContext> xblContext) noexcept
            : m_xblContext{ std::move(xblContext) }
        {
            m_token = XblPresenceAddDevicePresenceChangedHandler(m_xblContext.get(), func, this);
            VERIFY_ARE_NOT_EQUAL(0, m_token);
        };

        ~DevicePresenceChangedHandler() noexcept
        {
            VERIFY_SUCCEEDED(XblPresenceRemoveDevicePresenceChangedHandler(m_xblContext.get(), m_token));
        }

        Event rtaTapReceived;
        uint64_t xuid{ 0 };
        XblPresenceDeviceType deviceType{ XblPresenceDeviceType::Unknown };
        bool isUserLoggedOnDevice{ false };

    private:
        static void CALLBACK func(void* context, uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)
        {
            auto pThis{ static_cast<DevicePresenceChangedHandler*>(context) };
            pThis->xuid = xuid;
            pThis->deviceType = deviceType;
            pThis->isUserLoggedOnDevice = isUserLoggedOnDevice;
            pThis->rtaTapReceived.Set();
        }

        std::shared_ptr<XblContext> m_xblContext;
        XblFunctionContext m_token{ 0 };
    };

    DEFINE_TEST_CASE(TestRTADevicePresence)
    {
        TEST_LOG(L"Test starting: TestRTADevicePresence");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        const uint64_t xuid = 1234;
        const xsapi_internal_string devicePresenceUri{ "https://userpresence.xboxlive.com/users/xuid(1234)/devices" };

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            if (uri == devicePresenceUri)
            {
                mockRtaService.CompleteSubscribeHandshake(n, defaultPresenceResponse);
            }
        });

        VERIFY_SUCCEEDED(XblPresenceTrackUsers(xboxLiveContext.get(), &xuid, 1));

        DevicePresenceChangedHandler handler{ xboxLiveContext };
        // Wait for subscription complete event
        handler.rtaTapReceived.Wait();

        mockRtaService.RaiseEvent(devicePresenceUri, devicePresenceResponse);
        handler.rtaTapReceived.Wait();

        JsonDocument deviceResponseJson{};
        deviceResponseJson.Parse(devicePresenceResponse);

        auto response = utils::string_split_internal(deviceResponseJson.GetString(), ':');
        VERIFY_ARE_EQUAL_INT(xuid, handler.xuid);
        VERIFY_IS_TRUE(xbox::services::presence::DeviceRecord::DeviceTypeFromString(response[0]) == handler.deviceType);
        VERIFY_ARE_EQUAL(response[1] == "true", handler.isUserLoggedOnDevice);
    }

    DEFINE_TEST_CASE(TestSubscriptionManagement)
    {
        TEST_LOG(L"Test starting: TestSubscriptionManagement");

        TestEnvironment env{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        const uint64_t xuid1{ 1 };
        const uint64_t xuid2{ 2 };
        std::map<uint64_t, Event> subAddedEvents;

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRtaService.CompleteSubscribeHandshake(n, defaultPresenceResponse);

            const char xuidPrefix[]{ "xuid(" };
            const char* p{ &uri[uri.find(xuidPrefix) + _countof(xuidPrefix) - 1] };
            uint64_t xuid = strtoull(p, nullptr, 0);
            subAddedEvents[xuid].Set();
        });

        {
            DevicePresenceChangedHandler handler{ xboxLiveContext };

            VERIFY_SUCCEEDED(XblPresenceTrackUsers(xboxLiveContext.get(), &xuid1, 1));
            subAddedEvents[xuid1].Wait();

            VERIFY_SUCCEEDED(XblPresenceTrackUsers(xboxLiveContext.get(), &xuid2, 1));
            subAddedEvents[xuid2].Wait();

            // Subs should be removed along with the handler
        }

        {
            // Subs should be re-added automatically if a handler is re-added
            DevicePresenceChangedHandler handler{ xboxLiveContext };
            subAddedEvents[xuid1].Wait();
            subAddedEvents[xuid2].Wait();
        }
    }

    DEFINE_TEST_CASE(TestLegacySubscriptions)
    {
        TEST_LOG(L"Test starting: TestLegacySubscriptions");

        TestEnvironment env{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        uint32_t subCount{ 0 };
        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRtaService.CompleteSubscribeHandshake(n, defaultPresenceResponse);
            subCount++;
        });

        TitlePresenceChangedHandler titlePresenceHandler{ xboxLiveContext };
        DevicePresenceChangedHandler devicePresenceHandler{ xboxLiveContext };

        XblRealTimeActivitySubscriptionHandle titlePresenceSub{ nullptr };
        VERIFY_SUCCEEDED(XblPresenceSubscribeToTitlePresenceChange(xboxLiveContext.get(), 1, 1, &titlePresenceSub));

        // Both handlers should be invoked since title presence subscription tracks the user and the title
        titlePresenceHandler.rtaTapReceived.Wait();
        devicePresenceHandler.rtaTapReceived.Wait();

        // Since we are already tracking Xuid "1", this shouldn't result in any new subs
        XblRealTimeActivitySubscriptionHandle devicePresenceSub{ nullptr };
        VERIFY_SUCCEEDED(XblPresenceSubscribeToDevicePresenceChange(xboxLiveContext.get(), 1, &devicePresenceSub));

        VERIFY_SUCCEEDED(XblPresenceUnsubscribeFromTitlePresenceChange(xboxLiveContext.get(), titlePresenceSub));

        // Ensure we continue to receive device presence changed notification
        mockRtaService.RaiseEvent("https://userpresence.xboxlive.com/users/xuid(1)/devices", devicePresenceResponse);
        devicePresenceHandler.rtaTapReceived.Wait();

        VERIFY_SUCCEEDED(XblPresenceUnsubscribeFromDevicePresenceChange(xboxLiveContext.get(), devicePresenceSub));
    }

    DEFINE_TEST_CASE(TestPresenceInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestPresenceInvalidArgs");

        TestEnvironment env{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        XAsyncBlock async{};
        uint64_t xuid{};
        const char name{};
        XblPresenceQueryFilters* filters{ nullptr };

        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceAsync(nullptr, 1, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceAsync(xboxLiveContext.get(), 1, nullptr), E_INVALIDARG);

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForMultipleUsersAsync(nullptr, &xuid, 0, filters, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForMultipleUsersAsync(xboxLiveContext.get(), nullptr, 0, filters, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForMultipleUsersAsync(xboxLiveContext.get(), &xuid, 0, filters, nullptr), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForSocialGroupAsync(nullptr, &name, &xuid, filters, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForSocialGroupAsync(xboxLiveContext.get(), nullptr, &xuid, filters, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblPresenceGetPresenceForSocialGroupAsync(xboxLiveContext.get(), &name, &xuid, filters, nullptr), E_INVALIDARG);
    }
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
