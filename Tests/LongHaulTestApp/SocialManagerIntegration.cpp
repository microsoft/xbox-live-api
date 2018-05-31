#include "pch.h"
#include "GameLogic\Game.h"

using namespace LongHaulTestApp;

#define NUM_OF_USER_GROUP_TYPES 2
#define NUM_OF_PRESENCE_FILTERS 7
#define NUM_OF_RELATIONSHIP_FILTERS 2

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////

void Game::TestSocialManagerFlow()
{
    m_testGroupType = (XblSocialUserGroupType)(((int)m_testGroupType + 1) % NUM_OF_USER_GROUP_TYPES);

    Log("===== Starting TestSocialManagerFlow =====");
    Log("[Test] " + GroupTypeToString() + " group type");
    AddLocalUserToSocialManager();
}


///////////////////////////////////////
//////        Test Utils         //////
///////////////////////////////////////

void Game::AddLocalUserToSocialManager()
{
    Log("XblSocialManagerAddLocalUser");
    XblSocialManagerAddLocalUser(m_user, XblSocialManagerExtraDetailLevel::XblSocialManagerExtraDetailLevel_All);

    Game* pThis = this;
    WaitForSocialEvent(XblSocialManagerEventType::XblSocialManagerEventType_LocalUserAdded, [pThis](XblSocialManagerEvent e) {
        pThis->Log("XblSocialManagerGetLocalUsers");
        xbl_user_handle* users;
        uint32_t size;
        XblSocialManagerGetLocalUsers(&users, &size);

        pThis->CreateSocialUserGroup();
    });
}

void Game::RemoveLocalUserFromSocialManager()
{
    Log("XblSocialManagerRemoveLocalUser");
    XblSocialManagerRemoveLocalUser(m_user);

    Game* pThis = this;
    WaitForSocialEvent(XblSocialManagerEventType::XblSocialManagerEventType_LocalUserRemoved, [pThis](XblSocialManagerEvent e) {
        pThis->Log("===== Finished TestSocialManagerFlow =====");
        pThis->EndTest();
    });
}

void Game::CreateSocialUserGroup()
{
    if (m_testGroupType == XblSocialUserGroupType::XblSocialUserGroupType_FilterType)
    {
        Log("[Test] Creating social user group with combination " + PresenceFilterToString() + ", " + RelationshipFilterToString());
        Log("XblSocialManagerCreateSocialUserGroupFromFilters");
        XblSocialManagerCreateSocialUserGroupFromFilters(m_user, m_presenceFilter, m_relationshipFilter, &m_socialUserGroup);
    }
    else
    {
        std::vector<uint64_t> xuids;
        xuids.push_back(m_xuid);

        Log("XblSocialManagerCreateSocialUserGroupFromList");
        XblSocialManagerCreateSocialUserGroupFromList(m_user, xuids.data(), (uint32_t)xuids.size(), &m_socialUserGroup);
    }


    Game* pThis = this;
    WaitForSocialEvent(XblSocialManagerEventType::XblSocialManagerEventType_SocialUserGroupLoaded, [pThis](XblSocialManagerEvent e) {
        pThis->TestSocialUserGroup();
    });
}

void Game::TestSocialUserGroup()
{
    if (m_socialUserGroup->trackedUsersCount > 0)
    {
        Log("XblSocialManagerUserGroupGetUsersTrackedByGroup");
        std::vector<uint64_t> xuids(m_socialUserGroup->trackedUsersCount);
        XblSocialManagerUserGroupGetUsersTrackedByGroup(m_socialUserGroup, m_socialUserGroup->trackedUsersCount, xuids.data());

        Log("XblSocialManagerUserGroupGetUsersFromXboxUserIds");
        std::vector<XblSocialManagerUser> xboxSocialUsers(m_socialUserGroup->trackedUsersCount);
        uint32_t size;
        XblSocialManagerUserGroupGetUsersFromXboxUserIds(m_socialUserGroup, xuids.data(), (uint32_t)xuids.size(), xboxSocialUsers.data(), &size);
    }

    if (m_socialUserGroup->usersCount > 0)
    {
        Log("XblSocialManagerUserGroupGetUsers");
        std::vector<XblSocialManagerUser> users(m_socialUserGroup->usersCount);
        XblSocialManagerUserGroupGetUsers(m_socialUserGroup, m_socialUserGroup->usersCount, users.data());

        Log("XblSocialManagerPresenceRecordIsUserPlayingTitle");
        for (auto user : users)
        {
            XblSocialManagerPresenceRecordIsUserPlayingTitle(&(user.presenceRecord), m_config->titleId);
        }
    }

    Log("XblSocialManagerDestroySocialUserGroup");
    XblSocialManagerDestroySocialUserGroup(m_socialUserGroup);
    RemoveLocalUserFromSocialManager();
}

///////////////////////////////////////
//////      Manager Utils        //////
///////////////////////////////////////

void Game::NextSocialManagerTestConfig()
{
    if (m_relationshipFilter == XblRelationshipFilter_Friends)
    {
        m_relationshipFilter = XblRelationshipFilter_Favorite;
    }
    else
    {
        m_relationshipFilter = XblRelationshipFilter_Friends;
        m_presenceFilter = (XblPresenceFilter)(((int)m_presenceFilter + 1) % NUM_OF_PRESENCE_FILTERS);
    }
}

void Game::SocialManagerIntegrationUpdate()
{
    XblSocialManagerEvent* events;
    uint32_t eventsSize;

    XblSocialManagerDoWork(&events, &eventsSize);
    PrintSocialManagerDoWork(events, eventsSize);

    if (m_waitingForEvent)
    {
        for (uint32_t i = 0; i < eventsSize; i++)
        {
            if (events[i].eventType == m_eventType)
            {
                m_waitingForEvent = false;
                m_eventTypeCallback(events[i]);
                break;
            }
        }
    }
}

void Game::WaitForSocialEvent(XblSocialManagerEventType eventType, std::function<void(XblSocialManagerEvent)> callback)
{
    m_waitingForEvent = true;
    m_eventType = eventType;
    m_eventTypeCallback = callback;
}

void Game::PrintSocialManagerDoWork(XblSocialManagerEvent* events, uint32_t size)
{
    if (size > 0)
    {
        Log("[DoWork] Processed " + std::to_string(size) + " events");

        for (uint32_t i = 0; i < size; i++)
        {
            Log("[DoWork][Event] Processed " + SocialEventTypeToString(events[i].eventType));
            if (events[i].err)
            {
                Log("[DoWork][Event][Error] Error occured " + std::to_string(events[i].err));
            }
            else
            {
                if (events[i].usersAffectedCount > 0)
                {
                    std::vector<uint64_t> affectedUsers(events[i].usersAffectedCount);

                    XblSocialManagerEventGetUsersAffected(&events[i], affectedUsers.data());

                    Log("[DoWork][Event] " + std::to_string(events[i].usersAffectedCount) + " users affected");
                    for (uint64_t xuid : affectedUsers)
                    {
                        Log(std::to_string(xuid));
                    }
                }
            }
        }
    }
}

string Game::GroupTypeToString() {
    string type = "unknown";

    switch (m_testGroupType)
    {
    case XblSocialUserGroupType::XblSocialUserGroupType_FilterType: type = "filter"; break;
    case XblSocialUserGroupType::XblSocialUserGroupType_UserListType: type = "user list"; break;
    }

    return type;
}


string Game::SocialEventTypeToString(XblSocialManagerEventType eventType)
{
    string type = "Unknown";

    switch (eventType)
    {
    case XblSocialManagerEventType_UsersAddedToSocialGraph: type = "UsersAddedToSocialGraph"; break;
    case XblSocialManagerEventType_UsersRemovedFromSocialGraph: type = "UsersRemovedFromSocialGraph"; break;
    case XblSocialManagerEventType_PresenceChanged: type = "PresenceChanged"; break;
    case XblSocialManagerEventType_ProfilesChanged: type = "ProfilesChanged"; break;
    case XblSocialManagerEventType_SocialRelationshipsChanged: type = "SocialRelationshipsChanged"; break;
    case XblSocialManagerEventType_LocalUserAdded: type = "LocalUserAdded"; break;
    case XblSocialManagerEventType_LocalUserRemoved: type = "LocalUserRemoved"; break;
    case XblSocialManagerEventType_SocialUserGroupLoaded: type = "SocialUserGroupLoaded"; break;
    case XblSocialManagerEventType_SocialUserGroupUpdated: type = "SocialUserGroupUpdated"; break;
    case XblSocialManagerEventType_UnknownEvent: type = "UnknownEvent"; break;
    }

    return type;
}

string Game::PresenceFilterToString()
{
    string filter = "Unknown";

    switch (m_presenceFilter)
    {
    case XblPresenceFilter_Unknown: filter = "Unknown"; break;
    case XblPresenceFilter_TitleOnline: filter = "Title Online"; break;
    case XblPresenceFilter_TitleOffline: filter = "Title Offline"; break;
    case XblPresenceFilter_AllOnline: filter = "All Online"; break;
    case XblPresenceFilter_AllOffline: filter = "AllOffline"; break;
    case XblPresenceFilter_AllTitle: filter = "AllTitle"; break;
    case XblPresenceFilter_All: filter = "All"; break;
    }

    return filter;
}

string Game::RelationshipFilterToString()
{
    string filter = "Unknown";

    switch (m_relationshipFilter)
    {
    case XblRelationshipFilter_Friends: filter = "Friends"; break;
    case XblRelationshipFilter_Favorite: filter = "Favorites"; break;
    }

    return filter;
}