// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "CurrentMatchMetadata_WinRT.h"

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::Multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

CurrentMatchMetadata::CurrentMatchMetadata(
    _In_ xbox::services::tournaments::current_match_metadata cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_gameSessionReference = ref new MultiplayerSessionReference(m_cppObj.game_session_reference());
    m_matchDetails = ref new MatchMetadata(m_cppObj.match_details());
}

const xbox::services::tournaments::current_match_metadata&
CurrentMatchMetadata::GetCppObj() const
{
    return m_cppObj;
}

MultiplayerSessionReference^
CurrentMatchMetadata::GameSessionReference::get()
{
    return m_gameSessionReference;
}

MatchMetadata^
CurrentMatchMetadata::MatchDetails::get()
{
    return m_matchDetails;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END