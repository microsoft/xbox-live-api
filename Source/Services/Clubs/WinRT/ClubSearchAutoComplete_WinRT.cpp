// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubSearchAutoComplete_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::clubs;

ClubSearchAutoComplete::ClubSearchAutoComplete(_In_ xbox::services::clubs::club_search_auto_complete cppObj)
    : m_cppObj(std::move(cppObj))
{
    m_tags = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.tags());
    m_associatedTitles = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.associated_titles());    
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ ClubSearchAutoComplete::Tags::get()
{
    return m_tags->GetView();
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ ClubSearchAutoComplete::AssociatedTitles::get()
{
    return m_associatedTitles->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END