// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#if XSAPI_NONXDK_CPP_AUTH || XSAPI_NONXDK_WINRT_AUTH
class token_and_signature_result_internal
{
public:
    token_and_signature_result_internal();

    token_and_signature_result_internal(
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
    );

#ifndef DEFAULT_MOVE_ENABLED
    _XSAPIIMP token_and_signature_result(token_and_signature_result&& other);
    _XSAPIIMP token_and_signature_result& operator=(token_and_signature_result&& other);
#endif

    _XSAPIIMP const xsapi_internal_string& token() const;

    _XSAPIIMP const xsapi_internal_string& signature() const;

    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

    _XSAPIIMP const xsapi_internal_string& gamertag() const;

    _XSAPIIMP const xsapi_internal_string& xbox_user_hash() const;

    _XSAPIIMP const xsapi_internal_string& reserved() const;

    _XSAPIIMP const xsapi_internal_string& age_group() const;

    _XSAPIIMP const xsapi_internal_string& privileges() const;

#if XSAPI_U
    _XSAPIIMP const stxsapi_internal_stringring_t& user_settings_restrictions() const;

    _XSAPIIMP const xsapi_internal_string& user_enforcement_restrictions() const;

    _XSAPIIMP const xsapi_internal_string& user_title_restrictions() const;
#endif

    _XSAPIIMP const xsapi_internal_string& web_account_id() const;

private:

    xsapi_internal_string m_token;
    xsapi_internal_string m_signature;
    xsapi_internal_string m_xboxUserId;
    xsapi_internal_string m_gamerTag;
    xsapi_internal_string m_xboxUserHash;
    xsapi_internal_string m_ageGroup;
    xsapi_internal_string m_privileges;
#if XSAPI_U
    xsapi_internal_string m_userSettingsRestrictions;
    xsapi_internal_string m_userEnforcementRestrictions;
    xsapi_internal_string m_userTitleRestrictions;
#endif
    xsapi_internal_string m_webAccountId;
    xsapi_internal_string m_reserved;

#if UWP_API
public:
    token_and_signature_result_internal(
        Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
    );

    Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ token_request_result() const;

private:
    Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ m_tokenResult;
#endif 
};
#endif

class xbox_live_mutex
{
public:
    xbox_live_mutex();
    xbox_live_mutex(_In_ const xbox_live_mutex& other);
    xbox_live_mutex operator=(_In_ const xbox_live_mutex& other);
    std::mutex& get();
private:
    std::mutex m_xboxLiveMutex;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END