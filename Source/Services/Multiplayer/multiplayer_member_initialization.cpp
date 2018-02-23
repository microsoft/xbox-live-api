// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_member_initialization::multiplayer_member_initialization() :
    m_managedInitializationSet(false),
    m_externalEvaluation(false),
    m_writeManagedInitialization(false),
    m_membersNeededToStart(0)
{
}

multiplayer_member_initialization::multiplayer_member_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measureTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool externalEvaluation,
    _In_ uint32_t membersNeededToStart
    ) :
    m_joinTimeout(std::move(joinTimeout)),
    m_measurementTimeout(std::move(measureTimeout)),
    m_evaluationTimeout(std::move(evaluationTimeout)),
    m_externalEvaluation(externalEvaluation),
    m_membersNeededToStart(membersNeededToStart),
    m_managedInitializationSet(true),
    m_writeManagedInitialization(true)
{
}

bool
multiplayer_member_initialization::member_initialization_set() const
{
    return m_managedInitializationSet;
}

const std::chrono::milliseconds&
multiplayer_member_initialization::join_timeout() const
{
    return m_joinTimeout;
}

const std::chrono::milliseconds&
multiplayer_member_initialization::measurement_timeout() const
{
    return m_measurementTimeout;
}

const std::chrono::milliseconds&
multiplayer_member_initialization::evaluation_timeout() const
{
    return m_evaluationTimeout;
}

bool
multiplayer_member_initialization::external_evaluation() const
{
    return m_externalEvaluation;
}

uint32_t
multiplayer_member_initialization::members_need_to_start() const
{
    return m_membersNeededToStart;
}

web::json::value
multiplayer_member_initialization::_Serialize()
{
    web::json::value serializedObject;
    if (m_writeManagedInitialization)
    {
        serializedObject[_T("joinTimeout")] = utils::serialize_uint52_to_json(m_joinTimeout.count());
        serializedObject[_T("measurementTimeout")] = utils::serialize_uint52_to_json(m_measurementTimeout.count());
        if (m_externalEvaluation)
        {
            serializedObject[_T("evaluationTimeout")] = utils::serialize_uint52_to_json(m_evaluationTimeout.count()); 
        }
        serializedObject[_T("externalEvaluation")] = web::json::value(m_externalEvaluation);
        serializedObject[_T("membersNeededToStart")] = web::json::value(m_membersNeededToStart);
    }

    return serializedObject;
}

xbox_live_result<multiplayer_member_initialization>
multiplayer_member_initialization::_Deserialize(
    _In_ const web::json::value& json
)
{
    multiplayer_member_initialization returnObject;
    if (json.is_null()) return xbox_live_result<multiplayer_member_initialization>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value managedInitializationJson = utils::extract_json_field(json, _T("memberInitialization"), errc, false);

    returnObject.m_managedInitializationSet = !managedInitializationJson.is_null();

    if (returnObject.m_managedInitializationSet)
    {
        returnObject.m_joinTimeout = std::chrono::milliseconds(utils::extract_json_uint52(managedInitializationJson, "joinTimeout", errc));
        returnObject.m_measurementTimeout = std::chrono::milliseconds(utils::extract_json_uint52(managedInitializationJson, "measurementTimeout", errc));
        returnObject.m_evaluationTimeout = std::chrono::milliseconds(utils::extract_json_uint52(managedInitializationJson, "evaluationTimeout", errc));
        returnObject.m_externalEvaluation = utils::extract_json_bool(managedInitializationJson, _T("externalEvaluation"), errc);
        returnObject.m_membersNeededToStart = utils::extract_json_int(managedInitializationJson, _T("membersNeededToStart"), errc);
    }

    return returnObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END