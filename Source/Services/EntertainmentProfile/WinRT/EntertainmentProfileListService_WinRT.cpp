// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if TV_API || UNIT_TEST_SERVICES
#include "EntertainmentProfileListService_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

EntertainmentProfileListService::EntertainmentProfileListService(
    _In_ xbox::services::entertainment_profile::entertainment_profile_list_service cppObj
    ) :
    m_cppObj(cppObj)
{
}

EntertainmentProfileListXboxOnePins^
EntertainmentProfileListService::XboxOnePins::get()
{
    return ref new EntertainmentProfileListXboxOnePins(m_cppObj.xbox_one_pins());
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
