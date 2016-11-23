//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include "xsapi/system.h"
#include "shared_macros.h"
#include "signature_policy.h"
#include "token_result.h"
#include "utils.h"
#include "Ecdsa.h"
#include "cpprest/http_msg.h"
#include "local_config.h"
#include "auth_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class token_manager
{
public:
    token_manager(
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ std::shared_ptr<auth_config> authConfig,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
        );

    pplx::task<xbox::services::xbox_live_result<token_result>> get_xtoken(
        _In_ const string_t& relyingParty,
        _In_ const string_t& subRelyingParty,
        _In_ const string_t tokenType,
        _In_ bool promptForCreds,
        _In_ bool forceRefresh);

    void set_rps_ticket(const string_t& rpsTicket);

    void clear_token_cache();

    const string_t& get_rps_ticket();

    string_t get_event_token_from_xuid(const string_t& xuid);

private:

    struct token_info
    {
    public:
        token_info();

        token_info(
            _In_ string_t relyingParty,
            _In_ string_t subRelyingParty,
            _In_ string_t tokenType);

        void set_token_result(_In_ const token_result& tokenResult)
        {
            std::lock_guard<std::mutex> tokenLock(Lock);
            Token = tokenResult;
            RefreshEvent.set(tokenResult);
            IsRefreshInProgress = false;
        }

        const string_t RelyingParty;
        const string_t SubRelyingParty;
        const string_t TokenType;

        std::mutex Lock; // Lock for synchronizing access to non-const fields
        token_result Token;
        bool IsRefreshInProgress;
        pplx::task_completion_event<xbox::services::xbox_live_result<token_result>> RefreshEvent;
    };

    struct token_state
    {
        token_state(
            _In_ std::shared_ptr<ecdsa> proofKey,
            _In_ std::shared_ptr<auth_config> authConfig,
            _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings);

        std::map<string_t, std::shared_ptr<token_info>> Cache;
        string_t RpsTicket;
        std::mutex DataLock;
        const std::shared_ptr<ecdsa> ProofKey;
        const std::shared_ptr<auth_config> AuthConfig;
        const std::shared_ptr<xbox_live_context_settings> XboxLiveContextSettings;
    };

    NO_COPY_AND_ASSIGN(token_manager);

    std::unordered_map<string_t, string_t> m_xuidTokenMap;
    string_t m_ticket;

    // We store the state in a shared_ptr in case the background
    // token refresh thread (which updates the state) outlives the 
    // lifetime of the token manager.
    std::shared_ptr<token_state> m_state;

    static string_t create_cache_key(
        _In_ const string_t& relyingParty,
        _In_ const string_t& subRelyingParty,
        _In_ const string_t& tokenType,
        _In_ token_identity_type tokenIdentityType);

    // Gets a token from state->Cache in a thread safe manner.
    static std::shared_ptr<token_info> get_token_from_cache(
        _In_ std::shared_ptr<token_state> state,
        _In_ const string_t& relyingParty,
        _In_ const string_t& subRelyingParty,
        _In_ const string_t& tokenType,
        _In_ token_identity_type tokenIdentityType,
        _In_ bool forceRefresh = false);

    static pplx::task<xbox::services::xbox_live_result<token_result>> get_token_from_cache_or_service(
        _In_ std::shared_ptr<token_state> state,
        _In_ const string_t& relyingParty,
        _In_ const string_t& subRelyingParty,
        _In_ const string_t tokenType,
        _In_ bool promptForCreds,
        _In_ token_identity_type tokenIdentityType,
        _In_ bool forceRefresh = false
        );

    // Kicks off a background thread to fetch a new XToken. The
    // task finishes when the background thread completes. Repeated
    // calls for the same token (here tokens are the "same" when
    // the token type and RP are the same) does not start multiple
    // threads. If an existing thread exists, the call is given
    // a task that finishes when that thread finishes.
    static pplx::task<xbox::services::xbox_live_result<token_result>> refresh_x_token(
        _In_ std::shared_ptr<token_state> state,
        _In_ std::shared_ptr<token_info> tokenInfo,
        _In_ bool promptForCreds
        );

    static pplx::task<xbox::services::xbox_live_result<token_result>> refresh_t_token(
        _In_ std::shared_ptr<token_state> state,
        _In_ std::shared_ptr<token_info> tokenInfo
        );

    static pplx::task<xbox::services::xbox_live_result<token_result>> refresh_u_token(
        _In_ std::shared_ptr<token_state> state,
        _In_ std::shared_ptr<token_info> tokenInfo
        );

    static pplx::task<xbox::services::xbox_live_result<token_result>> refresh_d_token(
        _In_ std::shared_ptr<token_state> state,
        _In_ std::shared_ptr<token_info> tokenInfo
        );

    static pplx::task<xbox::services::xbox_live_result<token_result>> refresh_s_token(
        _In_ std::shared_ptr<token_state> state,
        _In_ std::shared_ptr<token_info> tokenInfo
        );
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
