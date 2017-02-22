// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentTeamResult_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace xbox::services;
using namespace xbox::services::tournaments;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentTeamResult::TournamentTeamResult()
{
}

TournamentTeamResult::TournamentTeamResult(
    _In_ tournament_team_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

xbox::services::tournaments::tournament_team_result 
TournamentTeamResult::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END