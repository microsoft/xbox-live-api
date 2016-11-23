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
#include "xsapi/user_statistics.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

requested_statistics::requested_statistics(
    _In_ string_t serviceConfigurationId,
    _In_ std::vector<string_t> statistics
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_statistics(std::move(statistics))
{
}

const string_t& requested_statistics::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const std::vector<string_t>& requested_statistics::statistics() const
{
    return m_statistics;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END