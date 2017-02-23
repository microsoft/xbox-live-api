// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Statistic_winrt.h" 
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::user_statistics;

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

Statistic::Statistic(
    _In_ statistic cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_statType = UtilsWinRT::ConvertStringToPropertyType(m_cppObj.statistic_type());
}

Windows::Foundation::PropertyType 
Statistic::StatisticType::get()
{
    return m_statType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END