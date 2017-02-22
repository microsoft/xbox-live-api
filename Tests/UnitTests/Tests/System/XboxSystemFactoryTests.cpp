// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestBase.h"
#define TEST_CLASS_OWNER L"jasonsa"
#include "DefineTestMacros.h"
#include "xbox_system_factory.h"
#include "user_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class XboxSystemFactoryTests : public UnitTestBase
{
public:
    TEST_CLASS(XboxSystemFactoryTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD(TestFactory)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::shared_ptr<xbox_system_factory> factory = std::make_shared<xbox_system_factory_winstore_impl>();
        xbox_system_factory_winstore_impl::set_factory(factory);
        VERIFY_IS_NOT_NULL(factory->create_http_call(
            nullptr, L"", L"", L"", xbox_live_api::unspecified));
        VERIFY_IS_NOT_NULL(factory->create_xtitle_service());
        VERIFY_IS_NOT_NULL(factory->create_user_token());
        VERIFY_IS_NOT_NULL(factory->create_xsts_token());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

