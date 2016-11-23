///*********************************************************
///
/// Copyright (c) Microsoft. All rights reserved.
/// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
/// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
/// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
/// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
///
///*********************************************************
#pragma once
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// The users preferred color scheme
/// </summary>
public ref class PreferredColor sealed
{
public:
    /// <summary>
    /// The user's primary preferred color
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(PrimaryColor, primary_color);

    /// <summary>
    /// The user's secondary preferred color
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(SecondaryColor, secondary_color);

    /// <summary>
    /// The user's tertiary preferred color
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(TertiaryColor, tertiary_color);

internal:
    PreferredColor(
        _In_ xbox::services::social::manager::preferred_color cppObj
        );

private:
    xbox::services::social::manager::preferred_color m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END