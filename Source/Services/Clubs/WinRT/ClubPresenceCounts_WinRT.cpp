// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubPresenceCounts_WinRT.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

ClubPresenceCounts::ClubPresenceCounts(_In_ xbox::services::clubs::club_presence_counts cppObj)
    : m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END