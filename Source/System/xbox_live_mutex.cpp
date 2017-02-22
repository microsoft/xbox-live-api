// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

xbox_live_mutex::xbox_live_mutex()
{
}

xbox_live_mutex::xbox_live_mutex(_In_ const xbox_live_mutex& other)
{
    UNREFERENCED_PARAMETER(other);
}

xbox_live_mutex
xbox_live_mutex::operator=(_In_ const xbox_live_mutex& other)
{
    UNREFERENCED_PARAMETER(other);
    return xbox_live_mutex();
}

std::mutex&
xbox_live_mutex::get()
{
    return m_xboxLiveMutex;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END