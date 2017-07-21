// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const club_setting<string_t>& club_profile::name() const
{
    return m_nameSetting;
}

const club_setting<string_t>& club_profile::description() const
{
    return m_descriptionSetting;
}

const club_setting<bool>& club_profile::mature_content_enabled() const
{
    return m_matureContentEnabledSetting;
}

const club_setting<bool>& club_profile::watch_club_titles_only() const
{
    return m_watchClubTitlesOnlySetting;
}

const club_setting<bool>& club_profile::is_searchable() const
{
    return m_isSearchableSetting;
}

const club_setting<bool>& club_profile::is_recommendable() const
{
    return m_isRecommendableSetting;
}

const club_setting<bool>& club_profile::request_to_join_enabled() const
{
    return m_requestToJoinEnabledSetting;
}

const club_setting<bool>& club_profile::leave_enabled() const
{
    return m_leaveEnabledSetting;
}

const club_setting<bool>& club_profile::transfer_ownership_enabled() const
{
    return m_transferOwnershipEnabledSetting;
}

const club_setting<string_t>& club_profile::display_image_url() const
{
    return m_displayImageUrlSetting;
}

const club_setting<string_t>& club_profile::background_image_url() const
{
    return m_backgroundImageUrlSetting;
}

const club_multi_setting<string_t>& club_profile::tags() const
{
    return m_tagsSetting;
}

const club_setting<string_t>& club_profile::preferred_locale() const
{
    return m_preferredLocaleSetting;
}

const club_multi_setting<string_t>& club_profile::associated_titles() const
{
    return m_associatedTitlesSetting;
}

const club_setting<string_t>& club_profile::primary_color() const
{
    return m_primaryColorSetting;
}

const club_setting<string_t>& club_profile::secondary_color() const
{
    return m_secondaryColorSetting;
}

const club_setting<string_t>& club_profile::tertiary_color() const
{
    return m_tertiaryColorSetting;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
