/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "shared_macros.h"
//#include "WexTestClass.h"
#include "user_impl.h"
#include "user_context.h"
#include "MockXboxSystemFactory.h"
#include "User_WinRT.h"
#include "XboxLiveContext_WinRT.h"
#include <strsafe.h>

#define DATETIME_STRING_LENGTH_TO_SECOND 19
#define TICKS_PER_SECOND 10000000i64
typedef std::chrono::duration<long long, std::ratio<1, 10000000>> ticks;

Microsoft::Xbox::Services::System::XboxLiveUser^ SignInUserWithMocks_WinRT(const string_t& id = string_t());
Microsoft::Xbox::Services::XboxLiveContext^ GetMockXboxLiveContext_WinRT(const string_t& id = string_t());
std::shared_ptr<xbox::services::xbox_live_context> GetMockXboxLiveContext_Cpp(const string_t& id = string_t());

class UnitTestHelpers
{
public:
    static void StartResponseLogging();

    static void RemoveResponseLogging();

    static bool SetupFactoryHelper(std::shared_ptr<xbox::services::system::MockXboxSystemFactory>& factory)
    {
        if (factory == nullptr)
        {
            factory = std::make_shared<xbox::services::system::MockXboxSystemFactory>();

            xbox::services::system::xbox_system_factory::set_factory(factory);
        }

        factory->reinit();
        return true;
    }
};

std::wstring TimeSpanToString(
    _In_ Windows::Foundation::TimeSpan timeSpan
    );

std::wstring DateTimeToString(
    _In_ Windows::Foundation::DateTime dateTime
    );


std::wstring FormatString(LPCWSTR strMsg, ...);
