// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"
#include "user_context.h"
#include "StatisticValue_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::stats::manager;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

StatisticValue::StatisticValue(
    _In_ std::shared_ptr<stat_value> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END