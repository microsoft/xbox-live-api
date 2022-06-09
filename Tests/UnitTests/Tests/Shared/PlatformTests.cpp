// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(PlatformTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PlatformTests);

private:
    struct LocalStorageManager
    {
    public:
        LocalStorageManager() noexcept
        {
            VERIFY_SUCCEEDED(XblLocalStorageSetHandlers(nullptr, WriteHandler, ReadHandler, ClearHandler, this));
        }

        ~LocalStorageManager() noexcept
        {
            VERIFY_SUCCEEDED(XblLocalStorageSetHandlers(nullptr, nullptr, nullptr, nullptr, nullptr));
        }

    private:
        static void WriteHandler(
            _In_opt_ void* context,
            _In_ XblClientOperationHandle operation,
            _In_ XblUserHandle user,
            _In_ XblLocalStorageWriteMode mode,
            _In_z_ char const* key,
            _In_ size_t dataSize,
            _In_reads_bytes_(dataSize) void const* data
        )
        {
            UNREFERENCED_PARAMETER(user);
            assert(context);

            auto pThis{ static_cast<LocalStorageManager*>(context) };
            std::unique_lock<std::mutex> lock(pThis->m_mutex, std::defer_lock);

            Sleep(2000);

            if (lock.try_lock())
            {
                auto& vec{ pThis->m_data[key] };
                auto bytes{ static_cast<const uint8_t*>(data) };

                switch (mode)
                {
                case XblLocalStorageWriteMode::Append:
                {
                    vec.insert(vec.end(), bytes, bytes + dataSize);
                    break;
                }
                case XblLocalStorageWriteMode::Truncate:
                {
                    vec = std::vector<uint8_t>(bytes, bytes + dataSize);
                    break;
                }
                }
                lock.unlock();

                XblLocalStorageWriteComplete(operation, XblClientOperationResult::Success, vec.size());
            }
            else
            {
                XblLocalStorageReadComplete(operation, XblClientOperationResult::Failure, 0, nullptr);
            }
        }

        static void ReadHandler(
            _In_opt_ void* context,
            _In_ XblClientOperationHandle operation,
            _In_ XblUserHandle user,
            _In_z_ const char* key
        )
        {
            UNREFERENCED_PARAMETER(user);
            assert(context);

            auto pThis{ static_cast<LocalStorageManager*>(context) };

            std::unique_lock<std::mutex> lock(pThis->m_mutex, std::defer_lock);
            if (lock.try_lock())
            {
                auto& vec{ pThis->m_data[key] };
                XblLocalStorageReadComplete(operation, XblClientOperationResult::Success, vec.size(), vec.data());
            }
            else
            {
                XblLocalStorageReadComplete(operation, XblClientOperationResult::Failure, 0, nullptr);
            }
        }

        static void ClearHandler(
            _In_opt_ void* context,
            _In_ XblClientOperationHandle operation,
            _In_ XblUserHandle user,
            _In_z_ const char* key
        )
        {
            UNREFERENCED_PARAMETER(user);
            assert(context);

            auto pThis{ static_cast<LocalStorageManager*>(context) };

            std::unique_lock<std::mutex> lock(pThis->m_mutex, std::defer_lock);
            if (lock.try_lock())
            {
                pThis->m_data.erase(key);
                XblLocalStorageClearComplete(operation, XblClientOperationResult::Success);
            }
            else
            {
                XblLocalStorageReadComplete(operation, XblClientOperationResult::Failure, 0, nullptr);
            }
        }

        std::unordered_map<std::string, std::vector<uint8_t>> m_data;
        std::mutex m_mutex;
    };

public:
    DEFINE_TEST_CASE(TestCustomLocalStorageHandlers)
    {
        TEST_LOG(L"Test starting: TestCustomLocalStorageHandlers");
        LocalStorageManager storageManager{};
        TestEnvironment env{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto localStorage{ GlobalState::Get()->LocalStorage() };
        VERIFY_IS_NOT_NULL(localStorage.get());

        std::string dataKey{ "key" };
        std::string writeData{ "Hello, world!" };

        localStorage->WriteAsync(
            xboxLiveContext->User(),
            XblLocalStorageWriteMode::Truncate,
            dataKey.data(),
            Vector<uint8_t>(writeData.data(), writeData.data() + (writeData.size() + 1)),
            nullptr
        );

        Event readComplete;
        std::string readData;

        localStorage->ReadAsync(
            xboxLiveContext->User(),
            dataKey.data(),
            [&](Result<Vector<uint8_t>> result)
            {
                if (Succeeded(result) && result.Payload().size())
                {
                    auto data{ reinterpret_cast<char*>(result.Payload().data()) };
                    readData = data;
                }
                readComplete.Set();
            }
        );

        readComplete.Wait();
        VERIFY_IS_TRUE(writeData == readData);
    }

    enum TestEnum : uint32_t
    {
        ValueDefault = 0,
        Value1 = 1,
        Value3 = 3,
        ValueOutOfRange = DEFAULT_ENUM_MAX + 1,
    };

    enum class TestEnumClass : uint32_t
    {
        ValueDefault = 1,
        ValueMin = 1000,
        ValueMax = 1100
    };

    DEFINE_TEST_CASE(TestEnumTraits)
    {
        TEST_LOG(L"Test starting: TestEnumTraits");

        auto s = EnumName(Value1);
        VERIFY_ARE_EQUAL_STR("Value1", s);

        s = EnumName(ValueOutOfRange);
        VERIFY_ARE_EQUAL(0u, s.size());

        constexpr auto TestEnumName = EnumName<TestEnum, ValueDefault, ValueOutOfRange>;
        s = TestEnumName(ValueOutOfRange);
        VERIFY_ARE_EQUAL_STR("ValueOutOfRange", s);

        auto e = EnumValue<TestEnum>("vAlUe1");
        VERIFY_IS_TRUE(e == TestEnum::Value1);

        e = EnumValue<TestEnum>("value1error");
        VERIFY_IS_TRUE(e == TestEnum::ValueDefault);

        constexpr auto TestEnumClassName = EnumName<TestEnumClass, 1000, 1100>;
        constexpr auto TestEnumClassValue = EnumValue<TestEnumClass, 1000, 1100>;

        s = TestEnumClassName(TestEnumClass::ValueMin);
        VERIFY_ARE_EQUAL_STR(s, "ValueMin");

        auto ec = TestEnumClassValue("ValueMax");
        VERIFY_IS_TRUE(ec == TestEnumClass::ValueMax);

        ec = TestEnumClassValue("InvalidName");
        VERIFY_IS_TRUE(ec == TestEnumClass{});
    }

    DEFINE_TEST_CASE(TestPeriodicTask)
    {
        TEST_LOG(L"Test starting: TestPeriodicTask");

        TestEnvironment env{};

        XTaskQueueHandle queueHandle{ nullptr };
        VERIFY_SUCCEEDED(XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::ThreadPool, &queueHandle));
        TaskQueue queue{ queueHandle };
        XTaskQueueCloseHandle(queueHandle);

        Event e;
        size_t callCount{ 0 };
        using Clock = std::chrono::high_resolution_clock;
        auto startTime{ Clock::now() };

        // Create task. Should immediately run at t~0
        auto task = PeriodicTask::MakeAndRun(queue, 2000, [&]
        {
            std::wstringstream ss;
            ss << L"Task executing at t=" << std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count();
            LOGS_DEBUG << ss.str().data();
            if (++callCount == 5)
            {
                e.Set();
            }
        });

        // manually invoke task at time t~1000 & 4000
        queue.RunWork([&]
        {
            VERIFY_ARE_EQUAL_UINT(1, callCount);
            task->ScheduleImmediately();
            Sleep(3000);
            task->ScheduleImmediately();
        }, 1000);

        // task should run at t~0,1000,3000,4000,6000
        e.Wait();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count();
        VERIFY_IS_TRUE(totalTime < 7000);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END