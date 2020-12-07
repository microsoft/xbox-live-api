// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_xsapi.h"
#include "a/java_interop.h"
#include "a/jni_utils.h"
#include <cll/AndroidPartA.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

void EventsService::InitializeTenantSettings()
{
    m_tenantSettings = MakeShared<cll::CllTenantSettings>(cll::AndroidPartA(java_interop::get_java_interop_singleton()->GetJniEnv(), nullptr, IKey()));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END