// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"johlafo"
#define TEST_CLASS_AREA L"Utils"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

DEFINE_TEST_CLASS(XboxLiveCallbackTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveCallbackTests)

    static void invoke_callback(int value, xbox_live_callback<int> callback)
    {
        callback(value);
    }

    DEFINE_TEST_CASE(BasicCallback)
    {
        int value = 1;
        xbox_live_callback<int> lambda = [value](int _value)
        {
            VERIFY_ARE_EQUAL_INT(value, _value);
        };

        invoke_callback(value, lambda);
    }

    DEFINE_TEST_CASE(NestedCallbacks)
    {
        int firstValue = 1;
        int secondValue = 2;

        xbox_live_callback<int> lambda = [firstValue](int _value)
        {
            VERIFY_ARE_EQUAL_INT(firstValue, _value);
        };

        xbox_live_callback<int> enclosingLambda = [firstValue, secondValue, lambda](int _value)
        {
            VERIFY_ARE_EQUAL_INT(secondValue, _value);
            invoke_callback(firstValue, lambda);
        };

        invoke_callback(secondValue, enclosingLambda);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END