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
#include "xsapi/simple_stats.h"
#include "StatisticContext_WinRT.h"

using namespace xbox::services::experimental::stats::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

StatisticContext::StatisticContext(
    _In_ Platform::String^ name,
    _In_ Platform::String^ value
    ) 
{
    assert(name != nullptr);
    assert(value != nullptr);
    m_cppObj = stat_context(
        name->Data(),
        value->Data()
        );
}

StatisticContext::StatisticContext(
    _In_ stat_context cppObj
    ) : m_cppObj(std::move(cppObj))
{
}

const stat_context&
StatisticContext::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END