// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubProfile_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

ClubProfile::ClubProfile(_In_ xbox::services::clubs::club_profile cppObj)
    : m_cppObj(std::move(cppObj))
{
}

ClubStringSetting^ ClubProfile::Name::get()
{
    if (m_name == nullptr)
    {
        m_name = ref new ClubStringSetting(m_cppObj.name());
    }
    return m_name;
}

ClubStringSetting^ ClubProfile::Description::get()
{
    if (m_description == nullptr)
    {
        m_description = ref new ClubStringSetting(m_cppObj.description());
    }
    return m_description;
}

ClubBooleanSetting^ ClubProfile::MatureContentEnabled::get()
{
    if (m_matureContentEnabled == nullptr)
    {
        m_matureContentEnabled = ref new ClubBooleanSetting(m_cppObj.mature_content_enabled());
    }
    return m_matureContentEnabled;
}

ClubBooleanSetting^ ClubProfile::WatchClubTitlesOnly::get()
{
    if (m_watchClubTitlesOnly == nullptr)
    {
        m_watchClubTitlesOnly = ref new ClubBooleanSetting(m_cppObj.watch_club_titles_only());
    }
    return m_watchClubTitlesOnly;
}

ClubBooleanSetting^ ClubProfile::IsSearchable::get()
{
    if (m_isSearchable == nullptr)
    {
        m_isSearchable = ref new ClubBooleanSetting(m_cppObj.is_searchable());
    }
    return m_isSearchable;
}

ClubBooleanSetting^ ClubProfile::IsRecommendable::get()
{
    if (m_isRecommendable == nullptr)
    {
        m_isRecommendable = ref new ClubBooleanSetting(m_cppObj.is_recommendable());
    }
    return m_isRecommendable;
}

ClubBooleanSetting^ ClubProfile::RequestToJoinEnabled::get()
{
    if (m_requestToJoinEnabled == nullptr)
    {
        m_requestToJoinEnabled = ref new ClubBooleanSetting(m_cppObj.request_to_join_enabled());
    }
    return m_requestToJoinEnabled;
}

ClubBooleanSetting^ ClubProfile::LeaveEnabled::get()
{
    if (m_leaveEnabled == nullptr)
    {
        m_leaveEnabled = ref new ClubBooleanSetting(m_cppObj.leave_enabled());
    }
    return m_leaveEnabled;
}

ClubBooleanSetting^ ClubProfile::TransferOwnershipEnabled::get()
{
    if (m_transferOwnershipEnabled == nullptr)
    {
        m_transferOwnershipEnabled = ref new ClubBooleanSetting(m_cppObj.transfer_ownership_enabled());
    }
    return m_transferOwnershipEnabled;
}

ClubStringSetting^ ClubProfile::DisplayImageUrl::get()
{
    if (m_displayImageUrl == nullptr)
    {
        m_displayImageUrl = ref new ClubStringSetting(m_cppObj.display_image_url());
    }
    return m_displayImageUrl;
}

ClubStringSetting^ ClubProfile::BackgroundImageUrl::get()
{
    if (m_backgroundImageUrl == nullptr)
    {
        m_backgroundImageUrl = ref new ClubStringSetting(m_cppObj.background_image_url());
    }
    return m_backgroundImageUrl;
}

ClubStringMultiSetting^ ClubProfile::Tags::get()
{
    if (m_tags == nullptr)
    {
        m_tags = ref new ClubStringMultiSetting(m_cppObj.tags());
    }
    return m_tags;
}

ClubStringSetting^ ClubProfile::PreferredLocale::get()
{
    if (m_preferredLocale == nullptr)
    {
        m_preferredLocale = ref new ClubStringSetting(m_cppObj.preferred_locale());
    }
    return m_preferredLocale;
}

ClubStringMultiSetting^ ClubProfile::AssociatedTitles::get()
{
    if (m_associatedTitles == nullptr)
    {
        m_associatedTitles = ref new ClubStringMultiSetting(m_cppObj.associated_titles());
    }
    return m_associatedTitles;
}

ClubStringSetting^ ClubProfile::PrimaryColor::get()
{
    if (m_primaryColor == nullptr)
    {
        m_primaryColor = ref new ClubStringSetting(m_cppObj.primary_color());
    }
    return m_primaryColor;
}

ClubStringSetting^ ClubProfile::SecondayColor::get()
{
    if (m_secondaryColor == nullptr)
    {
        m_secondaryColor = ref new ClubStringSetting(m_cppObj.secondary_color());
    }
    return m_secondaryColor;
}

ClubStringSetting^ ClubProfile::TertiaryColor::get()
{
    if (m_tertiaryColor == nullptr)
    {
        m_tertiaryColor = ref new ClubStringSetting(m_cppObj.tertiary_color());
    }
    return m_tertiaryColor;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END