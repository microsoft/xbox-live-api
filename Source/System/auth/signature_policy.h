//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include <vector>
#include <string>

#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class signature_policy
{
public:
    signature_policy(
        _In_ int version, 
        _In_ int maxBodyBytes, 
        _In_ const std::vector<string_t>& extraHeaders);

    int version() const;
    int max_body_bytes() const;
    const std::vector<string_t> extra_headers() const;

    bool operator==(_In_ const signature_policy& rhs) const;

private:
    int m_version;
    int m_maxBodyBytes;
    std::vector<string_t> m_extraHeaders;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END