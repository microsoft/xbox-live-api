// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "utils.h"
#include "xsapi/multiplayer.h"
#include <mutex>
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_properties::multiplayer_session_properties() :
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_readRestriction(multiplayer_session_restriction::unknown)
{
    m_matchmakingTargetSessionConstants = web::json::value::object();
    m_customPropertiesJson = web::json::value::object();
    m_sessionRequest = std::make_shared<multiplayer_session_request>();
}

multiplayer_session_properties& multiplayer_session_properties::_Deep_copy(
    _In_ const multiplayer_session_properties& other
    )
{
    m_customPropertiesJson = other.m_customPropertiesJson;
    m_keywords = other.m_keywords;
    m_sessionOwnerIndices = other.m_sessionOwnerIndices;
    m_turnCollection = other.m_turnCollection;
    m_joinRestriction = other.m_joinRestriction;
    m_matchmakingTargetSessionConstants = other.m_matchmakingTargetSessionConstants;
    m_host = other.m_host;
    m_serverConnectionString = other.m_serverConnectionString;
    m_serverConnectionStringCandidates = other.m_serverConnectionStringCandidates;
    m_closed = other.m_closed;
    m_locked = other.m_locked;
    m_allocateCloudCompute = other.m_allocateCloudCompute;

    // We will set this from the session deep_copy.
    m_sessionRequest = std::make_shared<multiplayer_session_request>();

    return *this;
}

const std::vector<string_t>&
multiplayer_session_properties::keywords() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_keywords;
}

void 
multiplayer_session_properties::set_keywords(
    _In_ std::vector<string_t> keywords
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    m_keywords = std::move(keywords);
    m_sessionRequest->set_session_properties_keywords(m_keywords);
}

multiplayer_session_restriction 
multiplayer_session_properties::join_restriction() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_joinRestriction;
}

std::error_code
multiplayer_session_properties::set_join_restriction(
    _In_ multiplayer_session_restriction joinRestriction
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    if (joinRestriction < multiplayer_session_restriction::none ||
        joinRestriction > multiplayer_session_restriction::followed)
    {
        return xbox_live_error_code::invalid_argument;
    }

    m_joinRestriction = joinRestriction;
    m_sessionRequest->set_join_restriction(m_joinRestriction);
    return xbox_live_error_code::no_error;
}

multiplayer_session_restriction
multiplayer_session_properties::read_restriction() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_readRestriction;
}

std::error_code
multiplayer_session_properties::set_read_restriction(
    _In_ multiplayer_session_restriction readRestriction
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    if (readRestriction < multiplayer_session_restriction::none ||
        readRestriction > multiplayer_session_restriction::followed)
    {
        return xbox_live_error_code::invalid_argument;
    }

    m_readRestriction = std::move(readRestriction);
    m_sessionRequest->set_read_restriction(m_readRestriction);
    return xbox_live_error_code::no_error;
}

const std::vector<std::shared_ptr<multiplayer_session_member>>& 
multiplayer_session_properties::turn_collection() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_turnCollection;
}

const web::json::value&
multiplayer_session_properties::matchmaking_target_session_constants_json() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_matchmakingTargetSessionConstants;
}

const web::json::value&
multiplayer_session_properties::session_custom_properties_json() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_customPropertiesJson;
}

const string_t& 
multiplayer_session_properties::matchmaking_server_connection_string() const
{
   return m_serverConnectionString;
}

const std::vector<string_t>& 
multiplayer_session_properties::server_connection_string_candidates() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_serverConnectionStringCandidates;
}

const std::vector<uint32_t>&
multiplayer_session_properties::session_owner_indices() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_sessionOwnerIndices;
}

const string_t& 
multiplayer_session_properties::host_device_token() const
{
    return m_host;
}

bool 
multiplayer_session_properties::closed() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_closed;
}

bool
multiplayer_session_properties::locked() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_locked;
}

bool 
multiplayer_session_properties::allocate_cloud_compute() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    return m_allocateCloudCompute;
}

void 
multiplayer_session_properties::_Initialize(
    _In_ std::shared_ptr<multiplayer_session_request> sessionRequest,
    _In_ std::vector<std::shared_ptr<multiplayer_session_member>> members
    )
{
    m_sessionRequest = std::move(sessionRequest);
    std::vector<std::shared_ptr<multiplayer_session_member>> turnCollection;
    if (!m_turnIndexList.empty() && !members.empty())
    {
        for (uint32_t memberIndex : m_turnIndexList)
        {
            for (const auto& member : members)
            {
                if (member->member_id() == memberIndex)
                {
                    turnCollection.push_back(member);
                    break;
                }
            }
        }
    }
    m_turnCollection = turnCollection;
}

std::error_code
multiplayer_session_properties::set_turn_collection(
    _In_ std::vector<std::shared_ptr<multiplayer_session_member>> turnCollection
    )
{
    if (turnCollection.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    std::vector<uint32_t> turnIndexVector;

    for (const auto& member : turnCollection)
    {
        turnIndexVector.push_back(member->member_id());
    }
    m_sessionRequest->set_session_properties_turns(turnIndexVector);
    m_turnCollection = std::move(turnCollection);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session_properties::_Set_session_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    web::json::value customProperty;
    if (!valueJson.is_null())
    {
        customProperty = valueJson;
    }

    if (m_sessionRequest->session_properties_custom_properties().is_null())
    {
        m_sessionRequest->set_session_custom_properties(web::json::value());
    }

    web::json::value customProperties = m_sessionRequest->session_properties_custom_properties();
    if (customProperty.is_null())
    {
        customProperties[name] = web::json::value::null();
        m_sessionRequest->set_session_custom_properties(customProperties);
    }
    else
    {
        customProperties[name] = customProperty;
    }

    m_sessionRequest->set_session_custom_properties(customProperties);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session_properties::_Delete_session_custom_property_json(
    _In_ const string_t& name
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    _Set_session_custom_property_json(name, web::json::value::null());

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session_properties::_Set_matchmaking_target_session_constants_json(
    _In_ const web::json::value& matchmakingTargetSessionConstantsJson
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdPropertyLock);
    m_sessionRequest->set_write_matchmaking_session_constants(true);
    m_matchmakingTargetSessionConstants = matchmakingTargetSessionConstantsJson;
    m_sessionRequest->set_session_properties_target_sessions_constants( m_matchmakingTargetSessionConstants );

    return xbox_live_error_code::no_error;
}

xbox_live_result<multiplayer_session_properties>
multiplayer_session_properties::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_properties returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_session_properties>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value systemJson = utils::extract_json_field(json, _T("system"), errc, true);
    web::json::value systemMatchmakingJson = utils::extract_json_field(systemJson, _T("matchmaking"), errc, false);
    web::json::value systemMatchmakingClientResultJson = utils::extract_json_field(systemMatchmakingJson, _T("clientResult"), errc, false);

    returnResult.m_keywords = utils::extract_json_vector<string_t>(utils::json_string_extractor, systemJson, _T("keywords"), errc, false);
    returnResult.m_sessionOwnerIndices = utils::extract_json_vector<uint32_t>(utils::json_int_extractor, systemJson, _T("owners"), errc, false);
    returnResult.m_turnIndexList = utils::extract_json_vector<uint32_t>(utils::json_int_extractor, systemJson, _T("turn"), errc, false);
    string_t joinRestrictionString = utils::extract_json_string(systemJson, _T("joinRestriction"), errc);
    string_t readRestrictionString = utils::extract_json_string(systemJson, _T("readRestriction"), errc);
    if (!joinRestrictionString.empty())
    {
        returnResult.m_joinRestriction = multiplayer_session_states::_Convert_string_to_multiplayer_session_restriction(joinRestrictionString);
    }

    if (!readRestrictionString.empty())
    {
        returnResult.m_readRestriction = multiplayer_session_states::_Convert_string_to_multiplayer_session_restriction(readRestrictionString);
    }

    returnResult.m_closed = utils::extract_json_bool(systemJson, _T("closed"), errc);
    returnResult.m_locked = utils::extract_json_bool(systemJson, _T("locked"), errc);
    returnResult.m_allocateCloudCompute = utils::extract_json_bool(systemJson, _T("allocateCloudCompute"), errc);

    returnResult.m_matchmakingTargetSessionConstants = utils::extract_json_field(systemMatchmakingJson, _T("targetSessionConstants"), errc, false);
    returnResult.m_customPropertiesJson = utils::extract_json_field(json, _T("custom"), errc, false);
    
    returnResult.m_host = utils::extract_json_string(systemJson, _T("host"), errc);
    returnResult.m_serverConnectionString = utils::extract_json_string(systemMatchmakingJson, _T("serverConnectionString"), errc);
    returnResult.m_serverConnectionStringCandidates = utils::extract_json_vector<string_t>(utils::json_string_extractor, systemJson, _T("serverConnectionStringCandidates"), errc, false);
    
    return xbox_live_result<multiplayer_session_properties>(returnResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END