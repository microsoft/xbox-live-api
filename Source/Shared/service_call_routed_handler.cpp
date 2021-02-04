// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "service_call_routed_handler.h"
#include "http_call_request_message_internal.h"
#include "xsapi_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

ServiceCallRoutedHandler::ServiceCallRoutedHandler(
    _In_ XblCallRoutedHandler handler,
    _In_opt_ void* context
) noexcept :
    m_clientHandler{ handler },
    m_clientContext{ context }
{
    m_hcToken = HCAddCallRoutedHandler(HCCallRoutedHandler, this);
}

ServiceCallRoutedHandler::~ServiceCallRoutedHandler() noexcept
{
    HCRemoveCallRoutedHandler(m_hcToken);
}

void ServiceCallRoutedHandler::HCCallRoutedHandler(
    _In_ HCCallHandle call,
    _In_ void* context
)
{
    auto pThis{ static_cast<ServiceCallRoutedHandler*>(context) };

    String formattedResponse{ pThis->GetFormattedResponse(call) };

    XblServiceCallRoutedArgs args
    {
        call,
        s_nextResponseNumber++,
        formattedResponse.data()
    };

    pThis->m_clientHandler(args, pThis->m_clientContext);
}

String ServiceCallRoutedHandler::GetFormattedResponse(
    HCCallHandle call
) const noexcept
{
    Stringstream response;

    response << "== [XBOX SERVICE CALL] #";
    response << s_nextResponseNumber;
    response << "\r\n";

    const char* uri{ nullptr };
    HCHttpCallGetRequestUrl(call, &uri);
    response << "\r\n[URI]: ";
    response << uri;

    const char* token{ nullptr };
    HCHttpCallResponseGetHeader(call, AUTH_HEADER, &token);
    if (token)
    {
        response << "\r\n[Authorization Header]: ";
        response << token;
    }

    const char* signature{ nullptr };
    HCHttpCallResponseGetHeader(call, SIG_HEADER, &signature);
    if (signature)
    {
        response << "\r\n[Signature Header]: ";
        response << signature;
    }

    uint32_t httpStatus{ 0 };
    HCHttpCallResponseGetStatusCode(call, &httpStatus);
    response << "\r\n[HTTP Status]: ";
    response << httpStatus;
#if HC_PLATFORM_IS_MICROSOFT
    HRESULT hr = utils::convert_http_status_to_hresult(httpStatus);
    response << " [";
    response << utils::convert_hresult_to_error_name(hr);
    response << "] ";
#endif

    uint32_t numHeaders{ 0 };
    HCHttpCallResponseGetNumHeaders(call, &numHeaders);

    if (numHeaders > 0)
    {
        response << "\r\n[Response Headers]: ";

        const char* headerName{ nullptr };
        const char* headerValue{ nullptr };
        for (uint32_t i = 0; i < numHeaders; ++i)
        {
            HCHttpCallResponseGetHeaderAtIndex(call, i, &headerName, &headerValue);
            response << headerName << " : " << headerValue << "; ";
        }
    }

    const char* responseBody{ nullptr };
    HCHttpCallResponseGetResponseString(call, &responseBody);
    if (responseBody)
    {
        response << "\r\n[Response Body]: ";
        response << responseBody;
    }

    response << "\r\n";

    return response.str();
}

std::atomic<uint64_t> ServiceCallRoutedHandler::s_nextResponseNumber{ 0 };

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END