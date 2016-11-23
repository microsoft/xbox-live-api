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
#include "nsal.h"
#include "token_manager.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class xtitle_service;
class auth_manager;

class auth_manager : public std::enable_shared_from_this<auth_manager>
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    auth_manager(std::shared_ptr<auth_config> authConfig);

    static std::shared_ptr<auth_manager> get_auth_manager_instance();
    void set_rps_ticket(const string_t& rpsTicket);
    pplx::task<xbox::services::xbox_live_result<void>> initialize_default_nsal();
    pplx::task<xbox_live_result<void>> initialize_title_nsal(
        _In_ const string_t& titleId
        );

    pplx::task<xbox::services::xbox_live_result<token_and_signature_result>>
    internal_get_token_and_signature(
        _In_ string_t httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ string_t headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_ const string_t& relyingParty = _T("")
        );

    std::shared_ptr<token_manager> auth_token_manager() { return m_tokenManager; }

    std::shared_ptr<auth_config> get_auth_config();

private:

    std::shared_ptr<auth_config> m_authConfig;
    std::shared_ptr<xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xtitle_service> m_xtitle_service;

    nsal m_defaultNsal;
    nsal m_titleNsal;
    std::shared_ptr<ecdsa> m_proofKey;
    std::shared_ptr<token_manager> m_tokenManager;

    static std::shared_ptr<auth_manager> s_authManager;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
