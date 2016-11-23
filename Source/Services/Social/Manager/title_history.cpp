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
#include "social_manager_internal.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

title_history::title_history() :
    m_userHasPlayed(false)
{
}

bool
title_history::has_user_played() const
{
    return m_userHasPlayed;
}

const utility::datetime&
title_history::last_time_user_played() const
{
    return m_lastTimeUserPlayed;
}

bool
title_history::operator!= (
    _In_ const title_history& previousTitleHistory
    ) const
{
    return (
        previousTitleHistory.m_lastTimeUserPlayed != m_lastTimeUserPlayed ||
        previousTitleHistory.m_titleId != m_titleId ||
        previousTitleHistory.m_userHasPlayed != m_userHasPlayed
        );
}

xbox_live_result<title_history>
title_history::_Deserialize(
    _In_ const web::json::value& json,
    _In_ std::error_code& errcOut
    )
{
    title_history returnObject;
    if (json.is_null())
    {
        return returnObject;
    }

    std::error_code errc = xbox_live_error_code::no_error;
    returnObject.m_lastTimeUserPlayed = utils::extract_json_time(
        json,
        _T("LastTimePlayed"),
        errc
        );
    if (returnObject.m_lastTimeUserPlayed.to_interval() != 0)
    {
        returnObject.m_userHasPlayed = true;
    }
    returnObject.m_titleId = utils::extract_json_int(
        json,
        _T("TitleId"),
        errc
        );

    if (errc)
    {
        errcOut = errc;
    }
    return xbox_live_result<title_history>(
        returnObject,
        errc
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END