// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_activity_details::multiplayer_activity_details() :
    m_titleId(0),
    m_visibility(multiplayer_session_visibility::unknown),
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_closed(false),
    m_maxMembersCount(0),
    m_membersCount(0)
{
}

const multiplayer_session_reference&
multiplayer_activity_details::session_reference() const
{
    return m_sessionReference;
}

const string_t&
multiplayer_activity_details::handle_id() const
{
    return m_handleId;
}

uint32_t
multiplayer_activity_details::title_id() const
{
    return m_titleId;
}

multiplayer_session_visibility
multiplayer_activity_details::visibility() const
{
    return m_visibility;
}

multiplayer_session_restriction
multiplayer_activity_details::join_restriction() const
{
    return m_joinRestriction;
}

bool
multiplayer_activity_details::closed() const
{
    return m_closed;
}

const string_t&
multiplayer_activity_details::owner_xbox_user_id() const
{
    return m_ownerXboxUserId;
}

uint32_t
multiplayer_activity_details::max_members_count() const
{
    return m_maxMembersCount;
}

uint32_t
multiplayer_activity_details::members_count() const
{
    return m_membersCount;
}

const web::json::value&
multiplayer_activity_details::custom_session_properties_json() const
{
    return m_customSessionPropertiesJson;
}

xbox_live_result<multiplayer_activity_details>
multiplayer_activity_details::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_activity_details returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_activity_details>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    string_t type = utils::extract_json_string(json, _T("type"), errc);
    
    if (type.compare(_T("activity")) != 0)
    {
        return xbox_live_result<multiplayer_activity_details>(xbox_live_error_code::json_error, "Unexpected content");
    }

    returnResult.m_handleId = utils::extract_json_string(json, _T("id"), errc);
    auto sessionReference = multiplayer_session_reference::_Deserialize(
        utils::extract_json_field(json, _T("sessionRef"), errc, false)
        );

    if (sessionReference.err())
    {
        errc = sessionReference.err();
    }
    returnResult.m_sessionReference = sessionReference.payload();

    returnResult.m_ownerXboxUserId = utils::extract_json_string(json, _T("ownerXuid"), errc);

    returnResult.m_titleId = utils::string_t_to_uint32(utils::extract_json_string(json, _T("titleId"), errc));

    web::json::value relatedInfoObject = utils::extract_json_field(json, _T("relatedInfo"), errc, false);
    if (!relatedInfoObject.is_null())
    {
        returnResult.m_membersCount = utils::extract_json_int(relatedInfoObject, _T("membersCount"), errc);
        returnResult.m_maxMembersCount = utils::extract_json_int(relatedInfoObject, _T("maxMembersCount"), errc);
        returnResult.m_joinRestriction = multiplayer_session_states::_Convert_string_to_multiplayer_session_restriction(
            utils::extract_json_string(relatedInfoObject, _T("joinRestriction"), errc)
            );
        returnResult.m_visibility = multiplayer_session_states::_Convert_string_to_session_visibility(
            utils::extract_json_string(relatedInfoObject, _T("visibility"), errc)
            );

        returnResult.m_closed = utils::extract_json_bool(relatedInfoObject, _T("closed"), errc);
    }

    web::json::value customPropertiesObject = utils::extract_json_field(json, _T("customProperties"), errc, false);
    if (!customPropertiesObject.is_null())
    {
        returnResult.m_customSessionPropertiesJson = customPropertiesObject;
    }

    return returnResult;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END