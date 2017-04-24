// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_search_handle_details::multiplayer_search_handle_details() :
    m_visibility(multiplayer_session_visibility::unknown),
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_closed(false),
    m_maxMembersCount(0),
    m_membersCount(0)
{
}

const multiplayer_session_reference&
multiplayer_search_handle_details::session_reference() const
{
    return m_sessionReference;
}

const string_t&
multiplayer_search_handle_details::handle_id() const
{
    return m_handleId;
}

const std::vector<string_t>&
multiplayer_search_handle_details::session_owner_xbox_user_ids() const
{
    return m_sessionOwners;
}

const std::unordered_map<string_t, multiplayer_role_type>&
multiplayer_search_handle_details::role_types() const
{
    return m_roleTypes;
}

const std::vector<string_t>&
multiplayer_search_handle_details::tags() const
{
    return m_tags;
}

const std::unordered_map<string_t, double>&
multiplayer_search_handle_details::numbers_metadata() const
{
    return m_numbersMetadata;
}

const std::unordered_map<string_t, string_t>&
multiplayer_search_handle_details::strings_metadata() const
{
    return m_stringsMetadata;
}

multiplayer_session_visibility
multiplayer_search_handle_details::visibility() const
{
    return m_visibility;
}

multiplayer_session_restriction
multiplayer_search_handle_details::join_restriction() const
{
    return m_joinRestriction;
}

bool
multiplayer_search_handle_details::closed() const
{
    return m_closed;
}

uint32_t
multiplayer_search_handle_details::max_members_count() const
{
    return m_maxMembersCount;
}

uint32_t
multiplayer_search_handle_details::members_count() const
{
    return m_membersCount;
}

utility::datetime
multiplayer_search_handle_details::handle_creation_time() const
{
    return m_handleCreationTime;
}

const web::json::value&
multiplayer_search_handle_details::custom_session_properties_json() const
{
    return m_customSessionPropertiesJson;
}

xbox_live_result<multiplayer_search_handle_details>
multiplayer_search_handle_details::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_search_handle_details returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_search_handle_details>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    string_t type = utils::extract_json_string(json, _T("type"), errc);

    if (utils::str_icmp(type, _T("search")) != 0)
    {
        return xbox_live_result<multiplayer_search_handle_details>(xbox_live_error_code::json_error, "Unexpected content");
    }

    returnResult.m_handleId = utils::extract_json_string(json, _T("id"), errc);
    returnResult.m_handleCreationTime = utils::extract_json_time(json, _T("createTime"), errc);
    auto sessionReference = multiplayer_session_reference::_Deserialize(utils::extract_json_field(json, _T("sessionRef"), errc, false));
    if (sessionReference.err())
    {
        errc = sessionReference.err();
    }
    returnResult.m_sessionReference = sessionReference.payload();

    web::json::value customPropertiesObject = utils::extract_json_field(json, _T("customProperties"), errc, false);
    if (!customPropertiesObject.is_null())
    {
        returnResult.m_customSessionPropertiesJson = customPropertiesObject;
    }

    web::json::value searchAttributesObject = utils::extract_json_field(json, _T("searchAttributes"), errc, false);
    if (!searchAttributesObject.is_null())
    {
        returnResult.m_tags = utils::extract_json_vector<string_t>(utils::json_string_extractor, searchAttributesObject, _T("tags"), errc, false);
        auto stringsMetadataJson = utils::extract_json_field(searchAttributesObject, _T("strings"), errc, false);
        if (!stringsMetadataJson.is_null() && stringsMetadataJson.is_object())
        {
            web::json::object stringMetadataObj = stringsMetadataJson.as_object();
            for (const auto& stringsMetadata : stringMetadataObj)
            {
                returnResult.m_stringsMetadata[stringsMetadata.first] = stringsMetadata.second.as_string();
            }
        }

        auto numbersMetadataJson = utils::extract_json_field(searchAttributesObject, _T("numbers"), errc, false);
        if (!numbersMetadataJson.is_null() && numbersMetadataJson.is_object())
        {
            web::json::object numberMetadataObj = numbersMetadataJson.as_object();
            for (const auto& numbersMetadata : numberMetadataObj)
            {
                returnResult.m_numbersMetadata[numbersMetadata.first] = numbersMetadata.second.as_double();
            }
        }
    }

    web::json::value relatedInfoObject = utils::extract_json_field(json, _T("relatedInfo"), errc, false);
    if (!relatedInfoObject.is_null())
    {
        returnResult.m_membersCount = utils::extract_json_int(relatedInfoObject, _T("membersCount"), errc, true);
        returnResult.m_maxMembersCount = utils::extract_json_int(relatedInfoObject, _T("maxMembersCount"), errc, true);
        returnResult.m_joinRestriction = multiplayer_session_states::_Convert_string_to_multiplayer_session_restriction(
            utils::extract_json_string(relatedInfoObject, _T("joinRestriction"), errc)
            );
        returnResult.m_visibility = multiplayer_session_states::_Convert_string_to_session_visibility(
            utils::extract_json_string(relatedInfoObject, _T("visibility"), errc)
            );

        returnResult.m_closed = utils::extract_json_bool(relatedInfoObject, _T("closed"), errc);
        returnResult.m_sessionOwners = utils::extract_json_vector<string_t>(utils::json_string_extractor, relatedInfoObject, _T("sessionOwners"), errc, false);
    }

    auto roleInfo = utils::extract_json_field(json, _T("roleInfo"), errc, false);
    if (!roleInfo.is_null())
    {
        auto roleTypes = utils::extract_json_field(roleInfo, _T("roleTypes"), errc, false);
        if (!roleTypes.is_null() && roleTypes.is_object())
        {
            web::json::object roleTypesObj = roleTypes.as_object();
            for (const auto& roleType : roleTypesObj)
            {
                auto roleTypeResult = multiplayer_role_type::_Deserialize(roleType.second);
                if (roleTypeResult.err())
                {
                    errc = roleTypeResult.err();
                }
                returnResult.m_roleTypes[roleType.first] = roleTypeResult.payload();
            }
        }
    }

    return returnResult;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
