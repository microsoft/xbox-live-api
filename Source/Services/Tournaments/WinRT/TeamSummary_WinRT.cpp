// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TeamSummary_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::Tournaments;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamSummary::TeamSummary(
    _In_ xbox::services::tournaments::team_summary cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::team_summary&
TeamSummary::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END