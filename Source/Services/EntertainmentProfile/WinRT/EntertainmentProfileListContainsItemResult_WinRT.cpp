// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if TV_API || UNIT_TEST_SERVICES
#include "EntertainmentProfileListContainsItemResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

EntertainmentProfileListContainsItemResult::EntertainmentProfileListContainsItemResult(
    xbox::services::entertainment_profile::entertainment_profile_list_contains_item_result cppObj
    ) :
    m_cppObj(cppObj)
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
