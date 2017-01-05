///*********************************************************
///
/// Copyright (c) Microsoft. All rights reserved.
/// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
/// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
/// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
/// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
///
///*********************************************************
#include "pch.h"
#include "system_internal.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

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

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif