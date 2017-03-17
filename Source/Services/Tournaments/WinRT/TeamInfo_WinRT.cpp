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
#include "TeamInfo_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::Tournaments;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamInfo::TeamInfo(
    _In_ xbox::services::tournaments::team_info cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_currentMatchMetadata = ref new Microsoft::Xbox::Services::Tournaments::CurrentMatchMetadata(m_cppObj.current_match_metadata());
    m_previousMatchMetadata = ref new Microsoft::Xbox::Services::Tournaments::PreviousMatchMetadata(m_cppObj.previous_match_metadata());
    m_memberXboxUserIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.member_xbox_user_ids())->GetView();
}

IVectorView<Platform::String^>^
TeamInfo::MemberXboxUserIds::get()
{
    return m_memberXboxUserIds;
}

const xbox::services::tournaments::team_info&
TeamInfo::GetCppObj() const
{
    return m_cppObj;
}

CurrentMatchMetadata^
TeamInfo::CurrentMatchMetadata::get()
{
    return m_currentMatchMetadata;
}

PreviousMatchMetadata^
TeamInfo::PreviousMatchMetadata::get()
{
    return m_previousMatchMetadata;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END