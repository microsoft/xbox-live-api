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

