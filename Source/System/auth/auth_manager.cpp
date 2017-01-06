//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "request_signer.h"
#include "auth_manager.h"

using namespace XBOX_LIVE_NAMESPACE;

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

std::shared_ptr<auth_manager> auth_manager::s_authManager;

auth_manager::auth_manager(std::shared_ptr<auth_config> authConfig)
    :m_authConfig(std::move(authConfig)),
    m_proofKey(std::make_shared<ecdsa>()),
    m_xboxLiveContextSettings(std::make_shared<xbox_live_context_settings>()),
    m_xtitle_service(xbox_system_factory::get_factory()->create_xtitle_service()),
    m_tokenManager(std::make_shared<token_manager>(m_proofKey, m_authConfig, m_xboxLiveContextSettings))
{
}

std::shared_ptr<auth_manager> auth_manager::get_auth_manager_instance()
{
    if (s_authManager != nullptr)
    {
        return s_authManager;
    }

    auto localConfig = xbox_system_factory::get_factory()->create_local_config();

#if XSAPI_U
    std::shared_ptr<auth_config> authConfig = std::make_shared<auth_config>(
        xbox_live_app_config::get_app_config_singleton()->sandbox(),
        localConfig->environment_prefix(),
        localConfig->environment(),
        true
        );
#else
    std::shared_ptr<auth_config> authConfig = std::make_shared<auth_config>(
        localConfig->sandbox(),
        localConfig->environment_prefix(),
        localConfig->environment(),
        localConfig->use_first_party_token()
        );
#endif
    s_authManager = std::make_shared<auth_manager>(authConfig);
    return s_authManager;
}

void auth_manager::set_rps_ticket(const string_t& rpsTicket)
{
    m_tokenManager->set_rps_ticket(rpsTicket);
}

pplx::task<xbox_live_result<void>> 
auth_manager::initialize_default_nsal()
{
    std::weak_ptr<auth_manager> thisWeakPtr = shared_from_this();

    return m_xtitle_service->get_default_nsal(m_xboxLiveContextSettings, m_authConfig)
    .then([thisWeakPtr](xbox_live_result<nsal> defaultNsal)
    {
        std::shared_ptr<auth_manager> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "auth_manager shutting down.");
        }

        if (!defaultNsal.err())
        {
            pThis->m_defaultNsal = std::move(defaultNsal.payload());
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::AM_E_INVALID_NSAL_DATA, "Couldn't get default nsal.");
        }
    });
}

pplx::task<xbox_live_result<void>>
auth_manager::initialize_title_nsal(
    _In_ const string_t& titleId
    )
{
    std::weak_ptr<auth_manager> thisWeakPtr = shared_from_this();

    return m_xtitle_service->get_title_nsal(shared_from_this(), titleId, m_xboxLiveContextSettings, m_authConfig)
    .then([thisWeakPtr](xbox_live_result<nsal> titleNsal)
    {
        std::shared_ptr<auth_manager> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "auth_manager shutting down.");
        }

        if (!titleNsal.err())
        {
            pThis->m_titleNsal = std::move(titleNsal.payload());
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::AM_E_INVALID_NSAL_DATA, "Couldn't get title nsal.");
        }
    });
}

pplx::task<xbox_live_result<token_and_signature_result> >
auth_manager::internal_get_token_and_signature(
    _In_ string_t httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ string_t headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh,
    _In_ const string_t& relyingParty
    )
{
    web::http::uri parsedUrl(endpointForNsal);
    nsal_endpoint_info endpointInfo;
    if (!m_defaultNsal.get_endpoint(parsedUrl, endpointInfo))
    {
        if (!m_titleNsal.get_endpoint(parsedUrl, endpointInfo))
        {
            // No token or signature needed
            return pplx::task_from_result(xbox_live_result<token_and_signature_result>(token_and_signature_result()));
        }
    }
    
    int policyIndex = endpointInfo.signature_policy_index();

    string_t relyingPartyStr;
    if (relyingParty.empty())
    {
        relyingPartyStr = endpointInfo.relying_party();
    }
    else
    {
        relyingPartyStr = relyingParty;
    }

    std::shared_ptr<auth_manager> pThis = shared_from_this();
    return m_tokenManager->get_xtoken(
        relyingPartyStr,
        endpointInfo.sub_relying_party(),
        endpointInfo.token_type(),
        promptForCredentialsIfNeeded,
        forceRefresh
        )
    .then([pThis, policyIndex, headers, httpMethod, url, bytes](xbox_live_result<token_result> xblResult)
    {
        if (xblResult.err())
        {
            if (xblResult.payload().xerr() != 0)
            {
                pThis->m_authConfig->set_detail_error(xblResult.payload().xerr());
                pThis->m_authConfig->set_redirect(xblResult.payload().error().redirect());
            }

            pThis->m_authConfig->set_http_status_code(xblResult.payload().http_status_code());
            return xbox_live_result<token_and_signature_result>(xblResult.err(), xblResult.err_message());
        }

        token_result token = xblResult.payload();

        stringstream_t ss;
        ss << _T("XBL3.0 x=");
        // If no user hash, means the xtoken doesn't contain a user claim, use "-" instead
        if (token.user_hash().empty())
        {
            ss << _T("-");
        }
        else
        {
            ss << token.user_hash();
        }
        ss << _T(";");
        ss << token.token();
        string_t authHeader = ss.str();
        string_t sigHeader;

        if (policyIndex >= 0)
        {
            web::http::http_headers headerMap = utils::string_to_headers(std::move(headers));
            headerMap[_T("Authorization")] = authHeader;
#if XSAPI_SERVER || XSAPI_U
            sigHeader = request_signer::sign_request(
                *pThis->m_proofKey,
                pThis->m_defaultNsal.get_signature_policy(policyIndex),
                utility::datetime::utc_now().to_interval(),
                std::move(httpMethod),
                utils::path_and_query_from_uri(web::http::uri(url)),
                headerMap,
                bytes);
#endif
        }
        
        token_and_signature_result tokenResult(
            std::move(authHeader),
            std::move(sigHeader),
            token.user_xuid(),
            token.user_gamertag(),
            token.user_hash(),
            token.age_group(),
            token.privileges(),
            string_t(),
            token.title_id()
            );

        return xbox_live_result<token_and_signature_result>(std::move(tokenResult));
    });
}

std::shared_ptr<auth_config> auth_manager::get_auth_config()
{
    return m_authConfig;
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif