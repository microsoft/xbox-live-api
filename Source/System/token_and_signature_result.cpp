// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

token_and_signature_result::token_and_signature_result()
{
}

token_and_signature_result::token_and_signature_result(
    _In_ std::shared_ptr<token_and_signature_result_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(token_and_signature_result, token);
DEFINE_GET_STRING(token_and_signature_result, signature);
DEFINE_GET_STRING(token_and_signature_result, xbox_user_id);
DEFINE_GET_STRING(token_and_signature_result, gamertag);
DEFINE_GET_STRING(token_and_signature_result, xbox_user_hash);
DEFINE_GET_STRING(token_and_signature_result, reserved);
DEFINE_GET_STRING(token_and_signature_result, age_group);
DEFINE_GET_STRING(token_and_signature_result, privileges);
#if XSAPI_U
DEFINE_GET_STRING(token_and_signature_result, user_settings_restrictions);
DEFINE_GET_STRING(token_and_signature_result, user_enforcement_restrictions);
DEFINE_GET_STRING(token_and_signature_result, user_title_restrictions);
#endif
DEFINE_GET_STRING(token_and_signature_result, web_account_id);
#if UWP_API
DEFINE_GET_OBJECT(token_and_signature_result, Windows::Security::Authentication::Web::Core::WebTokenRequestResult^, token_request_result);
#endif

token_and_signature_result_internal::token_and_signature_result_internal(
    _In_ xsapi_internal_string token,
    _In_ xsapi_internal_string signature,
    _In_ xsapi_internal_string xuid,
    _In_ xsapi_internal_string gamertag,
    _In_ xsapi_internal_string userHash,
    _In_ xsapi_internal_string ageGroup,
    _In_ xsapi_internal_string privileges,
#if XSAPI_U
    _In_ xsapi_internal_string userSettingsRestrictions,
    _In_ xsapi_internal_string userEnforcementRestrictions,
    _In_ xsapi_internal_string userTitleRestrictions,
#endif
    _In_ xsapi_internal_string webAccountId,
    _In_ xsapi_internal_string reserved
    ) :
    m_token(std::move(token)), 
    m_signature(std::move(signature)), 
    m_xboxUserId(std::move(xuid)), 
    m_xboxUserHash(std::move(userHash)), 
    m_gamerTag(std::move(gamertag)),
    m_ageGroup(std::move(ageGroup)),
    m_privileges(std::move(privileges)),
#if XSAPI_U
    m_userSettingsRestrictions(std::move(userSettingsRestrictions)),
    m_userEnforcementRestrictions(std::move(userEnforcementRestrictions)),
    m_userTitleRestrictions(std::move(userTitleRestrictions)),
#endif
    m_webAccountId(std::move(webAccountId)),
    m_reserved(std::move(reserved))
{
}

token_and_signature_result_internal::token_and_signature_result_internal()
{
}

#ifndef DEFAULT_MOVE_ENABLED
token_and_signature_result_internal::token_and_signature_result_internal(token_and_signature_result_internal&& other)
{
    *this = std::move(other);
}

token_and_signature_result_internal& token_and_signature_result_internal::operator = (token_and_signature_result_internal&& other)
{
    if (this != &other)
    {
        m_token = std::move(other.m_token);
        m_signature = std::move(other.m_signature);
        m_xboxUserId = std::move(other.m_xboxUserId);
        m_xboxUserHash = std::move(other.m_xboxUserHash);
        m_gamerTag = std::move(other.m_gamerTag);
        m_ageGroup = std::move(other.m_ageGroup);
        m_privileges = std::move(other.m_privileges);
#if XSAPI_U
        m_userSettingsRestrictions = std::move(other.m_userSettingsRestrictions);
        m_userEnforcementRestrictions = std::move(other.m_userEnforcementRestrictions);
        m_userTitleRestrictions = std::move(other.m_userTitleRestrictions);
#endif
        m_webAccountId = std::move(other.m_webAccountId);
        m_reserved = std::move(other.m_reserved);
    }

    return *this;
}
#endif

const xsapi_internal_string& token_and_signature_result_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

const xsapi_internal_string& token_and_signature_result_internal::token() const
{
    return m_token;
}

const xsapi_internal_string& token_and_signature_result_internal::signature() const
{
    return m_signature;
}

const xsapi_internal_string& token_and_signature_result_internal::gamertag() const
{
    return m_gamerTag;
}

const xsapi_internal_string& token_and_signature_result_internal::xbox_user_hash() const
{
    return m_xboxUserHash;
}

const xsapi_internal_string& token_and_signature_result_internal::age_group() const
{
    return m_ageGroup;
}

const xsapi_internal_string& token_and_signature_result_internal::privileges() const
{
    return m_privileges;
}

#if XSAPI_U
const xsapi_internal_string& token_and_signature_result_internal::user_settings_restrictions() const
{
    return m_userSettingsRestrictions;
}

const xsapi_internal_string& token_and_signature_result_internal::user_enforcement_restrictions() const
{
    return m_userEnforcementRestrictions;
}

const xsapi_internal_string& token_and_signature_result_internal::user_title_restrictions() const
{
    return m_userTitleRestrictions;
}
#endif

const xsapi_internal_string& token_and_signature_result_internal::reserved() const
{
    return m_reserved;
}

const xsapi_internal_string& token_and_signature_result_internal::web_account_id() const
{
    return m_webAccountId;
}

#if UWP_API
token_and_signature_result_internal::token_and_signature_result_internal(
    Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
    ):
    m_tokenResult(tokenResult)
{
}

Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ 
token_and_signature_result_internal::token_request_result() const
{
    return m_tokenResult;
}
#endif


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
