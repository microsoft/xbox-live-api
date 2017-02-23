// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SocialRelationshipChangeEventArgs_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

SocialRelationshipChangeEventArgs::SocialRelationshipChangeEventArgs(
    _In_ xbox::services::social::social_relationship_change_event_args cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_xboxUserIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.xbox_user_ids())->GetView();
}

IVectorView<Platform::String^>^
SocialRelationshipChangeEventArgs::XboxUserIds::get()
{
    return m_xboxUserIds;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END