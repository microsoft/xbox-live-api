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
#define TEST_CLASS_OWNER L"jasonsa2"
#define TEST_CLASS_AREA L"XboxLiveContext"
#include "UnitTestIncludes.h"
#include "StockMocks.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "xsapi/multiplayer.h"
#include "xsapi/mem.h"

using namespace Microsoft::Xbox::Services;

const std::wstring defaultAchievementResponse =
LR"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

static int g_MemAllocHookCalls = 0;
static int g_MemFreeHookCalls = 0;

DEFINE_TEST_CLASS(XboxLiveContextTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveContextTests)

    DEFINE_TEST_CASE(TestUserContext)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUserContext);

        auto user = SignInUserWithMocks_WinRT();

        xbox::services::user_context userContext(user);
        VERIFY_ARE_EQUAL_STR(L"TestXboxUserId", userContext.xbox_user_id()); // StockMock results
        VERIFY_ARE_EQUAL(user->XboxUserId, userContext.user()->XboxUserId);
    }

    DEFINE_TEST_CASE(TestXboxLiveContext)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestXboxLiveContext);

        auto user = SignInUserWithMocks_WinRT();

        xbox::services::user_context userContext(user);
        XboxLiveContext^ xboxLiveContext = ref new XboxLiveContext(user);
        VERIFY_ARE_EQUAL(xboxLiveContext->User->XboxUserId, user->XboxUserId);
        VERIFY_IS_NOT_NULL(xboxLiveContext->ProfileService);
        VERIFY_IS_NOT_NULL(xboxLiveContext->SocialService);
    }

    DEFINE_TEST_CASE(TestServiceCallRouted)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestServiceCallRouted);

        auto user = SignInUserWithMocks_WinRT();

        task_completion_event<void> tce;
        xbox::services::user_context userContext(user);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        xboxLiveContext->Settings->EnableServiceCallRoutedEvents = true;
        xboxLiveContext->Settings->ServiceCallRouted += ref new Windows::Foundation::EventHandler<Microsoft::Xbox::Services::XboxServiceCallRoutedEventArgs^>(
            [&tce](Platform::Object^, Microsoft::Xbox::Services::XboxServiceCallRoutedEventArgs^ args)
        {
            VERIFY_ARE_EQUAL_STR(L"GET", args->HttpMethod);
            VERIFY_ARE_EQUAL_INT(200, args->HttpStatus);
            VERIFY_ARE_EQUAL_STR(L"https://achievements.mockenv.xboxlive.com/", args->Url->AbsoluteUri);
            VERIFY_IS_EQUAL_JSON(web::json::value::parse(defaultAchievementResponse), web::json::value::parse(args->ResponseBody->Data()));
            tce.set();
        });

        auto responseJson = web::json::value::parse(defaultAchievementResponse);
        auto achievementToVerify = responseJson.as_object()[L"achievements"].as_array()[0];

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson,200, web::http::http_response(), xboxLiveContext->Settings->GetCppObj());

        auto result = pplx::create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            "xboxUserId",
            "serviceConfigurationId",
            "achievementId"
            )).get();
        pplx::create_task(tce).wait();
    }

    DEFINE_TEST_CASE(TestUserContext2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUserContext2);
        // TODO (878285): Figure out user_context resolution
        //User^ user = SignInUserWithMocks_WinRT();

        //auto userContext = std::make_shared<xbox::services::user_context>(user);
        //auto xboxLiveContextSettings = std::make_shared<xbox::services::xbox_live_context_settings>();
        //std::shared_ptr<xbox::services::local_config> localConfig = xbox::services::system::xbox_system_factory::get_factory()->create_local_config();

        //XboxLiveContext^ xboxLiveContext = ref new XboxLiveContext(user);
        //xbox::services::achievements::achievement_service achievementService(userContext, xboxLiveContextSettings, localConfig);
        //xbox::services::multiplayer::multiplayer_service multiplayerService(userContext, xboxLiveContextSettings, localConfig);
        //
        //auto multiplayerShared = multiplayerService.get_user_context();
        //VERIFY_ARE_EQUAL(multiplayerShared->xbox_user_id().c_str(), userContext.xbox_user_id().c_str());
    }

    DEFINE_TEST_CASE(StressTestXboxLiveContext)
    {
        DEFINE_TEST_CASE_PROPERTIES(StressTestXboxLiveContext);

        const uint32_t c_numTasks = 100;
        std::vector<pplx::task<void>> taskVec(c_numTasks);
        for (uint32_t i = 0; i < c_numTasks; ++i)
        {
            auto task = create_task([]()
            {
                VERIFY_NO_THROW(
                    auto user = SignInUserWithMocks_WinRT();

                    xbox::services::user_context userContext(user);
                    XboxLiveContext^ xboxLiveContext = ref new XboxLiveContext(user);
                );
            });

            taskVec[i] = task;
        }

        for (auto& task : taskVec)
        {
            task.wait();
        }
    }

    static _Ret_maybenull_ _Post_writable_byte_size_(dwSize) void* __stdcall MemAllocHook(
        _In_ size_t dwSize
        )
    {
        g_MemAllocHookCalls++;
        return new (std::nothrow) char[dwSize];
    }

    static void __stdcall MemFreeHook(
        _In_ void* pAddress
        )
    {
        g_MemFreeHookCalls++;
        delete[] pAddress;
    }

    DEFINE_TEST_CASE(TestMemoryHook)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMemoryHook);

        g_MemAllocHookCalls = 0;
        g_MemFreeHookCalls = 0;

        xbox_live_services_settings::get_singleton_instance()->set_memory_allocation_hooks(
            MemAllocHook,
            MemFreeHook
            );

        {
            xbox::services::system::xsapi_memory_buffer buf(1000);
            VERIFY_ARE_EQUAL_INT(0, g_MemFreeHookCalls);
            VERIFY_ARE_EQUAL_INT(1, g_MemAllocHookCalls);
            void* t = buf.get();
            VERIFY_IS_NOT_NULL(t);
        }
        VERIFY_ARE_EQUAL_INT(1, g_MemFreeHookCalls);
        VERIFY_ARE_EQUAL_INT(1, g_MemAllocHookCalls);

        g_MemAllocHookCalls = 0;
        g_MemFreeHookCalls = 0;

        xbox_live_services_settings::get_singleton_instance()->set_memory_allocation_hooks(
            nullptr,
            nullptr
            );

        {
            xbox::services::system::xsapi_memory_buffer buf(1000);
            VERIFY_ARE_EQUAL_INT(0, g_MemFreeHookCalls);
            VERIFY_ARE_EQUAL_INT(0, g_MemAllocHookCalls);
            void* t = buf.get();
            VERIFY_IS_NOT_NULL(t);
        }
        VERIFY_ARE_EQUAL_INT(0, g_MemFreeHookCalls);
        VERIFY_ARE_EQUAL_INT(0, g_MemAllocHookCalls);
    }

    DEFINE_TEST_CASE(TestMemoryHookVector)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMemoryHookVector);

        g_MemAllocHookCalls = 0;
        g_MemFreeHookCalls = 0;

        xbox_live_services_settings::get_singleton_instance()->set_memory_allocation_hooks(
            MemAllocHook,
            MemFreeHook
            );

        {
            xsapi_internal_vector(uint32_t) vec;
            vec.resize(10000);
            VERIFY_ARE_EQUAL_INT(0, g_MemFreeHookCalls);
            VERIFY_ARE_EQUAL_INT(2, g_MemAllocHookCalls);
        }
        VERIFY_ARE_EQUAL_INT(2, g_MemFreeHookCalls);
        VERIFY_ARE_EQUAL_INT(2, g_MemAllocHookCalls);
    }

    DEFINE_TEST_CASE(TestMemoryHookMap)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMemoryHookMap);

        g_MemAllocHookCalls = 0;
        g_MemFreeHookCalls = 0;

        xbox_live_services_settings::get_singleton_instance()->set_memory_allocation_hooks(
            MemAllocHook,
            MemFreeHook
            );

        {
            xsapi_internal_unordered_map(uint32_t, uint32_t) unorderedMap;
            for(int i=0; i<1000; i++)
            {
                unorderedMap[i] = i * 1000;
            }
            VERIFY_ARE_EQUAL_INT(3, g_MemFreeHookCalls);
            VERIFY_ARE_EQUAL_INT(1007, g_MemAllocHookCalls);
        }
        VERIFY_ARE_EQUAL_INT(1007, g_MemFreeHookCalls);
        VERIFY_ARE_EQUAL_INT(1007, g_MemAllocHookCalls);
    }


};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

