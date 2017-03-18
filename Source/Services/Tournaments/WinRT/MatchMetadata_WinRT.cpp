// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MatchMetadata_WinRT.h"
#include "Utils_WinRT.h"

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

MatchMetadata::MatchMetadata(
    _In_ xbox::services::tournaments::match_metadata cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_teamIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.opposing_team_ids())->GetView();
}

const xbox::services::tournaments::match_metadata&
MatchMetadata::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<Platform::String^>^
MatchMetadata::OpposingTeamIds::get()
{
    return m_teamIds;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END