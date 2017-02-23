// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SocialManagerPresenceTitleRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

SocialManagerPresenceTitleRecord::SocialManagerPresenceTitleRecord(
    _In_ xbox::services::social::manager::social_manager_presence_title_record cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END