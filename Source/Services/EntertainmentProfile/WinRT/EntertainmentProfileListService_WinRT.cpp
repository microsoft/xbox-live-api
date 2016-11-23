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
