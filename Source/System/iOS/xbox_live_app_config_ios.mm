// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-cpp/system.h"
#include "xbox_live_app_config_internal.h"
#import "XBLiOSGlobalState.h"
#include <rapidjson/allocators.hpp>
#include "uri_impl.h"	

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

const xsapi_internal_string& AppConfig::APNSEnvironment() const
{
    return m_apnsEnvironment;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
