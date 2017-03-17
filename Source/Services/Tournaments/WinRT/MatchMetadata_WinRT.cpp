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