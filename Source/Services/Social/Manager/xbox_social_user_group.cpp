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
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"
#include "xsapi/presence.h"

using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

xbox_social_user_group::xbox_social_user_group(
    _In_ string_t viewHash,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter,
    _In_ uint32_t titleId,
    _In_ xbox_live_user_t xboxLiveUser
    ) :
    m_viewHash(std::move(viewHash)),
    m_presenceFilter(presenceFilter),
    m_relationshipFilter(relationshipFilter),
    m_titleId(titleId),
    m_xboxLiveUser(xboxLiveUser),
    m_userGroupType(social_user_group_type::filter_type),
    m_detailLevel(social_manager_extra_detail_level::no_extra_detail),
    m_needsUpdate(true)
{
}

xbox_social_user_group::xbox_social_user_group(
    _In_ string_t viewHash,
    _In_ std::vector<string_t> userList,
    _In_ xbox_live_user_t xboxLiveUser
    ) :
    m_viewHash(std::move(viewHash)),
    m_xboxLiveUser(std::move(xboxLiveUser)),
    m_userGroupType(social_user_group_type::user_list_type),
    m_presenceFilter(presence_filter::unknown),
    m_relationshipFilter(relationship_filter::friends),
    m_detailLevel(social_manager_extra_detail_level::no_extra_detail),
    m_titleId(0),
    m_needsUpdate(true)
{
    for (auto& user : userList)
    {
        uint64_t id = utils::string_t_to_uint64(user.c_str());
        if (id == 0)
        {
            LOG_ERROR("Invalid user");
            continue;
        }

        m_userUpdateListString.push_back(user.c_str());
        m_userUpdateListInt.push_back(id);
    }
}

social_user_group_type
xbox_social_user_group::social_user_group_type()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_userGroupType;
}

const std::vector<xbox_user_id_container>&
xbox_social_user_group::users_tracked_by_social_user_group()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_userUpdateListString;
}

const xbox_live_user_t&
xbox_social_user_group::local_user()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_xboxLiveUser;
}

void
xbox_social_user_group::destroy()
{
    m_userUpdateListInt.clear();
    m_userGroupVector.clear();
    m_userUpdateListString.clear();
}

const std::vector<uint64_t>&
xbox_social_user_group::tracking_users()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_userUpdateListInt;
}

void xbox_social_user_group::update_view(
    _In_ const xsapi_internal_unordered_map(uint64_t, xbox_social_user_context)& snapshotList,
    _In_ const std::vector<social_event>& socialEvents
    )
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    if (m_userGroupType == social_user_group_type::filter_type)
    {
        filter_list(
            snapshotList,
            socialEvents
            );
    }
    else if (m_userGroupType == social_user_group_type::user_list_type)
    {
        if (m_userGroupVector.empty() || m_userGroupVector.size() != m_userUpdateListInt.size() || m_needsUpdate)
        {
            m_userGroupVector.clear();
            for (auto userUpdateInt : m_userUpdateListInt)
            {
                if (snapshotList.find(userUpdateInt) != snapshotList.end())
                {
                    auto socialUser = snapshotList.at(userUpdateInt).socialUser;
                    if (socialUser != nullptr)
                    {
                        m_userGroupVector.push_back(socialUser);
                    }
                }
            }
        }
        else
        {
            for (auto i = m_userGroupVector.begin(); i < m_userGroupVector.end(); ++i)
            {
                if (*i == nullptr)
                {
                    continue;
                }
                auto user = *i;
                auto userIter = snapshotList.find(user->_Xbox_user_id_as_integer());
                if (userIter != snapshotList.end())
                {
                    *i = userIter->second.socialUser;
                }
            }
        }
    }

    m_needsUpdate = false;
}

void
xbox_social_user_group::initialize_filter_list(
    _In_ const xsapi_internal_unordered_map(uint64_t, xbox_social_user_context)& users
    )
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    std::vector<uint64_t> resultVec;
    for (auto& userPairMap : users)
    {
        auto user = userPairMap.second.socialUser;
        if (user == nullptr)
        {
            continue;
        }
        if ((m_relationshipFilter == relationship_filter::favorite && user->is_favorite()) ||
            (m_relationshipFilter == relationship_filter::friends && user->is_followed_by_caller())
            )
        {
            bool userValid = get_presence_filter_result(
                user,
                m_presenceFilter
            );

            if (userValid)
            {
                m_userUpdateListInt.push_back(userPairMap.first);
                m_userGroupVector.push_back(user);

                m_userUpdateListString.push_back(userPairMap.second.socialUser->xbox_user_id());
            }
        }
    }
}

void
xbox_social_user_group::remove_users(
    _In_ const std::vector<xbox_removal_struct>& usersToRemove
    )
{
    for (auto& userRemovalStruct : usersToRemove)
    {
        for (auto userUpdateIter = m_userUpdateListString.begin(); userUpdateIter != m_userUpdateListString.end(); ++userUpdateIter)
        {
            const char_t* xuid = userUpdateIter->xbox_user_id();
            if (utils::char_t_cmp(xuid, userRemovalStruct.xuidContainer.xbox_user_id()) == 0)
            {
                m_userUpdateListString.erase(userUpdateIter);
                break;
            }
        }
        for (auto userUpdateIter = m_userGroupVector.begin(); userUpdateIter != m_userGroupVector.end(); ++userUpdateIter)
        {
            auto xuid = (*userUpdateIter)->_Xbox_user_id_as_integer();
            if (xuid == userRemovalStruct.xuidNum)
            {
                m_userGroupVector.erase(userUpdateIter);
                break;
            }
        }
        for (auto userUpdateIter = m_userUpdateListInt.begin(); userUpdateIter != m_userUpdateListInt.end(); ++userUpdateIter)
        {
            auto xuidAsInt = *userUpdateIter;
            if (xuidAsInt == userRemovalStruct.xuidNum)
            {
                m_userUpdateListInt.erase(userUpdateIter);
                break;
            }
        }
    }
}

void
xbox_social_user_group::filter_list(
    _In_ const xsapi_internal_unordered_map(uint64_t, xbox_social_user_context)& snapshotList,
    _In_ const std::vector<social_event>& socialEvents
    )
{
    std::vector<xbox_user_id_container> refilterList;
    std::vector<xbox_user_id_container> addedList;
    std::vector<xbox_user_id_container> removeList;
    std::vector<xbox_removal_struct> removalStructList;

    for (auto& evt : socialEvents)
    {
        switch (evt.event_type())
        {
        case social_event_type::presence_changed:
        case social_event_type::profiles_changed:
        case social_event_type::social_relationships_changed:
            refilterList.insert(refilterList.end(), evt.users_affected().begin(), evt.users_affected().end());
            break;
        case social_event_type::users_added_to_social_graph:
            addedList.insert(addedList.end(), evt.users_affected().begin(), evt.users_affected().end());
            break;
        case social_event_type::users_removed_from_social_graph:
            removeList.insert(removeList.end(), evt.users_affected().begin(), evt.users_affected().end());
            break;
        }
    }
    for (auto& userStr : refilterList)
    {
        uint64_t userInt = utils::string_t_to_uint64(userStr.xbox_user_id());
        auto userPair = snapshotList.find(userInt);
        if (userPair == snapshotList.end())
        {
            continue;
        }

        auto user = userPair->second.socialUser;
        if ((m_relationshipFilter == relationship_filter::favorite && user->is_favorite()) ||
            (m_relationshipFilter == relationship_filter::friends && user->is_followed_by_caller())
            )
        {
            bool userValid = get_presence_filter_result(
                user,
                m_presenceFilter
            );

            if (!userValid)
            {
                xbox_removal_struct xboxRemovalStruct;
                xboxRemovalStruct.xuidContainer = userStr.xbox_user_id();
                xboxRemovalStruct.xuidNum = userInt;

                removalStructList.push_back(xboxRemovalStruct);
            }
            else
            {
                // check if added
                bool wasFound = false;
                for (auto& updateUserInt : m_userUpdateListInt)
                {
                    if (updateUserInt == userInt)
                    {
                        wasFound = true;
                        break;
                    }
                }

                if (!wasFound)
                {
                    m_userUpdateListString.push_back(userStr.xbox_user_id());
                    m_userUpdateListInt.push_back(userInt);
                    m_userGroupVector.push_back(user);
                }
            }
        }
    }
    
    for (auto& userStr : addedList)
    {
        uint64_t userInt = utils::string_t_to_uint64(userStr.xbox_user_id());
        auto userPair = snapshotList.find(userInt);
        if (userPair == snapshotList.end())
        {
            continue;
        }

        auto user = userPair->second.socialUser;
        if (user == nullptr)
        {
            continue;
        }
        if ((m_relationshipFilter == relationship_filter::favorite && user->is_favorite()) ||
            (m_relationshipFilter == relationship_filter::friends && user->is_followed_by_caller())
            )
        {
            bool userValid = get_presence_filter_result(
                user,
                m_presenceFilter
            );

            if (userValid)
            {
                m_userUpdateListString.push_back(userStr.xbox_user_id());
                m_userUpdateListInt.push_back(userInt);
                m_userGroupVector.push_back(user);
            }
        }
    }

    for (auto& userStr : removeList)
    {
        uint64_t userInt = utils::string_t_to_uint64(userStr.xbox_user_id());
        xbox_removal_struct xboxRemovalStruct;
        
        xboxRemovalStruct.xuidContainer = userStr;
        xboxRemovalStruct.xuidNum = userInt;

        removalStructList.push_back(xboxRemovalStruct);
    }

    if (!removalStructList.empty())
    {
        remove_users(removalStructList);
    }

    m_userGroupVector.clear();
    for (auto& userInt : m_userUpdateListInt)
    {
        auto userIter = snapshotList.find(userInt);
        if (userIter != snapshotList.end())
        {
            auto& user = userIter->second.socialUser;
            m_userGroupVector.push_back(user);
        }
    }
}

const std::vector<xbox_social_user*>&
xbox_social_user_group::users()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_userGroupVector;
}

user_group_status_change
xbox_social_user_group::_Update_users_in_group(
    _In_ const std::vector<string_t>& userList
    )
{
    xsapi_internal_unordered_map(uint64_t, uint32_t) changeMap;
    xsapi_internal_vector(uint64_t) userIdList;
    userIdList.reserve(userList.size());

    user_group_status_change changeGroups;
    for (auto& user : userList)
    {
        uint64_t id = utils::string_t_to_uint64(user.c_str());
        
        if (id == 0)
        {
            LOG_ERROR("Invalid user");
            continue;
        }

        userIdList.push_back(id);
        bool userFound = false;
        for (auto userInt : m_userUpdateListInt)
        {
            if (userInt == id)
            {
                userFound = true;
                break;
            }
        }
        if (userFound)
        {
            continue;
        }

        changeGroups.addGroup.push_back(user);
        m_userUpdateListInt.push_back(id);

        m_userUpdateListString.push_back(user.c_str());
    }

    std::vector<uint64_t> userCompareList(m_userUpdateListInt);
    for (auto updateUser : userCompareList)
    {
        bool wasFound = false;
        for (auto userId : userIdList)
        {
            if (userId == updateUser)
            {
                wasFound = true;
            }
        }

        if (!wasFound)
        {
            changeGroups.removeGroup.push_back(updateUser);
            
            for (std::vector<uint64_t>::iterator i = m_userUpdateListInt.begin(); i != m_userUpdateListInt.end(); ++i)
            {
                if (*i == updateUser)
                {
                    m_userUpdateListInt.erase(i);
                    break;
                }
            }

            for (auto i = m_userUpdateListString.begin(); i != m_userUpdateListString.end(); ++i)
            {
                stringstream_t str;
                str << updateUser;
                if (utils::str_icmp(i->xbox_user_id(), str.str()) == 0)
                {
                    m_userUpdateListString.erase(i);
                    break;
                }
            }
        }
    }

    if (!changeGroups.addGroup.empty() || !changeGroups.removeGroup.empty())
    {
        m_needsUpdate = true;
    }
    return changeGroups;
}

const string_t&
xbox_social_user_group::hash() const
{
    return m_viewHash;
}

bool
xbox_social_user_group::needs_update()
{
    std::lock_guard<std::mutex> lock(m_groupMutex);
    return m_needsUpdate;
}

xbox_live_result<void>
xbox_social_user_group::get_copy_of_users(
    _Inout_ std::vector<xbox_social_user>& socialUserVector
    )
{
    std::lock_guard<std::mutex> lock(social_manager::get_singleton_instance()->m_socialMangerLock);
    std::lock_guard<std::mutex> socialGroupLock(m_groupMutex);

    socialUserVector.clear();
    socialUserVector.reserve(m_userGroupVector.size());
    for (auto& user : m_userGroupVector)
    {
        socialUserVector.push_back(*user);
    }

    return xbox_live_result<void>();
}

bool
xbox_social_user_group::get_presence_filter_result(
    _In_ const xbox_social_user* user,
    _In_ presence_filter presenceFilter
    ) const
{
    switch (presenceFilter)
    {
    case presence_filter::all:
        return true;
    case presence_filter::all_offline:
        return user->presence_record().user_state() == user_presence_state::offline;
    case presence_filter::all_online:
        return user->presence_record().user_state() == user_presence_state::online;
    case presence_filter::all_title:
        return user->title_history().has_user_played();
    case presence_filter::title_offline:
        return user->presence_record().user_state() == user_presence_state::offline && user->title_history().has_user_played();
    case presence_filter::title_online:
        return user->presence_record().is_user_playing_title(m_titleId);
    default:
        return false;
    }
}

std::vector<xbox_social_user*>
xbox_social_user_group::get_users_from_xbox_user_ids(
    _In_ const std::vector<xbox_user_id_container>& xboxUserIds
    )
{
    std::vector<xbox_social_user*> returnVec;
    std::lock_guard<std::mutex> lock(m_groupMutex);
    for (auto& user : m_userGroupVector)
    {
        for (auto searchUser : xboxUserIds)
        {
            if (utils::char_t_cmp(searchUser.xbox_user_id(), user->xbox_user_id()) == 0)
            {
                returnVec.push_back(user);
            }
        }
    }

    return returnVec;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END