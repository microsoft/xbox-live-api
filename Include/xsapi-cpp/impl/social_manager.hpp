// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

title_history::title_history(
    const XblTitleHistory& titleHistory
) :
    m_titleHistory{ &titleHistory }
{
}

title_history::title_history(
    const title_history& other
) :
    m_owningPtr{ std::make_shared<XblTitleHistory>(*other.m_titleHistory) },
    m_titleHistory{ m_owningPtr.get() }
{
}

title_history& title_history::operator=(title_history other)
{
    m_owningPtr = other.m_owningPtr;
    m_titleHistory = m_owningPtr.get();
    return *this;
}

bool title_history::has_user_played() const
{
    return m_titleHistory->hasUserPlayed;
}

utility::datetime title_history::last_time_user_played() const
{
    return Utils::DatetimeFromTimeT(m_titleHistory->lastTimeUserPlayed);
}

preferred_color::preferred_color(
    const XblPreferredColor& preferredColor
)
{
    m_primaryColor = Utils::StringTFromUtf8(preferredColor.primaryColor);
    m_secondaryColor = Utils::StringTFromUtf8(preferredColor.secondaryColor);
    m_tertiaryColor = Utils::StringTFromUtf8(preferredColor.tertiaryColor);
}

const char_t* preferred_color::primary_color() const
{
    return m_primaryColor.data();
}

const char_t* preferred_color::secondary_color() const
{
    return m_secondaryColor.data();
}

const char_t* preferred_color::tertiary_color() const
{
    return m_tertiaryColor.data();
}

bool preferred_color::operator!=(
    const preferred_color& rhs
) const
{
    return Utils::Stricmp(m_primaryColor, rhs.m_primaryColor) != 0 ||
        Utils::Stricmp(m_secondaryColor, rhs.m_secondaryColor) != 0 ||
        Utils::Stricmp(m_tertiaryColor, rhs.m_tertiaryColor) != 0;
}

social_manager_presence_title_record::social_manager_presence_title_record(
    const XblSocialManagerPresenceTitleRecord& titleRecord
) :
    m_titleRecord{ &titleRecord }
{
    m_presenceText = Utils::StringTFromUtf8(m_titleRecord->presenceText);
}

social_manager_presence_title_record::social_manager_presence_title_record(
    const social_manager_presence_title_record& other
) :
    m_owningPtr{ std::make_shared<XblSocialManagerPresenceTitleRecord>(*other.m_titleRecord) },
    m_titleRecord{ m_owningPtr.get() },
    m_presenceText{ other.m_presenceText }
{
}

social_manager_presence_title_record& social_manager_presence_title_record::operator=(
    social_manager_presence_title_record other
)
{
    m_owningPtr = other.m_owningPtr;
    m_titleRecord = m_owningPtr.get();
    m_presenceText = other.m_presenceText;
    return *this;
}

uint32_t social_manager_presence_title_record::title_id() const
{
    return m_titleRecord->titleId;
}

bool social_manager_presence_title_record::is_title_active() const
{
    return m_titleRecord->isTitleActive;
}

const char_t* social_manager_presence_title_record::presence_text() const
{
    return m_presenceText.data();
}

bool social_manager_presence_title_record::is_broadcasting() const
{
    return m_titleRecord->isBroadcasting;
}

xbox::services::presence::presence_device_type social_manager_presence_title_record::device_type() const
{
    return static_cast<xbox::services::presence::presence_device_type>(m_titleRecord->deviceType);
}

bool social_manager_presence_title_record::is_primary() const
{
    return m_titleRecord->isPrimary;
}

social_manager_presence_record::social_manager_presence_record(
    const XblSocialManagerPresenceRecord& presenceRecord
) :
    m_presenceRecord{ &presenceRecord }
{
    m_titleRecords.reserve(XBL_NUM_PRESENCE_RECORDS);
    for (size_t i = 0; i < m_presenceRecord->presenceTitleRecordCount; ++i)
    {
        m_titleRecords.push_back(social_manager_presence_title_record{ m_presenceRecord->presenceTitleRecords[i] });
    }
}

social_manager_presence_record::social_manager_presence_record(
    const social_manager_presence_record& other
) :
    m_owningPtr{ std::make_shared<XblSocialManagerPresenceRecord>(*other.m_presenceRecord) },
    m_presenceRecord{ m_owningPtr.get() }
{
    m_titleRecords.reserve(XBL_NUM_PRESENCE_RECORDS);
    for (size_t i = 0; i < m_presenceRecord->presenceTitleRecordCount; ++i)
    {
        m_titleRecords.push_back(social_manager_presence_title_record{ m_presenceRecord->presenceTitleRecords[i] });
    }
}

social_manager_presence_record& social_manager_presence_record::operator=(
    social_manager_presence_record other
)
{
    m_owningPtr = other.m_owningPtr;
    m_presenceRecord = m_owningPtr.get();

    m_titleRecords.clear();
    for (size_t i = 0; i < m_presenceRecord->presenceTitleRecordCount; ++i)
    {
        m_titleRecords.push_back(social_manager_presence_title_record{ m_presenceRecord->presenceTitleRecords[i] });
    }
    return *this;
}

xbox::services::presence::user_presence_state social_manager_presence_record::user_state() const
{
    return static_cast<xbox::services::presence::user_presence_state>(m_presenceRecord->userState);
}

const std::vector<social_manager_presence_title_record>& social_manager_presence_record::presence_title_records() const
{
    return m_titleRecords;
}

bool social_manager_presence_record::is_user_playing_title(
    _In_ uint32_t titleId
) const
{
    for (auto& titleRecord : m_titleRecords)
    {
        if (titleRecord.title_id() == titleId)
        {
            return true;
        }
    }
    return false;
}

xbox_social_user::xbox_social_user(
    const XblSocialManagerUser& user
) :
    m_user{ &user },
    m_titleHistory{ m_user->titleHistory },
    m_preferredColor{ m_user->preferredColor },
    m_presenceRecord{ m_user->presenceRecord }
{
    m_gamerscore = Utils::StringTFromUtf8(m_user->gamerscore);
    m_gamertag = Utils::StringTFromUtf8(m_user->gamertag);
    m_modernGamertag = Utils::StringTFromUtf8(m_user->modernGamertag);
    m_modernGamertagSuffix = Utils::StringTFromUtf8(m_user->modernGamertagSuffix);
    m_uniqueModernGamertag = Utils::StringTFromUtf8(m_user->uniqueModernGamertag);
    m_xboxUserId = Utils::StringTFromUint64(m_user->xboxUserId);
    m_displayName = Utils::StringTFromUtf8(m_user->displayName);
    m_realName = Utils::StringTFromUtf8(m_user->realName);
    m_displayPicUrlRaw = Utils::StringTFromUtf8(m_user->displayPicUrlRaw);
}

xbox_social_user::xbox_social_user(
    const xbox_social_user& other
) :
    m_owningPtr{ std::make_shared<XblSocialManagerUser>(*other.m_user) },
    m_user{ m_owningPtr.get() },
    m_gamerscore{ other.m_gamerscore },
    m_gamertag{ other.m_gamertag },
    m_modernGamertag{ other.m_modernGamertag },
    m_modernGamertagSuffix{ other.m_modernGamertagSuffix },
    m_uniqueModernGamertag{ other.m_uniqueModernGamertag },
    m_xboxUserId{ other.m_xboxUserId },
    m_displayName{ other.m_displayName },
    m_realName{ other.m_realName },
    m_displayPicUrlRaw{ other.m_displayPicUrlRaw },
    m_titleHistory{ m_user->titleHistory },
    m_preferredColor{ m_user->preferredColor },
    m_presenceRecord{ m_user->presenceRecord }
{
}

xbox_social_user& xbox_social_user::operator=(
    xbox_social_user other
)
{
    m_owningPtr = std::make_shared<XblSocialManagerUser>(*other.m_user);
    m_user = other.m_user;
    m_gamerscore = other.m_gamerscore;
    m_gamertag = other.m_gamertag;
    m_modernGamertag = other.m_modernGamertag;
    m_modernGamertagSuffix = other.m_modernGamertagSuffix;
    m_uniqueModernGamertag = other.m_uniqueModernGamertag;
    m_xboxUserId = other.m_xboxUserId;
    m_displayName = other.m_displayName;
    m_realName = other.m_realName;
    m_displayPicUrlRaw = other.m_displayPicUrlRaw;
    m_titleHistory = xbox::services::social::manager::title_history{ m_user->titleHistory };
    m_preferredColor = xbox::services::social::manager::preferred_color{ m_user->preferredColor };
    m_presenceRecord = xbox::services::social::manager::social_manager_presence_record{ m_user->presenceRecord };

    return *this;
}

const char_t* xbox_social_user::xbox_user_id() const
{
    return m_xboxUserId.data();
}

bool xbox_social_user::is_favorite() const
{
    return m_user->isFavorite;
}

bool xbox_social_user::is_following_user() const
{
    return m_user->isFollowingUser;
}

bool xbox_social_user::is_followed_by_caller() const
{
    return m_user->isFollowedByCaller;
}

const char_t* xbox_social_user::display_name() const
{
    return m_displayName.data();
}

const char_t* xbox_social_user::real_name() const
{
    return m_realName.data();
}

const char_t* xbox_social_user::display_pic_url_raw() const
{
    return m_displayPicUrlRaw.data();
}

bool xbox_social_user::use_avatar() const
{
    return m_user->useAvatar;
}

const char_t* xbox_social_user::gamerscore() const
{
    return m_gamerscore.data();
}

const char_t* xbox_social_user::gamertag() const
{
    return m_gamertag.data();
}

const char_t* xbox_social_user::modern_gamertag() const
{
    return m_modernGamertag.data();
}

const char_t* xbox_social_user::modern_gamertag_suffix() const
{
    return m_modernGamertagSuffix.data();
}

const char_t* xbox_social_user::unique_modern_gamertag() const
{
    return m_uniqueModernGamertag.data();
}

const social_manager_presence_record& xbox_social_user::presence_record() const
{
    return m_presenceRecord;
}

const xbox::services::social::manager::title_history& xbox_social_user::title_history() const
{
    return m_titleHistory;
}

const preferred_color& xbox_social_user::preferred_color() const
{
    return m_preferredColor;
}

xbox_user_id_container::xbox_user_id_container(
    _In_ uint64_t xuid
) :
    m_xboxUserId{ Utils::StringTFromUint64(xuid) }
{
}

const char_t* xbox_user_id_container::xbox_user_id() const
{
    return m_xboxUserId.data();
}

social_event::social_event(
    const XblSocialManagerEvent& event
) :
    m_event{ event },
    m_eventType{ static_cast<social_event_type>(m_event.eventType) }
{
    if (m_event.groupAffected)
    {
        auto groupAffected = social_manager::get_singleton_instance()->m_groups[m_event.groupAffected];
        assert(groupAffected);
        m_args = std::make_shared<social_user_group_loaded_event_args>(groupAffected);
    }
}

social_event::social_event(
    const xbox_live_user_t& removedUser
) :
    m_eventType{ social_event_type::local_user_removed }
{
    m_event.user = removedUser;
    m_event.hr = S_OK;
}

xbox_live_user_t social_event::user() const
{
    return m_event.user;
}

social_event_type social_event::event_type() const
{
    return m_eventType;
}

std::vector<xbox_user_id_container> social_event::users_affected() const
{
    std::vector<xbox_user_id_container> usersAffected;
    for (auto& user : m_event.usersAffected)
    {
        if (user)
        {
            usersAffected.push_back(user->xboxUserId);
        }
    }
    return usersAffected;
}

std::shared_ptr<social_event_args> social_event::event_args() const
{
    return m_args;
}

std::error_code social_event::err() const
{
    return Utils::ConvertHr(m_event.hr);
}

std::string social_event::err_message() const
{
    return std::string{};
}

xbox_social_user_group::xbox_social_user_group(
    XblSocialManagerUserGroupHandle group
) :
    m_group{ group }
{
}

std::vector<xbox_social_user*> xbox_social_user_group::users() const
{
    PopulateUsers();

    std::vector<xbox_social_user*> userPointers;
    for (auto& user : m_users)
    {
        userPointers.push_back(&user);
    }
    return userPointers;
}

xbox_live_result<void> xbox_social_user_group::get_copy_of_users(
    _Inout_ std::vector<xbox_social_user>& socialUserVector
)
{
    PopulateUsers();

    for (auto& user : m_users)
    {
        socialUserVector.push_back(user);
    }

    return xbox_live_result<void>{};
}

social_user_group_type xbox_social_user_group::social_user_group_type() const
{
    XblSocialUserGroupType type{};
    auto hr = XblSocialManagerUserGroupGetType(m_group, &type);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return static_cast<xbox::services::social::manager::social_user_group_type>(type);
}

std::vector<xbox_user_id_container> xbox_social_user_group::users_tracked_by_social_user_group() const
{
    const uint64_t* trackedXuids{ nullptr };
    size_t count{ 0 };
    auto hr = XblSocialManagerUserGroupGetUsersTrackedByGroup(m_group, &trackedXuids, &count);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return Utils::Transform<xbox_user_id_container>(trackedXuids, count);
}

xbox_live_user_t xbox_social_user_group::local_user() const
{
    XblUserHandle userHandle{ nullptr };
    auto hr = XblSocialManagerUserGroupGetLocalUser(m_group, &userHandle);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return userHandle;
}

void xbox_social_user_group::PopulateUsers() const
{
    const XblSocialManagerUser* const * users{ nullptr };
    size_t usersCount{ 0 };

    XblSocialManagerUserGroupGetUsers(m_group, &users, &usersCount);

    m_users.clear();
    for (size_t i = 0; i < usersCount; ++i)
    {
        m_users.push_back(xbox_social_user{ *users[i] });
    }
}

presence_filter xbox_social_user_group::presence_filter_of_group() const
{
    XblPresenceFilter filter{ XblPresenceFilter::Unknown };
    XblSocialManagerUserGroupGetFilters(m_group, &filter, nullptr);
    return static_cast<presence_filter>(filter);
}

relationship_filter xbox_social_user_group::relationship_filter_of_group() const
{
    XblRelationshipFilter filter{ XblRelationshipFilter::Friends };
    XblSocialManagerUserGroupGetFilters(m_group, nullptr, &filter);
    return static_cast<relationship_filter>(filter);
}

std::vector<xbox_social_user*> xbox_social_user_group::get_users_from_xbox_user_ids(
    _In_ const std::vector<xbox_user_id_container>& xboxUserIds
)
{
    PopulateUsers();

    std::vector<xbox_social_user*> userPointers;
    for (auto& xuid : xboxUserIds)
    {
        for (auto& user : m_users)
        {
            if (Utils::Stricmp(user.xbox_user_id(), xuid.xbox_user_id()) == 0)
            {
                userPointers.push_back(&user);
                break;
            }
        }
    }
    return userPointers;
}

social_user_group_loaded_event_args::social_user_group_loaded_event_args(
    std::shared_ptr<xbox_social_user_group> group
) :
    m_group{ std::move(group) }
{
}

std::shared_ptr<xbox_social_user_group> social_user_group_loaded_event_args::social_user_group() const
{
    return m_group;
}

std::shared_ptr<social_manager> social_manager::get_singleton_instance()
{
    static std::shared_ptr<social_manager> instance = std::shared_ptr<social_manager>(new social_manager{});
    return instance;
}

xbox_live_result<void> social_manager::add_local_user(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level extraDetailLevel
)
{
    return Utils::ConvertHr(XblSocialManagerAddLocalUser(
        user,
        static_cast<XblSocialManagerExtraDetailLevel>(extraDetailLevel),
        nullptr
    ));
}

xbox_live_result<void> social_manager::remove_local_user(
    _In_ xbox_live_user_t user
)
{
    auto hr = XblSocialManagerRemoveLocalUser(user);
    if (SUCCEEDED(hr))
    {
        // Track removed users to raise user removed event in next do_work call. This is to maintain
        // legacy behavior in c++ interface since removing a user no longer yields an event.
        m_removedUsers.push_back(user);
    }

    return Utils::ConvertHr(hr);
}

std::vector<social_event> social_manager::do_work()
{
    const XblSocialManagerEvent* events{ nullptr };
    size_t eventCount{ 0 };

    auto hr = XblSocialManagerDoWork(&events, &eventCount);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    auto legacyEvents{ Utils::Transform<social_event>(events, eventCount) };
    for (auto& user : m_removedUsers)
    {
        legacyEvents.push_back(social_event{ user });
    }
    m_removedUsers.clear();

    return legacyEvents;
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>> social_manager::create_social_user_group_from_filters(
    _In_ xbox_live_user_t user,
    _In_ presence_filter presenceDetailLevel,
    _In_ relationship_filter filter
)
{
    XblSocialManagerUserGroupHandle groupHandle{ nullptr };

    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromFilters(
        user,
        static_cast<XblPresenceFilter>(presenceDetailLevel),
        static_cast<XblRelationshipFilter>(filter),
        &groupHandle
    );

    if (SUCCEEDED(hr))
    {
        auto group = std::make_shared<xbox_social_user_group>(groupHandle);
        m_groups[groupHandle] = group;
        return group;
    }
    return Utils::ConvertHr(hr);
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>> social_manager::create_social_user_group_from_list(
    _In_ xbox_live_user_t user,
    _In_ std::vector<string_t> xboxUserIdList
)
{
    XblSocialManagerUserGroupHandle groupHandle{ nullptr };

    auto xuids{ Utils::XuidVectorFromXuidStringVector(xboxUserIdList) };

    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromList(
        user,
        xuids.data(),
        xuids.size(),
        &groupHandle
    );

    if (SUCCEEDED(hr))
    {
        auto group = std::make_shared<xbox_social_user_group>(groupHandle);
        m_groups[groupHandle] = group;
        return group;
    }
    return Utils::ConvertHr(hr);
}

xbox_live_result<void> social_manager::destroy_social_user_group(
    _In_ std::shared_ptr<xbox_social_user_group> socialUserGroup
)
{
    auto hr = XblSocialManagerDestroySocialUserGroup(socialUserGroup->m_group);
    if (SUCCEEDED(hr))
    {
        m_groups.erase(socialUserGroup->m_group);
    }
    return Utils::ConvertHr(hr);
}

std::vector<xbox_live_user_t> social_manager::local_users() const
{
    std::vector<xbox_live_user_t> users{};
    size_t localUsersCount{ XblSocialManagerGetLocalUserCount() };
    users = std::vector<XblUserHandle>(localUsersCount, nullptr);
    XblSocialManagerGetLocalUsers(localUsersCount, users.data());

    return users;
}

xbox_live_result<void> social_manager::update_social_user_group(
    _In_ const std::shared_ptr<xbox_social_user_group>& group,
    _In_ const std::vector<string_t>& users
)
{
    auto xuids{ Utils::XuidVectorFromXuidStringVector(users) };
    return Utils::ConvertHr(XblSocialManagerUpdateSocialUserGroup(group->m_group, xuids.data(), xuids.size()));
}

xbox_live_result<void> social_manager::set_rich_presence_polling_status(
    _In_ xbox_live_user_t user,
    _In_ bool shouldEnablePolling
)
{
    return Utils::ConvertHr(XblSocialManagerSetRichPresencePollingStatus(user, shouldEnablePolling));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
