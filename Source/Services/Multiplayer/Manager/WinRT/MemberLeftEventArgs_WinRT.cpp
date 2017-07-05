// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MemberLeftEventArgs_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MemberLeftEventArgs::MemberLeftEventArgs(
    _In_ std::shared_ptr<member_left_event_args> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

std::shared_ptr<member_left_event_args>
MemberLeftEventArgs::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<MultiplayerMember^>^ 
MemberLeftEventArgs::Members::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerMember>(m_cppObj->members())->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END