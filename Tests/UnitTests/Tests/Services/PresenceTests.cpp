//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "UnitTestIncludes.h"
#define TEST_CLASS_OWNER L"blgross"
#define TEST_CLASS_AREA L"Presence"
#include "UnitTestIncludes.h"

#include "RtaTestHelper.h"

#include "xsapi/presence.h"
#include "presence_internal.h"
#include "xsapi/xbox_live_context.h"

using namespace xbox::services::presence;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Presence;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring titlePresenceStartedResponse = _T("Started");

const std::wstring titlePresenceEndedResponse = _T("Ended");

const std::wstring devicePresenceResponse = _T("MoLive:false");

const std::wstring richPresenceResponse = LR"(
{
    "devicetype": "WindowsOneCore",
    "titleid": 85494077,
    "string1": "Microsoft Solitaire Collection",
    "string2": "Menus"
}
)";

const std::wstring setPresenceRequest = LR"(
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

const std::wstring getPresenceForMultipleUsersRequest = LR"(
{
    "users": [
        "12345",
        "56789"
    ],
    "onlineOnly": false,
    "broadcastingOnly": false
}
)";

const std::wstring getPresenceForMultipleUsersOverloadRequest= LR"(
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

const std::wstring getPresenceForSocialGroupOverloadRequest = LR"(
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

const std::wstring defaultPresenceResponse = LR"(
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
                    "lastModified": "2015-01-21T23:19:21.4740585Z",
                    "activity": {
                        "richPresence": "yes",
                        "broadcast": {
                            "id": "12345",
                            "session": "test0",
                            "provider": "temp",
                            "viewers": 10,
                            "started": "2013-02-01T00:00:00Z"
                        }
                    }
                }
            ]
        }
    ]
}
)";

const std::wstring defaultMultiplePresenceResponse = LR"(
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
                        "lastModified": "2013-02-01T00:00:00Z",
                        "state": "active",
                        "placement": "Full",
                        "activity": {
                            "richPresence": "yes",
                            "broadcast": {
                                "id": "12345",
                                "session": "test0",
                                "provider": "temp",
                                "viewers": 10,
                                "started": "2013-02-01T00:00:00Z"
                            }
                        }
                    }
                ]
            }
        ]
    },
    {
        "xuid": "12345",
        "state": "Online",
        "devices": [
            {
                "type": "PC",
                "titles": [
                    {
                        "id": "99467",
                        "name": "awesomeGame",
                        "lastModified": "2013-02-01T00:00:00Z",
                        "state": "active",
                        "placement": "Full",
                        "activity": {
                            "richPresence": "yes",
                            "broadcast": {
                                "id": "12345",
                                "session": "test0",
                                "provider": "temp",
                                "viewers": 10,
                                "started": "2013-02-01T00:00:00Z"
                            }
                        }
                    }
                ]
            }
        ]
    }
]
)";

// TODO: Need better code coverage
DEFINE_TEST_CLASS(PresenceTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PresenceTests)

    void VerifyBroadcastRecord(PresenceBroadcastRecord^ broadcastRecord, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL(broadcastRecord->BroadcastId->Data(), resultToVerify[L"id"].as_string());
        VERIFY_ARE_EQUAL(broadcastRecord->Session->Data(), resultToVerify[L"session"].as_string());
        VERIFY_ARE_EQUAL(broadcastRecord->Provider->Data(), resultToVerify[L"provider"].as_string());
        VERIFY_ARE_EQUAL_INT(broadcastRecord->ViewerCount, resultToVerify[L"viewers"].as_number().to_uint32());
        VERIFY_ARE_EQUAL(
            DateTimeToString(broadcastRecord->StartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"started"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND)
            );
    }

    void VerifyPresenceTitleRecord(PresenceTitleRecord^ record, web::json::value resultToVerify)
    {
        web::json::value activityJson = utils::extract_json_field(resultToVerify, L"activity", true);

        VERIFY_ARE_EQUAL_INT(record->TitleId, std::stoi(resultToVerify[L"id"].as_string()));
        VERIFY_ARE_EQUAL(record->TitleName->Data(), resultToVerify[L"name"].as_string());

        auto str = DateTimeToString(record->LastModifiedDate);
        VERIFY_ARE_EQUAL(
            DateTimeToString(record->LastModifiedDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"lastModified"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND)
            );

        string_t stateString = resultToVerify[L"state"].as_string();
        if (utils::str_icmp(stateString, L"active") == 0)
        {
            VERIFY_IS_TRUE(record->IsTitleActive);
        }
        else
        {
            VERIFY_IS_FALSE(record->IsTitleActive);
        }

        string_t recordToString;
        switch (record->TitleViewState)
        {
            case PresenceTitleViewState::FullScreen:
                recordToString = L"Full";
                break;
            case PresenceTitleViewState::Filled:
                recordToString = L"Fill";
                break;
            case PresenceTitleViewState::Snapped:
                recordToString = L"Snapped";
                break;
            case PresenceTitleViewState::Background:
                recordToString = L"Background";
                break;
            default:
                recordToString = L"Unknown";
        }

        string_t resultToString = resultToVerify[L"placement"].as_string();
        VERIFY_ARE_EQUAL(recordToString, resultToString);
        
        VERIFY_ARE_EQUAL(record->Presence->Data(), activityJson[L"richPresence"].as_string());

        VerifyBroadcastRecord(record->BroadcastRecord, utils::extract_json_field(activityJson, L"broadcast", true));
    }

    void VerifyPresenceRecord(PresenceRecord^ record, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL(record->XboxUserId->Data(), resultToVerify[L"xuid"].as_string());
        VERIFY_ARE_EQUAL(record->UserState.ToString()->Data(), resultToVerify[L"state"].as_string());

        web::json::array deviceArray = resultToVerify[L"devices"].as_array();

        uint32_t deviceCounter = 0;
        for (auto &devices : deviceArray)
        {
            web::json::array titleArray = devices[L"titles"].as_array();
            auto deviceRecord = record->PresenceDeviceRecords->GetAt(deviceCounter);
            uint32_t titleCounter = 0;
            string_t deviceRecordToString = deviceRecord->DeviceType.ToString()->Data();
            string_t jsonToString = devices[L"type"].as_string();
            VERIFY_ARE_EQUAL(deviceRecordToString, jsonToString);
            for (auto &titleValue : titleArray)
            {
                auto presenceTitleRecord = deviceRecord->PresenceTitleRecords->GetAt(titleCounter);
                VerifyPresenceTitleRecord(presenceTitleRecord, titleValue);
                ++titleCounter;
            }
            ++deviceCounter;
        }
    }

    DEFINE_TEST_CASE(TestSetPresenceAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetPresenceAsync);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(
            xboxLiveContext->PresenceService->SetPresenceAsync(
                true,
                ref new PresenceData(
                    L"12345675467",
                    L"20"
                    )
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(TestXboxUserId)/devices/current/titles/current",
            httpCall->PathQueryFragment.to_string()
            );

        auto setPresenceJson = web::json::value::parse(setPresenceRequest);
        auto requestJson = web::json::value::parse(httpCall->request_body().request_message_string());
        VERIFY_IS_EQUAL_JSON(setPresenceJson, requestJson);
    }

    DEFINE_TEST_CASE(TestGetPresenceAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetPresenceAsync);
        auto responseJson = web::json::value::parse(defaultPresenceResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        PresenceRecord^ result = create_task(
            xboxLiveContext->PresenceService->GetPresenceAsync(
                ref new Platform::String(L"12345")
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(12345)?level=all",
            httpCall->PathQueryFragment.to_string()
            );

        VerifyPresenceRecord(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetPresenceForMultipleUsersAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetPresenceForMultipleUsersAsync);
        auto responseJson = web::json::value::parse(defaultMultiplePresenceResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Platform::Collections::Vector<Platform::String^>^ users = ref new Platform::Collections::Vector<Platform::String^>();
        users->Append(L"12345");
        users->Append(L"56789");

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Windows::Foundation::Collections::IVectorView<PresenceRecord^>^ result = create_task(
            xboxLiveContext->PresenceService->GetPresenceForMultipleUsersAsync(
                users->GetView()
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/batch",
            httpCall->PathQueryFragment.to_string()
            );

        auto getPresenceForMultipleUsers = web::json::value::parse(getPresenceForMultipleUsersRequest);
        auto requestJson = web::json::value::parse(httpCall->request_body().request_message_string());
        VERIFY_IS_EQUAL_JSON(getPresenceForMultipleUsers, requestJson);

        web::json::array jsonArray = responseJson.as_array();
        uint32 counter = 0;
        for (auto user : result)
        {
            VerifyPresenceRecord(user, jsonArray[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetPresenceForMultipleUsersOverloadAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetPresenceForMultipleUsersOverloadAsync);
        auto responseJson = web::json::value::parse(defaultMultiplePresenceResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Platform::Collections::Vector<Platform::String^>^ users = ref new Platform::Collections::Vector<Platform::String^>();
        users->Append(L"12345");
        users->Append(L"56789");

        Platform::Collections::Vector<PresenceDeviceType>^ deviceTypes = ref new Platform::Collections::Vector<PresenceDeviceType>();
        deviceTypes->Append(PresenceDeviceType::PC);
        deviceTypes->Append(PresenceDeviceType::Windows8);

        Platform::Collections::Vector<uint32>^ titleIds = ref new Platform::Collections::Vector<uint32>();
        titleIds->Append(0);
        titleIds->Append(1);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Windows::Foundation::Collections::IVectorView<PresenceRecord^>^ result = create_task(
            xboxLiveContext->PresenceService->GetPresenceForMultipleUsersAsync(
                users->GetView(),
                deviceTypes->GetView(),
                titleIds->GetView(),
                PresenceDetailLevel::All,
                true,
                true
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/batch",
            httpCall->PathQueryFragment.to_string()
            );

        auto getPresenceForMultipleUsers = web::json::value::parse(getPresenceForMultipleUsersOverloadRequest);
        auto requestJson = web::json::value::parse(httpCall->request_body().request_message_string());
        VERIFY_IS_EQUAL_JSON(getPresenceForMultipleUsers, requestJson);

        web::json::array jsonArray = responseJson.as_array();
        uint32 counter = 0;
        for (auto user : result)
        {
            VerifyPresenceRecord(user, jsonArray[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetPresenceForSocialGroupAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetPresenceForSocialGroupAsync);
        auto responseJson = web::json::value::parse(defaultMultiplePresenceResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Windows::Foundation::Collections::IVectorView<PresenceRecord^>^ result = create_task(
            xboxLiveContext->PresenceService->GetPresenceForSocialGroupAsync(
                    ref new Platform::String(L"testGroup")
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(TestXboxUserId)/groups/testGroup?level=all",
            httpCall->PathQueryFragment.to_string()
            );

        web::json::array jsonArray = responseJson.as_array();
        uint32 counter = 0;
        for (auto user : result)
        {
            VerifyPresenceRecord(user, jsonArray[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetPresenceForSocialGroupOverloadAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetPresenceForSocialGroupOverloadAsync);
        auto responseJson = web::json::value::parse(defaultMultiplePresenceResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Platform::Collections::Vector<PresenceDeviceType>^ deviceTypes = ref new Platform::Collections::Vector<PresenceDeviceType>();
        deviceTypes->Append(PresenceDeviceType::PC);
        deviceTypes->Append(PresenceDeviceType::Windows8);

        Platform::Collections::Vector<uint32>^ titleIds = ref new Platform::Collections::Vector<uint32>();
        titleIds->Append(0);
        titleIds->Append(1);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Windows::Foundation::Collections::IVectorView<PresenceRecord^>^ result = create_task(
            xboxLiveContext->PresenceService->GetPresenceForSocialGroupAsync(
                L"testGroup", 
                L"12345",
                deviceTypes->GetView(),
                titleIds->GetView(),
                PresenceDetailLevel::All,
                true,
                false
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/batch",
            httpCall->PathQueryFragment.to_string()
            );

        auto getPresenceForSocialGroup = web::json::value::parse(getPresenceForSocialGroupOverloadRequest);
        auto requestJson = web::json::value::parse(httpCall->request_body().request_message_string());
        VERIFY_IS_EQUAL_JSON(getPresenceForSocialGroup, requestJson);

        web::json::array jsonArray = responseJson.as_array();
        uint32 counter = 0;
        for (auto user : result)
        {
            VerifyPresenceRecord(user, jsonArray[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestRTATitlePresence)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTATitlePresence);
        const int subId = 321;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, defaultPresenceResponse, subId);

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();

        bool didFire = false;
        concurrency::event fireEvent;
        auto sessionChangeEvent = xboxLiveContext->PresenceService->TitlePresenceChanged += 
        ref new Windows::Foundation::EventHandler<TitlePresenceChangeEventArgs^>([this, &fireEvent, &didFire](Platform::Object^, TitlePresenceChangeEventArgs^ args)
        {
            didFire = true;
            VERIFY_ARE_EQUAL(args->XboxUserId->Data(), string_t(L"TestUser"));
            VERIFY_ARE_EQUAL_INT(args->TitleId, 1563044810);

            VERIFY_IS_TRUE(args->TitleState == TitlePresenceState::Started);
            fireEvent.set();
        });

        auto titlePresenceSubscription = xboxLiveContext->PresenceService->SubscribeToTitlePresenceChange(
            ref new Platform::String(_T("TestUser")),
            1563044810
            );

        fireEvent.wait();
        fireEvent.reset();

        mockSocket->receive_rta_event(subId, web::json::value::string(titlePresenceStartedResponse).serialize());

        string_t titleSubUri = titlePresenceSubscription->ResourceUri->Data();
        VERIFY_ARE_EQUAL(titleSubUri, L"https://userpresence.xboxlive.com/users/xuid(TestUser)/titles/1563044810");
        fireEvent.wait();

        VERIFY_IS_TRUE(titlePresenceSubscription->State == Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState::Subscribed);
        VERIFY_ARE_EQUAL_STR(titlePresenceSubscription->ResourceUri->Data(), L"https://userpresence.xboxlive.com/users/xuid(TestUser)/titles/1563044810");
        VERIFY_ARE_EQUAL_STR(titlePresenceSubscription->XboxUserId->Data(), L"TestUser");
        VERIFY_ARE_EQUAL_INT(titlePresenceSubscription->SubscriptionId, 321);

        fireEvent.reset();

        didFire = false;
        xboxLiveContext->PresenceService->UnsubscribeFromTitlePresenceChange(
            titlePresenceSubscription
            );

        mockSocket->receive_rta_event(subId, web::json::value::string(titlePresenceStartedResponse).serialize());
        VERIFY_IS_FALSE(didFire);
    }

    DEFINE_TEST_CASE(TestRTADevicePresence)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTADevicePresence);
        const int subId = 321;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, defaultPresenceResponse, subId);

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();

        web::json::value devicePresenceResponseJson;

        string_t testString(L"TestUser");

        bool didFire = false;
        concurrency::event fireEvent;

        auto sessionChangeEvent = xboxLiveContext->PresenceService->DevicePresenceChanged += 
            ref new Windows::Foundation::EventHandler<DevicePresenceChangeEventArgs^>([this, &fireEvent, &didFire, &devicePresenceResponseJson](Platform::Object^, DevicePresenceChangeEventArgs^ args)
        {
            didFire = true;
            VERIFY_ARE_EQUAL(args->XboxUserId->Data(), string_t(L"TestUser"));

            if (!devicePresenceResponseJson.is_null())
            {
                auto response = utils::string_split(devicePresenceResponseJson.as_string(), ':');
                auto expected = presence_device_record::_Convert_string_to_presence_device_type(response[0]);
                VERIFY_ARE_EQUAL_INT(static_cast<uint32>(args->DeviceType), static_cast<uint32>(expected));
                VERIFY_ARE_EQUAL(args->IsUserLoggedOnDevice.ToString()->Data(), response[1]);
            }
            else
            {
                VERIFY_IS_TRUE(args->IsUserLoggedOnDevice);    // we set to false if uninitialized
            }
            fireEvent.set();
        });

        auto devicePresenceSubscription = xboxLiveContext->PresenceService->SubscribeToDevicePresenceChange(
            ref new Platform::String(_T("TestUser"))
            );

        fireEvent.wait();
        fireEvent.reset();

        devicePresenceResponseJson = web::json::value::string(devicePresenceResponse);
        mockSocket->receive_rta_event(subId, devicePresenceResponseJson.serialize());
        fireEvent.wait();

        VERIFY_IS_TRUE(devicePresenceSubscription->State == Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState::Subscribed);
        VERIFY_ARE_EQUAL_STR(devicePresenceSubscription->ResourceUri, L"https://userpresence.xboxlive.com/users/xuid(TestUser)/devices");
        VERIFY_ARE_EQUAL_STR(devicePresenceSubscription->XboxUserId, L"TestUser");
        VERIFY_ARE_EQUAL_INT(devicePresenceSubscription->SubscriptionId, 321);

        fireEvent.reset();
        string_t deviceSubUri = devicePresenceSubscription->ResourceUri->Data();
        VERIFY_ARE_EQUAL(deviceSubUri, L"https://userpresence.xboxlive.com/users/xuid(TestUser)/devices");
        didFire = false;
        xboxLiveContext->PresenceService->UnsubscribeFromDevicePresenceChange(
            devicePresenceSubscription
            );

        mockSocket->receive_rta_event(subId, devicePresenceResponseJson.serialize());
        VERIFY_IS_FALSE(didFire);
    }

    DEFINE_TEST_CASE(TestPresenceWriter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPresenceWriter);
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(L"");
        httpCall->set_custom_header(L"X-Heartbeat-After", L"60");

        pplx::event writeFinishEvent;
        int writeDelay = 0;
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler([&writeFinishEvent, &writeDelay](int delay)
        {
            writeDelay = delay;
            writeFinishEvent.set();
        });

        auto presenceWriter = xbox::services::presence::presence_writer::get_presence_writer_singleton();
        presenceWriter->start_writer(
            xboxLiveContext->presence_service()._Impl()
        );

        writeFinishEvent.wait();
        writeFinishEvent.reset();


        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(TestXboxUserId)/devices/current/titles/current",
            httpCall->PathQueryFragment.to_string()
            );

        VERIFY_IS_TRUE(writeDelay == 1);
        VERIFY_IS_TRUE(httpCall->CallCounter == 1);

        presenceWriter->stop_writer(xboxLiveContext->user()->XboxUserId->Data());
        writeFinishEvent.wait();
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler(nullptr);

    }

    DEFINE_TEST_CASE(TestPresenceWriterNoHeartbeatAfter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPresenceWriterNoHeartbeatAfter);
        pplx::event writeFinishEvent;
        int writeDelay = 0;
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler([&writeFinishEvent, &writeDelay](int delay)
        {
            writeDelay = delay;
            writeFinishEvent.set();
        });
         
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(L"");
        auto presenceWriter = xbox::services::presence::presence_writer::get_presence_writer_singleton();
        presenceWriter->start_writer(
            xboxLiveContext->presence_service()._Impl()
            );

        writeFinishEvent.wait();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(TestXboxUserId)/devices/current/titles/current",
            httpCall->PathQueryFragment.to_string()
            );

        VERIFY_IS_TRUE(writeDelay == presenceWriter->s_defaultHeartBeatDelayInMins);
        VERIFY_IS_TRUE(httpCall->CallCounter == 1);

        writeFinishEvent.reset();

        presenceWriter->stop_writer(xboxLiveContext->user()->XboxUserId->Data());
        writeFinishEvent.wait();
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler(nullptr);

    }


    DEFINE_TEST_CASE(TestMultiUserPresenceWriter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultiUserPresenceWriter);
        pplx::event writeFinishEvent;
        int writeDelay = 0;
        int eventExpecting = 2;
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler([&writeFinishEvent, &writeDelay, &eventExpecting](int delay)
        {
            writeDelay = delay;
            if (!--eventExpecting)
            {
                writeFinishEvent.set();
            }
        });

        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        auto xboxLiveContext1 = GetMockXboxLiveContext_Cpp(L"TestUser1");

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(L"");
        httpCall->set_custom_header(L"X-Heartbeat-After", L"60");
        auto presenceWriter = xbox::services::presence::presence_writer::get_presence_writer_singleton();
        presenceWriter->start_writer(
            xboxLiveContext->presence_service()._Impl()
        );

        presenceWriter->start_writer(
            xboxLiveContext1->presence_service()._Impl()
        );

        writeFinishEvent.wait();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);

        VERIFY_IS_TRUE(writeDelay == 1);
        VERIFY_ARE_EQUAL_UINT(2, httpCall->CallCounter);
        VERIFY_ARE_EQUAL_UINT(2, presenceWriter->tracking_count()); 

        writeFinishEvent.reset();
        eventExpecting = 2;

        presenceWriter->stop_writer(xboxLiveContext->user()->XboxUserId->Data());
        presenceWriter->stop_writer(xboxLiveContext1->user()->XboxUserId->Data());
        writeFinishEvent.wait();
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler(nullptr);
    }

    DEFINE_TEST_CASE(TestMultiUserMultiContextPresenceWriter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPresenceWriter);

        pplx::event writeFinishEvent;
        int writeDelay = 0;
        int eventExpecting = 2;
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler([&writeFinishEvent, &writeDelay, &eventExpecting](int delay)
        {
            writeDelay = delay;
            if (!--eventExpecting)
            {
                writeFinishEvent.set();
            }
        });

        auto xboxLiveContext1_User1 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2_User1 = ref new Microsoft::Xbox::Services::XboxLiveContext(xboxLiveContext1_User1->User);

        auto xboxLiveContext1_User2 = GetMockXboxLiveContext_WinRT(L"TestUser1");
        auto xboxLiveContext2_User2 = ref new Microsoft::Xbox::Services::XboxLiveContext(xboxLiveContext1_User1->User);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(L"");
        httpCall->set_custom_header(L"X-Heartbeat-After", L"60");
        auto presenceWriter = xbox::services::presence::presence_writer::get_presence_writer_singleton();
        presenceWriter->start_writer(
            xboxLiveContext1_User1->PresenceService->GetCppObj()._Impl()
        );

        presenceWriter->start_writer(
            xboxLiveContext2_User1->PresenceService->GetCppObj()._Impl()
        );
        VERIFY_ARE_EQUAL_UINT(1, presenceWriter->tracking_count());

        presenceWriter->start_writer(
            xboxLiveContext1_User2->PresenceService->GetCppObj()._Impl()
        );
        presenceWriter->start_writer(
            xboxLiveContext2_User2->PresenceService->GetCppObj()._Impl()
        );
        VERIFY_ARE_EQUAL_UINT(2, presenceWriter->tracking_count());

        writeFinishEvent.wait();

        VERIFY_ARE_EQUAL_UINT(2, presenceWriter->tracking_count());

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userpresence.mockenv.xboxlive.com", httpCall->ServerName);

        VERIFY_IS_TRUE(writeDelay == 1);
        VERIFY_IS_TRUE(httpCall->CallCounter == 2);

        writeFinishEvent.reset();
        eventExpecting = 2;

        presenceWriter->stop_writer(xboxLiveContext1_User1->User->XboxUserId->Data());
        VERIFY_ARE_EQUAL_UINT(1, presenceWriter->tracking_count());
        presenceWriter->stop_writer(xboxLiveContext2_User1->User->XboxUserId->Data());
        VERIFY_ARE_EQUAL_UINT(1, presenceWriter->tracking_count());
        presenceWriter->stop_writer(xboxLiveContext1_User2->User->XboxUserId->Data());
        VERIFY_ARE_EQUAL_UINT(0, presenceWriter->tracking_count());
        presenceWriter->stop_writer(xboxLiveContext2_User2->User->XboxUserId->Data());
        VERIFY_ARE_EQUAL_UINT(0, presenceWriter->tracking_count());

        writeFinishEvent.wait();
        xbox::services::presence::presence_service_impl::set_presence_set_finished_handler(nullptr);
    }

    DEFINE_TEST_CASE(TestPresenceInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestPresenceInvalidArgs);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        VERIFY_THROWS_HR_CX(
            ref new PresenceData(ref new Platform::String(L""), ref new Platform::String(L"")),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            ref new PresenceData(ref new Platform::String(L"test"), ref new Platform::String(L"")),
            E_INVALIDARG
            );

        Platform::Collections::Vector<Platform::String^>^ tokenIds = ref new Platform::Collections::Vector<Platform::String^>();
        VERIFY_THROWS_HR_CX(
            ref new PresenceData(
                ref new Platform::String(L"test"), 
                ref new Platform::String(L"test1"),
                tokenIds->GetView()
                ),
            E_INVALIDARG
            );

        tokenIds->Append(ref new Platform::String(L"test"));
        tokenIds->Append(ref new Platform::String(L"test1"));
        tokenIds->Append(ref new Platform::String(L""));

        VERIFY_THROWS_HR_CX(
            ref new PresenceData(
                ref new Platform::String(L"test"),
                ref new Platform::String(L"test1"),
                tokenIds->GetView()
                ),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PresenceService->GetPresenceAsync(
                ref new Platform::String(L"")
                )).get(),
                E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PresenceService->GetPresenceForMultipleUsersAsync(
                nullptr,
                nullptr,
                nullptr,
                PresenceDetailLevel::All,
                true,
                true
                )).get(),
                E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PresenceService->GetPresenceForMultipleUsersAsync(
                nullptr
                )).get(),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PresenceService->GetPresenceForSocialGroupAsync(
                ref new Platform::String(L"")
                )).get(),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PresenceService->GetPresenceForSocialGroupAsync(
                ref new Platform::String(L""),
                ref new Platform::String(L""),
                nullptr,
                nullptr,
                PresenceDetailLevel::All,
                true,
                true
                )).get(),
            E_INVALIDARG
            );
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
