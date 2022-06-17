// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

size_t g_memAllocHookCalls{ 0 };
size_t g_memFreeHookCalls{ 0 };
std::unordered_map<void*, size_t> g_allocationMap{};

const char getProfileResponse[] = R"(
{
      "profileUsers": [
         {
            "id":"2533274791381930",
            "settings" : [
               {
                  "id":"GameDisplayName",
                  "value" : "John Smith"
               },
               {
                  "id":"GameDisplayPicRaw",
                  "value" : "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KIN0gxC2r1YECCd3mf2w1FDdmFCpSokJWa2z7xtVrlzOyVSc6pPRdWEXmYtpS2xE4F"
               },
               {
                  "id":"Gamerscore",
                  "value" : "0"
               },
               {
                  "id":"Gamertag",
                  "value" : "CracklierJewel9"
               }
            ],
            "isSponsoredUser":false
         }
      ]
})";

DEFINE_TEST_CLASS(GlobalTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(GlobalTests);

    DEFINE_TEST_CASE(TestXblInitializeAndCleanup)
    {
        TEST_LOG(L"Test starting: TestXblInitializeAndCleanup");

        XTaskQueueHandle queueHandle{};
        VERIFY_SUCCEEDED(XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::ThreadPool, &queueHandle));
        TaskQueue queue{ queueHandle };

        std::string scid{ "TestScid" };

        XblInitArgs args{};
        args.queue = queueHandle;
        args.scid = scid.data();
        char folder[MAX_PATH] = {};
        GetCurrentDirectoryA(MAX_PATH, folder);
        args.localStoragePath = folder;

        TestEnvironment env{ &args };

        const char* actualScid{ nullptr };
        VERIFY_SUCCEEDED(XblGetScid(&actualScid));
        VERIFY_IS_TRUE(scid == actualScid);

        XTaskQueueHandle actualQueue = XblGetAsyncQueue();
        // By design the queue returned is different than the queue provided, so just validate that the
        // returned Queue is non-null
        VERIFY_IS_NOT_NULL(actualQueue);
    }

    DEFINE_TEST_CASE(TestServiceCallRouted)
    {
        TEST_LOG(L"Test starting: TestServiceCallRouted");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        struct CallRoutedContext 
        {
            ~CallRoutedContext()
            {
                SetCallHandle(nullptr);
            }

            HCCallHandle CallHandle() const noexcept
            {
                return m_callHandle;
            }

            void SetCallHandle(HCCallHandle callHandle) noexcept
            {
                if (m_callHandle)
                {
                    VERIFY_SUCCEEDED(HCHttpCallCloseHandle(m_callHandle));
                }
                m_callHandle = HCHttpCallDuplicateHandle(callHandle);
            }

            size_t responseCount{ 0 };
        private:
            HCCallHandle m_callHandle{ nullptr };
        } c;

        auto token = XblAddServiceCallRoutedHandler(
            [](XblServiceCallRoutedArgs args, void* context)
            {
                LOGS_DEBUG << "Received call routed callback: " << args.fullResponseFormatted;

                auto callRoutedContext{ static_cast<CallRoutedContext*>(context) };
                callRoutedContext->SetCallHandle(args.call);
                callRoutedContext->responseCount++;
            },
            &c
        );

        JsonDocument getProfileResponseJson;
        getProfileResponseJson.Parse(getProfileResponse);
        HttpMock mock{ "POST", "https://profile.xboxlive.com", 200, getProfileResponseJson };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfileAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        XblUserProfile profile{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfileResult(&async, &profile));

        VERIFY_IS_NOT_NULL(c.CallHandle());
        VERIFY_ARE_EQUAL_INT(1, c.responseCount);

        const char* actualUrl{ nullptr };
        VERIFY_SUCCEEDED(HCHttpCallGetRequestUrl(c.CallHandle(), &actualUrl));

        std::string expectedUrl{ "https://profile.xboxlive.com" };
        VERIFY_IS_TRUE(std::string{ actualUrl }.substr(0, expectedUrl.size()) == expectedUrl);

        uint32_t httpStatus;
        VERIFY_SUCCEEDED(HCHttpCallResponseGetStatusCode(c.CallHandle(), &httpStatus));
        VERIFY_ARE_EQUAL_INT(httpStatus, 200);

        const char* responseBody{ nullptr };
        VERIFY_SUCCEEDED(HCHttpCallResponseGetResponseString(c.CallHandle(), &responseBody));
        JsonDocument responseBodyJson;
        responseBodyJson.Parse(responseBody);
        VERIFY_IS_EQUAL_JSON(responseBodyJson, getProfileResponseJson);

        XblRemoveServiceCallRoutedHandler(token);
    }

    DEFINE_TEST_CASE(TestMemoryHook)
    {
        TEST_LOG(L"Test starting: TestMemoryHook");

        class MemoryManager
        {
        public:
            MemoryManager() noexcept
            {
                g_memAllocHookCalls = 0;
                g_memFreeHookCalls = 0;
                g_allocationMap.clear();

                VERIFY_SUCCEEDED(XblMemSetFunctions(MemAllocHook, MemFreeHook));
            }

            ~MemoryManager() noexcept
            {
                VERIFY_SUCCEEDED(XblMemSetFunctions(nullptr, nullptr));
            }
        private:
            static _Ret_maybenull_ _Post_writable_byte_size_(dwSize) void* STDAPIVCALLTYPE MemAllocHook(
                _In_ size_t dwSize,
                _In_ HCMemoryType memType
            )
            {
                UNREFERENCED_PARAMETER(memType);
                auto ptr{ new char[dwSize] };
                g_allocationMap[ptr] = g_memAllocHookCalls++;
                return ptr;
            }

            static void STDAPIVCALLTYPE MemFreeHook(
                _In_ void* pAddress,
                _In_ HCMemoryType memType
            )
            {
                UNREFERENCED_PARAMETER(memType);
                g_memFreeHookCalls++;
                size_t removed{ g_allocationMap.erase(pAddress) };
                assert(removed);
                delete[] pAddress;
            }
        } memoryManager;

        {
            TestEnvironment env{};

            // We don't know how many allocation XSAPI will make during initialize, but validate that the hook is used
            VERIFY_IS_TRUE(g_memAllocHookCalls > 0);
        }

        VERIFY_ARE_EQUAL_INT(g_memAllocHookCalls, g_memFreeHookCalls);
    }

    struct CancellableOperation
    {
    public:
        CancellableOperation() noexcept = default;
        ~CancellableOperation() noexcept = default;

        HRESULT Run(XAsyncBlock* async) noexcept
        {
            return XAsyncBegin(async, this, nullptr, __FUNCTION__, CancellableOperation::Provider);
        }

        std::atomic<bool> complete{ false };

    private:
        static HRESULT CALLBACK Provider(XAsyncOp op, const XAsyncProviderData* data) noexcept
        {
            auto pThis{ static_cast<CancellableOperation*>(data->context) };

            switch (op)
            {
            case XAsyncOp::Begin:
            {
                return XAsyncSchedule(data->async, 0);
            }
            case XAsyncOp::DoWork:
            {
                if (!pThis->complete)
                {
                    RETURN_HR_IF_FAILED(XAsyncSchedule(data->async, 1000));
                    return E_PENDING;
                }
                return S_OK;
            }
            case XAsyncOp::Cancel:
            {
                return S_OK;
            }
            default:
            {
                return S_OK;
            }
            }
        };
    };

    DEFINE_TEST_CASE(TestCancellation)
    {
        TEST_LOG(L"Test starting: TestCancellation");

        XTaskQueueHandle queue{ nullptr };
        VERIFY_SUCCEEDED(XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::ThreadPool, &queue));

        XAsyncBlock async{ queue };

        CancellableOperation op;
        op.Run(&async);

        std::thread terminator{ [&]
        {
            Sleep(50);
            XTaskQueueTerminate(queue, true, nullptr, nullptr);
        } };

        HRESULT hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL(hr, E_ABORT);

        terminator.join();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END