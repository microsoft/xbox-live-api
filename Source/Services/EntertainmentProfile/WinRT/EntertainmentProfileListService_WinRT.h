// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if TV_API || UNIT_TEST_SERVICES

#include "EntertainmentProfileListVideoQueue_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

public ref class EntertainmentProfileListService sealed
{
public:
    /// <summary>
    /// Returns the XboxOne Pins
    /// </summary>
    property Microsoft::Xbox::Services::EntertainmentProfile::EntertainmentProfileListXboxOnePins^ 
        XboxOnePins { Microsoft::Xbox::Services::EntertainmentProfile::EntertainmentProfileListXboxOnePins^ get(); }

internal:
    EntertainmentProfileListService( 
        _In_ xbox::services::entertainment_profile::entertainment_profile_list_service cppObj
        );

private:
    xbox::services::entertainment_profile::entertainment_profile_list_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
