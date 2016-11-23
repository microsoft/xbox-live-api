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
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"XboxLiveContextSettings"
#include "UnitTestIncludes.h"
#include "XboxLiveContextSettings_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

DEFINE_TEST_CLASS(XboxLiveContextSettingsTests_WinRT)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveContextSettingsTests_WinRT)

    DEFINE_TEST_CASE(TestXboxLiveContextSettings)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestXboxLiveContextSettings);
        std::shared_ptr<xbox_live_context_settings> cppObj = std::make_shared<xbox_live_context_settings>();
        Microsoft::Xbox::Services::XboxLiveContextSettings^ xboxLiveContextSettings = ref new Microsoft::Xbox::Services::XboxLiveContextSettings(cppObj);

        // Verify defaults
        VERIFY_ARE_EQUAL(false, xboxLiveContextSettings->EnableServiceCallRoutedEvents);
        VERIFY_ARE_EQUAL_INT(30, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->HttpTimeout).count());
        VERIFY_ARE_EQUAL_INT(5 * 60, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->LongHttpTimeout).count());
        VERIFY_ARE_EQUAL_INT(2, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->HttpRetryDelay).count());
        VERIFY_ARE_EQUAL_INT(20, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->HttpTimeoutWindow).count());

        // Verify sets
        xboxLiveContextSettings->EnableServiceCallRoutedEvents = true;
        xboxLiveContextSettings->HttpTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(1000));
        xboxLiveContextSettings->HttpRetryDelay = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::milliseconds(2000));
        xboxLiveContextSettings->HttpTimeoutWindow = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::milliseconds(3000));
        xboxLiveContextSettings->LongHttpTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(4000));
        VERIFY_ARE_EQUAL(xboxLiveContextSettings->EnableServiceCallRoutedEvents, true);
        VERIFY_ARE_EQUAL_INT(1000, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->HttpTimeout).count());
        VERIFY_ARE_EQUAL_INT(2000, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(xboxLiveContextSettings->HttpRetryDelay).count());
        VERIFY_ARE_EQUAL_INT(3000, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(xboxLiveContextSettings->HttpTimeoutWindow).count());
        VERIFY_ARE_EQUAL_INT(4000, UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(xboxLiveContextSettings->LongHttpTimeout).count());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

