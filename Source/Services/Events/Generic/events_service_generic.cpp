// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include <cll/GenericPartA.h>

#include "events_service_xsapi.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

void EventsService::InitializeTenantSettings()
{
    auto appConfig = AppConfig::Instance();

    // .c_str() to force conversion from xsapi_internal_string const& to std::string&&
    m_tenantSettings = MakeShared<cll::CllTenantSettings>(cll::GenericPartA{
        IKey().c_str(),
        appConfig->AppId().c_str(),
        appConfig->AppVer().c_str(),
        appConfig->OsName().c_str(),
        appConfig->OsLocale().c_str(),
        appConfig->OsVersion().c_str(),
        appConfig->DeviceClass().c_str(),
        appConfig->DeviceId().c_str()
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END