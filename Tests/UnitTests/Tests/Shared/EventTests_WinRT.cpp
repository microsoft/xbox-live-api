// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"EventTests_WinRT"
#include "UnitTestIncludes.h"
#include "Event_WinRT.h"

using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(EventTests_WinRT)
{
public:
    DEFINE_TEST_CLASS_PROPS(EventTests_WinRT)

    DEFINE_TEST_CASE(TestEvent)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestEvent);

        Event^ event = ref new Event();
        VERIFY_NO_THROW(event->Reset());
        VERIFY_NO_THROW(event->Set());
        VERIFY_NO_THROW(event->Wait());
        VERIFY_IS_TRUE(event->Wait(1));
        VERIFY_NO_THROW(event->Reset());
        VERIFY_IS_FALSE(event->Wait(0));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

