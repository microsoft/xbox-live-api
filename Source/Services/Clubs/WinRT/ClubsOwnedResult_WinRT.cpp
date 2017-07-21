// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubsOwnedResult_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

using namespace Platform::Collections;
using namespace xbox::services::clubs;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

ClubsOwnedResult::ClubsOwnedResult(_In_ clubs_owned_result cppObj)
    : m_cppObj(std::move(cppObj))
{
    m_clubIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.club_ids());
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ ClubsOwnedResult::ClubIds::get()
{
    return m_clubIds->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END