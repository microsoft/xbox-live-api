// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"johlafo"
#define TEST_CLASS_AREA L"Utils"
#include "UnitTestIncludes.h"
#include "Utils_WinRT.h"
#include "utils_uwp.h"
#if !XSAPI_U
#include "ppltasks_extra.h"
using namespace Concurrency::extras; // TODO move from this namespace
#endif

using namespace xbox::services;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


DEFINE_TEST_CLASS(DelayedTaskTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(DelayedTaskTests)

    DEFINE_TEST_CASE(DelayedTaskBasic)
    {
        win32_handle handle;
        handle.set(CreateEvent(nullptr, true, false, nullptr));

        create_delayed_task(std::chrono::milliseconds(1000),
            [&handle]()
        {
            TEST_LOG(L"Delayed Callback called!");
            SetEvent(handle.get());
        });
        
        utils::sleep(1100);
        VERIFY_IS_TRUE(WAIT_TIMEOUT != WaitForSingleObjectEx(handle.get(), 0, false));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END