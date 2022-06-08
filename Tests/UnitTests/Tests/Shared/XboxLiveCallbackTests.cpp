// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
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
        TEST_LOG(L"Test starting: BasicCallback");

        int value = 1;
        xbox_live_callback<int> lambda = [value](int _value)
        {
            VERIFY_ARE_EQUAL_INT(value, _value);
        };

        invoke_callback(value, lambda);
    }

    DEFINE_TEST_CASE(NestedCallbacks)
    {
        TEST_LOG(L"Test starting: NestedCallbacks");

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

    DEFINE_TEST_CASE(TestXAsyncProviderException)
    {
        TEST_LOG(L"Test starting: TestXAsyncProviderException");

        TestEnvironment testEnv{};
        Event callbackInvoked;

        XAsyncBlock async{};
        async.context = &callbackInvoked;
        async.callback = [](XAsyncBlock* async)
        {
            auto pEvent{ static_cast<Event*>(async->context) };
            pEvent->Set();
        };

        VERIFY_SUCCEEDED(RunAsync(&async, __FUNCTION__, [](XAsyncOp op, const XAsyncProviderData* data)
            {
                UNREFERENCED_PARAMETER(data);
                switch (op)
                {
                case XAsyncOp::DoWork:
                {
                    throw std::exception();
                }
                default:
                {
                    return S_OK;
                }
                }
            }));

        callbackInvoked.Wait();
        VERIFY_ARE_EQUAL(E_FAIL, XAsyncGetStatus(&async, false));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END