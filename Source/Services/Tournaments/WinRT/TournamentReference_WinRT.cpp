// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentReference_WinRT.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentReference::TournamentReference()
{
}

TournamentReference::TournamentReference(
    _In_ xbox::services::tournaments::tournament_reference cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::tournament_reference&
TournamentReference::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END