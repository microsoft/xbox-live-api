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
/// Data about whether the user has played the title
///</summary>
public ref class TitleHistory sealed
{
public:
    /// <summary>
    /// Whether the user has played this title
    ///</summary>
    DEFINE_PROP_GET_OBJ(HasUserPlayed, has_user_played, bool);

    /// <summary>
    /// The last time the user had played
    ///</summary>
    DEFINE_PROP_GET_DATETIME_OBJ(LastTimeUserPlayed, last_time_user_played);

internal:
    TitleHistory(_In_ xbox::services::social::manager::title_history cppObj);

private:
    xbox::services::social::manager::title_history m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END