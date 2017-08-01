// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <time.h>
#include "Sample.h"

using namespace xbox::services;
using namespace xbox::services::clubs;
using namespace std;

void Sample::GetUsersClubAssociations(
    _In_ Windows::Xbox::System::User^ user
    )
{
    auto clubsService = m_liveResources->GetLiveContext()->clubs_service();

    clubsService.get_club_associations()
    .then([this](xbox_live_result<std::vector<club>> result)
    {
        if (!result.err())
        {
            m_clubs = result.payload();
            this->RefreshClubList();
        }
        else
        {
            m_console->Format(L"Get clubs failed with error: %u\n", result.err().value());
        }
    });
}

void Sample::RequerySelectedClub()
{
    if (m_selectedClub != nullptr)
    {
        auto clubsService = m_liveResources->GetLiveContext()->clubs_service();

        string_t id = m_selectedClub->id();

        clubsService.get_club(id)
            .then([this, id](xbox_live_result<club> result)
        {
            auto iter = std::find_if(m_clubs.begin(), m_clubs.end(), [&id](club& c)->bool
            {
                return c.id() == id;
            });

            if (iter != m_clubs.end())
            {
                *iter = result.payload();
            }
            this->RefreshClubList();
        });
    }
}

void Sample::CreateClub()
{
    auto clubsService = m_liveResources->GetLiveContext()->clubs_service();

    clubsService.create_club(L"Test club", club_type::public_club, L"48DA8BE2-73B0-4683-8075-8E8EE469D6E8")
    .then([this](xbox_live_result<club> result)
    {
        if (!result.err())
        {
            m_console->Write(L"Successfully created club!\n");
            m_clubs.push_back(result.payload());
            this->RefreshClubList();
        }
        else
        {
            m_console->Format(L"Create club failed with error: %u\n", result.err().value());
        }
    });
}

void Sample::InviteToClub()
{
    if (m_selectedClub != nullptr)
    {
        string_t xuidToInvite = L"2814631554770632";

        m_selectedClub->add_user_to_club(xuidToInvite)
        .then([this, xuidToInvite](xbox_live_result<std::vector<club_role>> result)
        {
            if (!result.err())
            {
                m_console->Format(L"Successfully sent invitation to user %s\n", xuidToInvite.data());
            }
            else
            {
                m_console->Format(L"Invite to club failed with error: %u\n", result.err().value());
            }
        });
    }
}

void Sample::JoinClub()
{
    if (m_selectedClub != nullptr)
    {
        m_selectedClub->add_user_to_club()
        .then([this](xbox_live_result<std::vector<club_role>> result)
        {
            if (!result.err())
            {
                m_console->Format(L"Successfully joined club %s\n", m_selectedClub->id().data());
                PrintClubRoles(result.payload());

                // Requery the club to refresh roles
                RequerySelectedClub();
            }
            else
            {
                m_console->Format(L"Join club failed with error: %u\n", result.err().value());
            }
        });
    }
}

void Sample::LeaveClub()
{
    if (m_selectedClub != nullptr)
    {
        m_selectedClub->remove_user_from_club()
        .then([this](xbox_live_result<std::vector<club_role>> result)
        {
            if (!result.err())
            {
                m_console->Format(L"Successfully left club %s\n", m_selectedClub->id().data());
                GetUsersClubAssociations(m_liveResources->GetUser());
            }
            else
            {
                m_console->Format(L"Leave club failed with error: %u\n", result.err().value());
            }
        });
    }
}

void Sample::RenameClub()
{
    if (m_selectedClub != nullptr)
    {
        m_selectedClub->rename_club(L"Renamed Club")
        .then([this](xbox_live_result<void> result)
        {
            if (!result.err())
            {
                m_console->Format(L"Successfully renamed club %s\n", m_selectedClub->id().data());
                
                // Requery the club to refresh name
                RequerySelectedClub();
            }
            else
            {
                m_console->Format(L"Rename club failed with error: %u\n", result.err().value());
            }
        });
    }
}

void Sample::DeleteClub()
{
    if (m_selectedClub != nullptr)
    {
        auto clubsService = m_liveResources->GetLiveContext()->clubs_service();

        string_t idToDelete = m_selectedClub->id();

        clubsService.delete_club(idToDelete)
        .then([this, idToDelete](xbox_live_result<void> result)
        {
            if (!result.err())
            {
                m_console->Format(L"Successfully deleted club %s\n", idToDelete.data());

                auto iter = std::find_if(m_clubs.begin(), m_clubs.end(), [&idToDelete](club& c)->bool 
                {
                    return c.id() == idToDelete;
                });

                if (iter != m_clubs.end())
                {
                    m_clubs.erase(iter);
                }
                this->RefreshClubList();
            }
            else
            {
                m_console->Format(L"Delete club failed with error: %u\n", result.err().value());
            }
        });
    }
}

void Sample::PrintClub(const xbox::services::clubs::club& club)
{
    stringstream_t ss;

    ss << "------------------------------------------" << endl;
    ss << "Club name: " << club.profile().name().value() << endl;
    ss << "Club ID: " << club.id() << endl;
    ss << "Owner Xuid: " << club.owner() << endl;
    ss << "Member count: " << club.members_count() << endl;
    ss << "------------------------------------------" << endl;

    m_console->Write(ss.str().data());   

    PrintClubRoles(club.viewer_roles());
}

void Sample::PrintClubRoles(const std::vector<xbox::services::clubs::club_role>& roles)
{
    stringstream_t ss;

    ss << "Roles:\n";
    for (const auto& role : roles)
    {
        ss << "--> ";
        switch (role)
        {
        case club_role::follower:
            ss << "follower\n";
            break;
        case club_role::owner:
            ss << "owner\n";
            break;
        case club_role::invited:
            ss << "invited\n";
            break;
        case club_role::member:
            ss << "member\n";
            break;
        case club_role::moderator:
            ss << "moderator\n";
            break;
        case club_role::recommended:
            ss << "recommended\n";
            break;
        case club_role::requested_to_join:
            ss << "requested to join\n";
            break;
        default:
            break;
        }
    }
    m_console->Write(ss.str().data());
}