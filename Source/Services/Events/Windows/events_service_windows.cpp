// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_xsapi.h"
#include <cll/Windows7PartA.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

void EventsService::InitializeTenantSettings()
{
    m_tenantSettings = MakeShared<cll::CllTenantSettings>(cll::Windows7PartA(IKey()));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END