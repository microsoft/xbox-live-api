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
#include "token_request.h"
#include "json_web_key.h"
#include "signature_policy.h"

#include <string>
#include <vector>
#include <cstdint>

#define PROPERTIES_FIELD _T("Properties")
#define PROOFKEY_PROPERTY _T("ProofKey")

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const signature_policy token_request::auth_signature_policy(1, INT32_MAX, std::vector<string_t>());

web::json::value CreateBaseRequest(
    _In_ string_t relyingParty,
    _In_ string_t tokenType)
{
    web::json::value request;
    request[_T("RelyingParty")] = web::json::value(std::move(relyingParty));
    request[_T("TokenType")] = web::json::value(std::move(tokenType));
    return request;
}

web::json::value token_request::create_device_token_request(
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ string_t authMethod,
    _In_ string_t siteName,
    _In_ string_t rpsTicket
    )
{
    web::json::value request(CreateBaseRequest(std::move(relyingParty), std::move(tokenType)));

    web::json::value& properties(request[PROPERTIES_FIELD]);
    properties[_T("AuthMethod")] = web::json::value(std::move(authMethod));
    properties[_T("SiteName")] = web::json::value(std::move(siteName));
    properties[_T("RpsTicket")] = web::json::value(std::move(rpsTicket));

    if (proofKey != NULL && (proofKey->pub_key().x.size() != 0 || proofKey->pub_key().y.size() != 0))
    {
        properties[PROOFKEY_PROPERTY] = json_web_key::serialize_json_web_key(proofKey);
    }

    return request;
}

web::json::value token_request::create_title_token_request(
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ string_t deviceToken,
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ string_t authMethod,
    _In_ string_t siteName,
    _In_ string_t rpsTicket
    )
{
    web::json::value request(CreateBaseRequest(std::move(relyingParty), std::move(tokenType)));

    web::json::value& properties(request[PROPERTIES_FIELD]);
    properties[_T("DeviceToken")] = web::json::value(std::move(deviceToken));
    properties[_T("AuthMethod")] = web::json::value(std::move(authMethod));
    properties[_T("SiteName")] = web::json::value(std::move(siteName));
    properties[_T("RpsTicket")] = web::json::value(std::move(rpsTicket));

    if (proofKey != NULL && (proofKey->pub_key().x.size() != 0 || proofKey->pub_key().y.size() != 0))
    {
        properties[PROOFKEY_PROPERTY] = json_web_key::serialize_json_web_key(proofKey);
    }

    return request;
}

web::json::value token_request::create_user_token_request(
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ string_t authMethod,
    _In_ string_t siteName,
    _In_ string_t rpsTicket,
    _In_ std::shared_ptr<ecdsa> proofKey
    )
{
    web::json::value request(CreateBaseRequest(std::move(relyingParty), std::move(tokenType)));
    
    web::json::value& properties(request[PROPERTIES_FIELD]);
    properties[_T("AuthMethod")] = web::json::value(std::move(authMethod));
    properties[_T("SiteName")] = web::json::value(std::move(siteName));
    properties[_T("RpsTicket")] = web::json::value(std::move(rpsTicket));
    if (proofKey->pub_key().x.size() != 0 || proofKey->pub_key().y.size() != 0)
    {
       properties[PROOFKEY_PROPERTY] = json_web_key::serialize_json_web_key(proofKey);
    }

    return request;
}

web::json::value token_request::create_service_token_request(
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ std::shared_ptr<ecdsa> proofKey)
{
    web::json::value request(CreateBaseRequest(std::move(relyingParty), std::move(tokenType)));

    web::json::value& properties(request[PROPERTIES_FIELD]);
    if (proofKey->pub_key().x.size() != 0 || proofKey->pub_key().y.size() != 0)
    {
        properties[PROOFKEY_PROPERTY] = json_web_key::serialize_json_web_key(proofKey);
    }

    return request;
}

web::json::value token_request::create_xtoken_request(
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ string_t deviceToken,
    _In_ string_t titleToken,
    _In_ string_t userToken,
    _In_ string_t serviceToken,
    _In_ string_t sandbox)
{
    web::json::value request(CreateBaseRequest(std::move(relyingParty), std::move(tokenType)));

    web::json::value& properties(request[PROPERTIES_FIELD]);
    if (!sandbox.empty())
    {
        properties[_T("SandboxId")] = web::json::value(std::move(sandbox));
    }
    
    if (!userToken.empty())
    {
        web::json::value users(web::json::value::array(1));
        users[0] = web::json::value(std::move(userToken));
        properties[_T("UserTokens")] = users;
    }

    if (!deviceToken.empty())
    {
        properties[_T("DeviceToken")] = web::json::value(std::move(deviceToken));
    }

    if (!titleToken.empty())
    {
        properties[_T("TitleToken")] = web::json::value(std::move(titleToken));
    }

#if XSAPI_SERVER
    if (!serviceToken.empty())
    {
        properties[_T("ServiceToken")] = web::json::value(std::move(serviceToken));
    }
#endif

    return request;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
