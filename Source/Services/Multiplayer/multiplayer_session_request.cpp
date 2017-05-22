// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "user_context.h"
#include "xsapi/multiplayer.h"
#include "utils.h"
#include <mutex>
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

std::shared_ptr<multiplayer_session_request> multiplayer_session_request::create_deep_copy()
{
    auto copy = std::make_shared<multiplayer_session_request>();
    copy->deep_copy_from(*this);
    return copy;
}

void multiplayer_session_request::deep_copy_from(
    _In_ const multiplayer_session_request& other
    )
{
    xbox::services::system::xbox_live_mutex& lock = const_cast<multiplayer_session_request&>(other).m_lock;
    std::lock_guard<std::mutex> guard(lock.get());

    m_sessionReference = other.m_sessionReference;
    m_sessionConstants = other.m_sessionConstants == nullptr ? nullptr : other.m_sessionConstants;
    m_memberRequest = other.m_memberRequest;
    m_sessionPropertiesKeywords = other.m_sessionPropertiesKeywords;
    m_sessionPropertiesTurns = other.m_sessionPropertiesTurns;
    m_joinRestriction = other.m_joinRestriction;
    m_readRestriction = other.m_readRestriction;
    m_sessionPropertiesCustomProperties = other.m_sessionPropertiesCustomProperties;
    m_writeMatchmakingClientResult = other.m_writeMatchmakingClientResult;
    m_writeMatchmakingSessionConstants = other.m_writeMatchmakingSessionConstants;
    m_sessionPropertiesTargetSessionsConstants = other.m_sessionPropertiesTargetSessionsConstants;
    m_writeInitializationStatus = other.m_writeInitializationStatus;
    m_initializationSucceeded = other.m_initializationSucceeded;
    m_writeHostDeviceToken = other.m_writeHostDeviceToken;
    m_hostDeviceToken = other.m_hostDeviceToken;
    m_writeMatchmakingServerConnectionPath = other.m_writeMatchmakingServerConnectionPath;
    m_matchmakingServerConnectionPath = other.m_matchmakingServerConnectionPath;
    m_writeMatchmakingResubmit = other.m_writeMatchmakingResubmit;
    m_matchmakingMatchResubmit = other.m_matchmakingMatchResubmit;
    m_writeServerConnectionStringCandidates = other.m_writeServerConnectionStringCandidates;
    m_serverConnectionStringCandidates = other.m_serverConnectionStringCandidates;
    m_members = other.m_members;
    m_servers = other.m_servers;
    m_memberRequestIndex = other.m_memberRequestIndex;
    m_writeClosed = other.m_writeClosed;
    m_closed = other.m_closed;
    m_writeLocked = other.m_writeLocked;
    m_locked = other.m_locked;
    m_writeAllocateCloudCompute = other.m_writeAllocateCloudCompute;
    m_allocateCloudCompute = other.m_allocateCloudCompute;
}

multiplayer_session_request::multiplayer_session_request() :
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_readRestriction(multiplayer_session_restriction::unknown),
    m_writeMatchmakingClientResult(false),
    m_writeMatchmakingSessionConstants(false),
    m_writeInitializationStatus(false),
    m_initializationSucceeded(false),
    m_writeHostDeviceToken(false),
    m_writeMatchmakingServerConnectionPath(false),
    m_writeMatchmakingResubmit(false),
    m_matchmakingMatchResubmit(false),
    m_writeServerConnectionStringCandidates(false),
    m_memberRequestIndex(0),
    m_bLeaveSession(false),
    m_writeClosed(false),
    m_closed(false),
    m_writeLocked(false),
    m_locked(false),
    m_writeAllocateCloudCompute(false),
    m_allocateCloudCompute(false)
{
    m_sessionConstants = std::make_shared<multiplayer_session_constants>();
    m_sessionPropertiesCustomProperties = web::json::value();
    m_sessionPropertiesTargetSessionsConstants = web::json::value();
    m_servers = web::json::value();
}

multiplayer_session_request::multiplayer_session_request(
    _In_opt_ std::shared_ptr<multiplayer_session_constants> multiplayerSessionConstants
    ) :
    m_sessionConstants(std::move(multiplayerSessionConstants)),
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_readRestriction(multiplayer_session_restriction::unknown),
    m_writeMatchmakingClientResult(false),
    m_writeMatchmakingSessionConstants(false),
    m_writeInitializationStatus(false),
    m_initializationSucceeded(false),
    m_writeHostDeviceToken(false),
    m_writeMatchmakingServerConnectionPath(false),
    m_writeMatchmakingResubmit(false),
    m_matchmakingMatchResubmit(false),
    m_writeServerConnectionStringCandidates(false),
    m_memberRequestIndex(0),
    m_bLeaveSession(false),
    m_writeClosed(false),
    m_closed(false),
    m_writeLocked(false),
    m_locked(false),
    m_writeAllocateCloudCompute(false),
    m_allocateCloudCompute(false)
{
}

std::shared_ptr<multiplayer_session_member_request>
multiplayer_session_request::add_member_request(
    _In_ bool writeRequest,
    _In_ bool isMe,
    _In_ string_t xboxUserId,
    _In_ web::json::value memberConstants,
    _In_ bool addInitializePropertyToRequest,
    _In_ bool initializedRequested
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    stringstream_t memberId;
    string_t meString = isMe ? _T("me") : _T("reserve_");
    memberId << meString;
    if (!isMe)
    {
        memberId << m_memberRequestIndex;
    }
    std::shared_ptr<multiplayer_session_member_request> memberRequest = std::make_shared<multiplayer_session_member_request>(
        writeRequest,
        memberId.str(),
        std::move(xboxUserId),
        std::move(memberConstants),
        addInitializePropertyToRequest,
        initializedRequested
        );

    if (!isMe)
    {
        m_memberRequestIndex++;
    }

    m_members.push_back(memberRequest);
    return memberRequest;
}

const multiplayer_session_reference&
multiplayer_session_request::session_reference() const
{
    return m_sessionReference;
}

void
multiplayer_session_request::set_session_reference(
    _In_ multiplayer_session_reference sessionReference
    )
{
    m_sessionReference = std::move(sessionReference);
}

void
multiplayer_session_request::leave_session()
{
    m_bLeaveSession = true;
}

void 
multiplayer_session_request::set_leave_session(
    _In_ bool leaveSession
    )
{
    m_bLeaveSession = leaveSession;
}

std::shared_ptr<multiplayer_session_constants>
multiplayer_session_request::session_constants() const
{
    return m_sessionConstants;
}

void
multiplayer_session_request::set_session_constants(
    _In_ std::shared_ptr<multiplayer_session_constants> sessionConstants
    )
{
    m_sessionConstants = sessionConstants;
}

const std::vector<string_t>&
multiplayer_session_request::session_properties_keywords() const
{
    return m_sessionPropertiesKeywords;
}

void 
multiplayer_session_request::set_session_properties_keywords(
    _In_ std::vector<string_t> keywords
    )
{
    m_sessionPropertiesKeywords = std::move(keywords);
}

const std::vector<uint32_t>&
multiplayer_session_request::session_properties_turns() const
{
    return m_sessionPropertiesTurns;
}

void 
multiplayer_session_request::set_session_properties_turns(
    _In_ std::vector<uint32_t> sessionProperties
    )
{
    m_sessionPropertiesTurns = std::move(sessionProperties);
}

multiplayer_session_restriction 
multiplayer_session_request::join_restriction() const
{
    return m_joinRestriction;
}

void 
multiplayer_session_request::set_join_restriction(
    _In_ multiplayer_session_restriction joinRestriction
    )
{
    m_joinRestriction = joinRestriction;
}

multiplayer_session_restriction
multiplayer_session_request::read_restriction() const
{
    return m_readRestriction;
}

void
multiplayer_session_request::set_read_restriction(
    _In_ multiplayer_session_restriction readRestriction
    )
{
    m_readRestriction = readRestriction;
}

const web::json::value&
multiplayer_session_request::session_properties_custom_properties() const
{
    return m_sessionPropertiesCustomProperties;
}

void
multiplayer_session_request::set_session_custom_properties(
    _In_ web::json::value sessionCustomProperties
    )
{
    m_sessionPropertiesCustomProperties = std::move(sessionCustomProperties);
}

bool
multiplayer_session_request::write_matchmaking_client_result() const
{
    return m_writeMatchmakingClientResult;
}

void
multiplayer_session_request::set_write_matchmaking_client_result(
_In_ bool writeMatchmakingClientResult
)
{
    m_writeMatchmakingClientResult = writeMatchmakingClientResult;
}

bool
multiplayer_session_request::write_matchmaking_session_constants() const
{
    return m_writeMatchmakingSessionConstants;
}

void
multiplayer_session_request::set_write_matchmaking_session_constants(
    _In_ bool matchmakingSessionConstant
    )
{
    m_writeMatchmakingSessionConstants = matchmakingSessionConstant;
}

const web::json::value&
multiplayer_session_request::session_properties_target_sessions_constants() const
{
    return m_sessionPropertiesTargetSessionsConstants;
}

void
multiplayer_session_request::set_session_properties_target_sessions_constants(
    _In_ web::json::value targetSessionsConstants
    )
{
    m_sessionPropertiesTargetSessionsConstants = std::move(targetSessionsConstants);
}

bool
multiplayer_session_request::write_initialization_status() const
{
    return m_writeInitializationStatus;
}

void
multiplayer_session_request::set_write_initialization_status(
    _In_ bool status
    )
{
    m_writeInitializationStatus = status;
}

bool
multiplayer_session_request::initialization_succeeded() const
{
    return m_initializationSucceeded;
}

void 
multiplayer_session_request::set_initialization_succeeded(
    _In_ bool succeeded
    )
{
    m_initializationSucceeded = succeeded;
}

bool
multiplayer_session_request::write_host_device_token() const
{
    return m_writeHostDeviceToken;
}

void
multiplayer_session_request::set_write_host_device_token(
    _In_ bool writeToken
    )
{
    m_writeHostDeviceToken = writeToken;
}

const string_t&
multiplayer_session_request::host_device_token() const
{
    return m_hostDeviceToken;
}

void
multiplayer_session_request::set_host_device_token(
    _In_ string_t deviceToken
    )
{
    m_hostDeviceToken = std::move(deviceToken);
}

bool
multiplayer_session_request::write_matchmaking_server_connection_path() const
{
    return m_writeMatchmakingServerConnectionPath;
}


void 
multiplayer_session_request::set_write_matchmaking_server_connection_path(
    _In_ bool writePath
    )
{
    m_writeMatchmakingServerConnectionPath = writePath;
}

const string_t&
multiplayer_session_request::matchmaking_server_connection_path() const
{
    return m_matchmakingServerConnectionPath;
}


void 
multiplayer_session_request::set_matchmaking_server_connection_path(
    _In_ string_t serverPath
    )
{
    m_matchmakingServerConnectionPath = std::move(serverPath);
}

bool
multiplayer_session_request::write_matchmaking_resubmit() const
{
    return m_writeMatchmakingResubmit;
}

void
multiplayer_session_request::set_write_matchmaking_resubmit(
    _In_ bool writeMatchmakingResubmit
    )
{
    m_writeMatchmakingResubmit = writeMatchmakingResubmit;
}

bool
multiplayer_session_request::matchmaking_match_resubmit() const
{
    return m_matchmakingMatchResubmit;
}

void 
multiplayer_session_request::set_matchmaking_match_resubmit(
    _In_ bool matchmakingResubmit
    )
{
    m_matchmakingMatchResubmit = matchmakingResubmit;
}

bool
multiplayer_session_request::write_server_connection_string_candidates() const
{
    return m_writeServerConnectionStringCandidates;
}

void 
multiplayer_session_request::set_write_server_connection_string_candidates(
    _In_ bool writeCandidates
    )
{
    m_writeServerConnectionStringCandidates = writeCandidates;
}

const std::vector<string_t>& 
multiplayer_session_request::server_connection_string_candidates() const
{
    return m_serverConnectionStringCandidates;
}

void 
multiplayer_session_request::set_server_connection_string_candidates(
    _In_ std::vector<string_t> candidates
    )
{
    m_serverConnectionStringCandidates = std::move(candidates);
}

const std::vector<std::shared_ptr<multiplayer_session_member_request>>&
multiplayer_session_request::members() const
{
    return m_members;
}

void
multiplayer_session_request::set_members(
    _In_ std::vector<std::shared_ptr<multiplayer_session_member_request>> members
    )
{
    m_members = std::move(members);
}

web::json::value
multiplayer_session_request::servers() const
{
    return m_servers;
}

void
multiplayer_session_request::set_servers(
    _In_ web::json::value servers
    )
{
    m_servers = servers;
}

bool 
multiplayer_session_request::write_closed() const
{
    return m_writeClosed;
}

void 
multiplayer_session_request::set_write_closed(
    _In_ bool writeClosed
    )
{
    m_writeClosed = writeClosed;
}

bool 
multiplayer_session_request::closed() const
{
    return m_closed;
}

void 
multiplayer_session_request::set_closed(
    _In_ bool closed
    )
{
    m_closed = closed;
}

bool
multiplayer_session_request::write_locked() const
{
    return m_writeLocked;
}

void
multiplayer_session_request::set_write_locked(
    _In_ bool writeLocked
    )
{
    m_writeLocked = writeLocked;
}

bool
multiplayer_session_request::locked() const
{
    return m_locked;
}

void
multiplayer_session_request::set_locked(
    _In_ bool locked
)
{
    m_locked = locked;
}

bool 
multiplayer_session_request::write_allocate_cloud_compute() const
{
    return m_writeAllocateCloudCompute;
}

void 
multiplayer_session_request::set_write_allocate_cloud_compute(
    _In_ bool writeAllocateCloudCompute
    )
{
    m_writeAllocateCloudCompute = writeAllocateCloudCompute;
}

bool 
multiplayer_session_request::allocate_cloud_compute() const
{
    return m_allocateCloudCompute;
}

void 
multiplayer_session_request::set_allocate_cloud_compute(
    _In_ bool allocateCloudCompute
    )
{
    m_allocateCloudCompute = allocateCloudCompute;
}

void
multiplayer_session_request::set_mutable_role_settings(
    _In_ const std::unordered_map<string_t, multiplayer_role_type>& roleTypes
    )
{
    m_roleTypes = std::move(roleTypes);
}

web::json::value
multiplayer_session_request::create_properties_json()
{
    web::json::value jsonProperties;
    web::json::value jsonPropertiesSystem;
    if (!m_sessionPropertiesKeywords.empty())
    {
        jsonPropertiesSystem[_T("keywords")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_sessionPropertiesKeywords);
    }
    
    if (!m_sessionPropertiesTurns.empty())
    {
        jsonPropertiesSystem[_T("turn")] = utils::serialize_vector<uint32_t>(utils::json_int_serializer, m_sessionPropertiesTurns);
    }

    if (m_joinRestriction != multiplayer_session_restriction::unknown)
    {
        auto joinRestrictionToString = multiplayer_session_states::_Convert_multiplayer_session_restriction_to_string(m_joinRestriction);
        jsonPropertiesSystem[_T("joinRestriction")] = web::json::value::string(
            joinRestrictionToString.payload()
            );
    }

    if (m_readRestriction != multiplayer_session_restriction::unknown)
    {
        auto readRestrictionToString = multiplayer_session_states::_Convert_multiplayer_session_restriction_to_string(m_readRestriction);
        jsonPropertiesSystem[_T("readRestriction")] = web::json::value::string(
            readRestrictionToString.payload()
            );
    }

    if (m_writeClosed)
    {
        jsonPropertiesSystem[_T("closed")] = web::json::value(m_closed);
    }

    if (m_writeLocked)
    {
        jsonPropertiesSystem[_T("locked")] = web::json::value(m_locked);
    }

    if (m_writeAllocateCloudCompute)
    {
        jsonPropertiesSystem[_T("allocateCloudCompute")] = web::json::value(m_allocateCloudCompute);
    }

    if (m_writeMatchmakingClientResult || m_writeMatchmakingSessionConstants || m_writeMatchmakingServerConnectionPath)
    {
        jsonPropertiesSystem[_T("matchmaking")] = create_matchmaking_json();
    }

    if (m_writeMatchmakingResubmit)
    {
        jsonPropertiesSystem[_T("matchmakingResubmit")] = web::json::value(m_matchmakingMatchResubmit);
    }

    if (m_writeInitializationStatus)
    {
        jsonPropertiesSystem[_T("initializationSucceeded")] = web::json::value(m_initializationSucceeded);
    }

    if (m_writeHostDeviceToken)
    {
        jsonPropertiesSystem[_T("host")] = web::json::value(m_hostDeviceToken);
    }

    if (m_writeServerConnectionStringCandidates)
    {
        jsonPropertiesSystem[_T("serverConnectionStringCandidates")] = utils::serialize_vector<string_t>(
            utils::json_string_serializer, 
            m_serverConnectionStringCandidates
            );
    }

    if (!jsonPropertiesSystem.is_null())
    {
        jsonProperties[_T("system")] = jsonPropertiesSystem;
    }

    if (!m_sessionPropertiesCustomProperties.is_null())
    {
        jsonProperties[_T("custom")] = m_sessionPropertiesCustomProperties;
    }

    return jsonProperties;
}

web::json::value 
multiplayer_session_request::create_matchmaking_json()
{
    web::json::value jsonMatchmaking;

    if (!m_sessionPropertiesTargetSessionsConstants.is_null())
    {
        jsonMatchmaking[_T("targetSessionConstants")] = m_sessionPropertiesTargetSessionsConstants;
    }

    if (m_writeMatchmakingServerConnectionPath)
    {
        jsonMatchmaking[_T("serverConnectionString")] = web::json::value::string(m_matchmakingServerConnectionPath);
    }

    return jsonMatchmaking;
}

web::json::value
multiplayer_session_request::create_role_types_json()
{
    web::json::value serializedObject;
    if (m_roleTypes.size() > 0)
    {
        web::json::value roleTypeJson;
        for (const auto& roleType : m_roleTypes)
        {
            web::json::value rolesJson;
            auto roles = roleType.second.roles();
            for (const auto& role : roles)
            {
                web::json::value roleJson;
                auto roleInfo = role.second;
                if (roleInfo.max_members_count() > 0)
                {
                    roleJson[_T("max")] = roleInfo.max_members_count();
                }
                if (roleInfo.target_count() > 0)
                {
                    roleJson[_T("target")] = roleInfo.target_count();
                }
                rolesJson[role.first] = roleJson;
            }
            roleTypeJson[_T("roles")] = rolesJson;
            serializedObject[roleType.first] = roleTypeJson;
        }
    }
    return serializedObject;
}

web::json::value 
multiplayer_session_request::serialize()
{
    web::json::value serializedObject = web::json::value::object();
    std::lock_guard<std::mutex> lock(m_lock.get()); 

    if (m_sessionConstants != nullptr)
    {
        web::json::value sessionConstantsJson = m_sessionConstants->_Serialize(); 
        if (!sessionConstantsJson.is_null())
        {
            serializedObject[_T("constants")] = sessionConstantsJson;
        }
    }

    web::json::value roleTypesJson = create_role_types_json();
    if (!roleTypesJson.is_null())
    {
        serializedObject[_T("roleTypes")] = std::move(roleTypesJson);
    }

    web::json::value jsonProperties = create_properties_json();
    if (!jsonProperties.is_null())
    {
        serializedObject[_T("properties")] = jsonProperties;
    }

    if (!m_members.empty() || m_bLeaveSession)
    {
        web::json::value memberListJson;
        for (const auto& member : m_members)
        {
            web::json::value memberJson = member->serialize();
            if (!memberJson.is_null())
            {
                memberListJson[member->member_id()] = memberJson;
            }
        }

        if (m_bLeaveSession)
        {
            // Write "me" : null to leave session.
            memberListJson[_T("me")] = web::json::value();
        }

        if (!memberListJson.is_null())
        {
            serializedObject[_T("members")] = memberListJson;
        }
    }

    if (!m_servers.is_null())
    {
        serializedObject[_T("servers")] = m_servers;
    }

    return serializedObject;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END