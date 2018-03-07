// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

using namespace xbox::services::presence;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

social_manager_presence_record::social_manager_presence_record() :
    m_userState(xbox::services::presence::user_presence_state::unknown),
    m_xboxUserId(0)
{
}

social_manager_presence_record::social_manager_presence_record(
    _In_ const xbox::services::presence::presence_record& presenceRecord
    )
{
    m_userState = presenceRecord.user_state();
    m_xboxUserId = utils::string_t_to_uint64(presenceRecord.xbox_user_id());

    uint32_t counter = 0;
    for (auto& deviceRecord : presenceRecord.presence_device_records())
    {
        for (auto& titleRecord : deviceRecord.presence_title_records())
        {
            m_presenceVec[counter] = social_manager_presence_title_record(titleRecord, deviceRecord.device_type());
            if (counter == NUM_PRESENCE_RECORDS - 1)
            {
                break;
            }
            ++counter;
        }
    }
}

xbox::services::presence::user_presence_state
social_manager_presence_record::user_state() const
{
    return m_userState;
}

bool
social_manager_presence_record::is_user_playing_title(
    _In_ uint32_t titleId
    ) const
{
    for (uint32_t i = 0; i < NUM_PRESENCE_RECORDS; ++i)
    {
        auto& presenceRecord = m_presenceVec[i];
        if (!presenceRecord._Is_null() && presenceRecord.title_id() == titleId)
        {
            return true;
        }
    }

    return false;
}

xbox_live_result<social_manager_presence_record>
social_manager_presence_record::_Deserialize(
    _In_ const web::json::value& json,
    _Inout_ std::error_code& errc
    )
{
    if (json.is_null()) return social_manager_presence_record();

    social_manager_presence_record returnObject;
    returnObject.m_userState = user_presence_state::offline;

    auto presenceDetailsJson = utils::extract_json_field(
        json,
        _T("presenceDetails"),
        errc,
        false
        );

    auto presenceStateString = utils::extract_json_string(json, "presenceState", errc);
    returnObject.m_userState = presence_record_internal::convert_string_to_user_presence_state(presenceStateString);
    auto presenceEntries = utils::extract_json_vector<social_manager_presence_title_record>(
        social_manager_presence_title_record::_Deserialize,
        presenceDetailsJson,
        errc,
        false
        );

    auto presenceSize = __min(NUM_PRESENCE_RECORDS, presenceEntries.size());
    for (uint32_t i = 0; i < presenceSize; ++i)
    {
        auto presenceEntry = presenceEntries[i];
        returnObject.m_presenceVec[i] = presenceEntry;
        if (presenceEntry.is_title_active())
        {
            returnObject.m_userState = user_presence_state::online;
        }
    }

    return returnObject;
}

uint64_t social_manager_presence_record::_Xbox_user_id() const
{
    return m_xboxUserId;
}

void
social_manager_presence_record::_Set_xbox_user_id(_In_ uint64_t xboxUserId)
{
    m_xboxUserId = xboxUserId;
}

const std::vector<social_manager_presence_title_record>
social_manager_presence_record::presence_title_records() const
{
    std::vector<social_manager_presence_title_record> returnVec;
    for (uint32_t i = 0; i < NUM_PRESENCE_RECORDS; ++i)
    {
        auto presenceRecord = m_presenceVec[i];
        if (!presenceRecord._Is_null())
        {
            returnVec.push_back(presenceRecord);
        }
    }

    return returnVec;
}

bool
social_manager_presence_record::_Compare(
    _In_ const social_manager_presence_record& presenceRecord
    ) const
{
    if (presenceRecord.m_userState != m_userState)
    {
        return true;
    }


    for (auto& titleRecord : presenceRecord.m_presenceVec)
    {
        bool wasTitleFound = false;
        for (auto& compareTitleRecord : m_presenceVec)
        {
            if (compareTitleRecord.title_id() == titleRecord.title_id()
                && utils::str_icmp(compareTitleRecord.presence_text(), titleRecord.presence_text()) == 0
                && compareTitleRecord.is_title_active() == titleRecord.is_title_active()
                && compareTitleRecord.is_broadcasting() == titleRecord.is_broadcasting()
                )
            {
                wasTitleFound = true;
            }
        }

        if (!wasTitleFound)
        {
            return true;
        }
    }

    return false;
}

void
social_manager_presence_record::_Update_device(
    _In_ presence_device_type deviceType,
    _In_ bool isUserLoggedIn
    )
{
    bool isNull = true;
    if(!isUserLoggedIn)
    {
        for (uint32_t i = 0; i < NUM_PRESENCE_RECORDS; ++i)
        {
            auto presenceTitleEntry = m_presenceVec[i];
            if (presenceTitleEntry.device_type() == deviceType)
            {
                m_presenceVec[i] = social_manager_presence_title_record();
            }
            else
            {
                isNull |= presenceTitleEntry._Is_null();
            }
        }
    }

    if (isNull && !isUserLoggedIn)
    {
        m_userState = user_presence_state::offline;
    }
}

void
social_manager_presence_record::_Remove_title(
    _In_ uint32_t titleId
    )
{
    for (auto i = 0; i < NUM_PRESENCE_RECORDS; ++i)
    {
        auto presenceEntry = m_presenceVec[i];
        if (presenceEntry.title_id() == titleId)
        {
            m_presenceVec[i] = social_manager_presence_title_record();
            break;
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END