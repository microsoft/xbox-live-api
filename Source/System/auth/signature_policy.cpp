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
#include "signature_policy.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

signature_policy::signature_policy(
    _In_ int version, 
    _In_ int maxBodyBytes, 
    _In_ const std::vector<string_t>& extraHeaders) :
    m_version(version), 
    m_maxBodyBytes(maxBodyBytes), 
    m_extraHeaders(extraHeaders)
{
}

int signature_policy::version() const
{
    return m_version;
}

int signature_policy::max_body_bytes() const
{
    return m_maxBodyBytes;
}

const std::vector<string_t> signature_policy::extra_headers() const
{
    return m_extraHeaders;
}

bool signature_policy::operator==(
    _In_ const signature_policy& rhs) const
{
    if (m_version != rhs.m_version || 
        m_maxBodyBytes != rhs.m_maxBodyBytes ||
        m_extraHeaders.size() != rhs.m_extraHeaders.size())
    {
        return false;
    }

    auto lit = m_extraHeaders.cbegin();
    auto rit = rhs.m_extraHeaders.cbegin();

    while (lit != m_extraHeaders.cend())
    {
        if (*lit != *rit) return false;
        ++lit;
        ++rit;
    }

    return true;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
