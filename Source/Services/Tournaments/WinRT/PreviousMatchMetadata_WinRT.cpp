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
#include "PreviousMatchMetadata_WinRT.h"

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;
using namespace xbox::services::tournaments;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

PreviousMatchMetadata::PreviousMatchMetadata(
    _In_ xbox::services::tournaments::previous_match_metadata cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_result = ref new TournamentTeamResult(m_cppObj.result());
    m_matchDetails = ref new MatchMetadata(m_cppObj.match_details());
}

const xbox::services::tournaments::previous_match_metadata&
PreviousMatchMetadata::GetCppObj() const
{
    return m_cppObj;
}

TournamentTeamResult^
PreviousMatchMetadata::Result::get()
{
    return m_result;
}

MatchMetadata^
PreviousMatchMetadata::MatchDetails::get()
{
    return m_matchDetails;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END