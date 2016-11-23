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
#include "http_client.h"
#include "xbox_system_factory.h"
#include "user_impl_ios.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::shared_ptr<user_impl>
xbox_system_factory::create_user_impl(user_creation_context userCreationContext)
{
    UNREFERENCED_PARAMETER(userCreationContext)
    return std::make_shared<user_impl_ios>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
