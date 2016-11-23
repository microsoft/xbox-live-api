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