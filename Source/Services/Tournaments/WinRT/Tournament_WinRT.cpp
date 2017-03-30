// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Tournament_WinRT.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

Tournament::Tournament(
    _In_ xbox::services::tournaments::tournament cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    if (!m_cppObj.team_summary().is_null())
    {
        m_teamSummary = ref new Microsoft::Xbox::Services::Tournaments::TeamSummary(m_cppObj.team_summary());
    }
}

const xbox::services::tournaments::tournament&
Tournament::GetCppObj() const
{
    return m_cppObj;
}

TeamSummary^
Tournament::TeamSummary::get()
{
    return m_teamSummary;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END