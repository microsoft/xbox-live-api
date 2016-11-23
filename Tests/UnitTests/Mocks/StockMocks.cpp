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
#include "TE/UnitTestHelpers.h"
#include "StockMocks.h"
#include "http_call_response.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

web::json::value StockMocks::GetStockJsonMockUserTokenResult()
{
    web::json::value jsonRoot = web::json::value::parse(
        LR"({
            "DisplayClaims" :
            {
                "xui" :
                    [
                        {
                            "uhs" : "TestXboxUserHash"
                        }
                    ]
            },
            "IssueInstant" : "2014-03-05T20:14:34.3709609Z",
            "NotAfter" : "2014-03-06T04:14:34.3709609Z",
            "Token" : "TestToken"
        })");

    return jsonRoot;
}

web::json::value StockMocks::GetStockJsonMockXTokenResult()
{
    web::json::value jsonRoot = web::json::value::parse(
        LR"({
            "DisplayClaims" :
            {
                "xui" :
                    [
                        {
                            "agg" : "Adult",
                            "gtg" : "TestGamerTag",
                            "prv" : "191 192",
                            "uhs" : "TestXboxUserHash",
                            "xid" : "TestXboxUserId"
                        }
                    ]
            },
            "IssueInstant" : "2014-03-05T20:14:34.3709609Z",
            "NotAfter" : "2014-03-06T04:14:34.3709609Z",
            "Token" : "TestToken"
        })");

    return jsonRoot;
}

web::json::value StockMocks::GetStockNsal()
{
    return web::json::value::parse(LR"({"EndPoints":[{"Protocol":"https","Host":"xlink.xboxlive.com","HostType":"fqdn","RelyingParty":"http://xlink.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"*.dfhosted.net","HostType":"wildcard","RelyingParty":"http://xlink.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"musicdelivery-ssl.xboxlive.com","HostType":"fqdn","RelyingParty":"http://music.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"cloudcollection-ssl.xboxlive.com","HostType":"fqdn","RelyingParty":"http://music.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"music.xboxlive.com","HostType":"fqdn","RelyingParty":"http://music.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"websockets.platform.bing.com","HostType":"fqdn","RelyingParty":"http://platform.bing.com/","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"websockets.platform.bing-int.com","HostType":"fqdn","RelyingParty":"http://platform.bing.com/","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"inventory.xboxlive.com","HostType":"fqdn","RelyingParty":"http://licensing.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"licensing.xboxlive.com","HostType":"fqdn","RelyingParty":"http://licensing.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"accountstroubleshooter.xboxlive.com","HostType":"fqdn","RelyingParty":"http://accounts.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"help.ui.xboxlive.com","HostType":"fqdn","RelyingParty":"http://uxservices.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"*.ui.xboxlive.com","HostType":"wildcard"},{"Protocol":"https","Host":"data-vef.xboxlive.com","HostType":"fqdn","RelyingParty":"http://data-vef.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":1},{"Protocol":"https","Host":"update.xboxlive.com","HostType":"fqdn","RelyingParty":"http://update.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"update-cdn.xboxlive.com","HostType":"fqdn","RelyingParty":"http://update.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"packages.xboxlive.com","HostType":"fqdn","RelyingParty":"http://update.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"instance.mgt.xboxlive.com","HostType":"fqdn","RelyingParty":"http://instance.mgt.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":1},{"Protocol":"https","Host":"device.mgt.xboxlive.com","HostType":"fqdn","RelyingParty":"http://device.mgt.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":1},{"Protocol":"https","Host":"user.mgt.xboxlive.com","HostType":"fqdn","RelyingParty":"http://user.mgt.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":1},{"Protocol":"https","Host":"xkms.xboxlive.com","HostType":"fqdn","RelyingParty":"http://xkms.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"privileges.xboxlive.com","HostType":"fqdn","RelyingParty":"http://banning.xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":1},{"Protocol":"https","Host":"*.xboxlive.com","HostType":"wildcard","RelyingParty":"http://xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"https","Host":"xboxlive.com","HostType":"fqdn","RelyingParty":"http://xboxlive.com","TokenType":"JWT","SignaturePolicyIndex":0},{"Protocol":"http","Host":"*.xboxlive.com","HostType":"wildcard"}],"SignaturePolicies":[{"Version":1,"SupportedAlgorithms":["ES256"],"MaxBodyBytes":8192},{"Version":1,"SupportedAlgorithms":["ES256"],"MaxBodyBytes":4294967295}]})");
}

web::json::value StockMocks::GetCreateAccountXErrorResponse()
{
    return web::json::value::parse(LR"({"Token" : "TestToken", "Identity":"0","XErr":2148916233,"Message":"","Redirect":"https://start.ui.xboxlive.com/CreateAccount"})");
}

void
StockMocks::InitializeResponse(
    _In_ int statusCode,
    _In_ web::http::http_response& response
    )
{
    response.headers().add(L"ETag", L"MockETag");
    response.headers().add(L"Retry-After", L"1");
    response.headers().add(L"Content-Location", L"/serviceconfigs/MockScid/sessiontemplates/MockSessionTemplateName/sessions/MockSessionName");
    response.set_status_code(statusCode);
}

xbox::services::xbox_live_error_code get_xbox_live_error_code_from_http_status(
    _In_ web::http::status_code statusCode
    )
{
    if (statusCode < 300)
    {
        // Treat as success so 
        //      if (!result.err()) 
        // works properly which requires all non-errors to be 0.
        return xbox_live_error_code::no_error;
    }
    else
    {
        xbox::services::xbox_live_error_code err = static_cast<xbox::services::xbox_live_error_code>(statusCode);
        return err;
    }
}

std::shared_ptr<http_call_response>
StockMocks::CreateMockHttpCallResponse(
    _In_ web::json::value responseBodyJson,
    _In_ web::http::http_response httpResponse
    )
{
    return CreateMockHttpCallResponse(responseBodyJson, 200, httpResponse);
}

std::shared_ptr<http_call_response> 
StockMocks::CreateMockHttpCallResponse(
    _In_ web::json::value responseBodyJson,
    _In_ int statusCode,
    _In_ web::http::http_response httpResponse,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
    )
{
    if (xboxLiveContextSettings == nullptr)
    {
        xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();
    }

    std::shared_ptr<user_context> userContext = std::make_shared<user_context>();
    web::http::http_request request;
    std::wstring requestBody = L"";
    std::wstring baseUrl = L"www.microsoft.com";

    web::http::http_response response;
    if (httpResponse.headers().size() > 0)
    {
        response = httpResponse;
        response.set_status_code(statusCode);
    }
    else
    {
        InitializeResponse(statusCode, response);
    }

    auto httpCallResponse = std::make_shared<http_call_response>(
        userContext->xbox_user_id(),
        xboxLiveContextSettings,
        baseUrl,
        request,
        requestBody,
        xbox_live_api::unspecified,
        response
        );
    httpCallResponse->_Set_response_body(responseBodyJson);
    httpCallResponse->_Set_error(std::make_error_code(get_xbox_live_error_code_from_http_status(response.status_code())), std::string());
    return httpCallResponse;
}

std::shared_ptr<http_call_response>
StockMocks::CreateMockHttpCallResponse(
    _In_ string_t responseString,
    _In_ int statusCode,
    _In_ web::http::http_response httpResponse
    )
{
    std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();
    std::shared_ptr<user_context> userContext = std::make_shared<user_context>();
    web::http::http_request request;
    std::wstring requestBody = L"";
    std::wstring baseUrl = L"www.microsoft.com";

    web::http::http_response response;
    if (httpResponse.headers().size() > 0)
    {
        response = httpResponse;
        response.set_status_code(statusCode);
    }
    else
    {
        InitializeResponse(statusCode, response);
    }

    auto httpCallResponse = std::make_shared<http_call_response>(
        userContext->xbox_user_id(),
        xboxLiveContextSettings,
        baseUrl,
        request,
        requestBody,
        xbox_live_api::unspecified,
        response
        );
    httpCallResponse->_Set_response_body(responseString);
    httpCallResponse->_Set_error(std::make_error_code(get_xbox_live_error_code_from_http_status(response.status_code())), std::string());
    return httpCallResponse;
}

std::shared_ptr<http_call_response>
StockMocks::CreateMockHttpCallResponse(
    _In_ std::vector<unsigned char> responseVector,
    _In_ int statusCode,
    _In_ web::http::http_response httpResponse
    )
{
    std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();
    std::shared_ptr<user_context> userContext = std::make_shared<user_context>();
    web::http::http_request request;
    std::wstring requestBody = L"";
    std::wstring baseUrl = L"www.microsoft.com";

    web::http::http_response response;
    if (httpResponse.headers().size() > 0)
    {
        response = httpResponse;
        response.set_status_code(statusCode);
    }
    else
    {
        InitializeResponse(statusCode, response);
    }

    auto httpCallResponse = std::make_shared<http_call_response>(
        userContext->xbox_user_id(),
        xboxLiveContextSettings,
        baseUrl,
        request,
        requestBody,
        xbox_live_api::unspecified,
        response
        );
    httpCallResponse->_Set_response_body(responseVector);
    httpCallResponse->_Set_error(std::make_error_code(get_xbox_live_error_code_from_http_status(response.status_code())), std::string());
    return httpCallResponse;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

