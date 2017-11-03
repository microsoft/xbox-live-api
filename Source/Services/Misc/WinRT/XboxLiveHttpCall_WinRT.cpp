// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/services.h"
#include "XboxLiveContext_WinRT.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#if !TV_API
#include "User_WinRT.h"
#endif
#include "xsapi/profile.h"
#include "xsapi/http_call.h"
#include "XboxLiveHttpCall_WinRT.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

XboxLiveHttpCallResponse::XboxLiveHttpCallResponse(
    _In_ std::shared_ptr<http_call_response> cppObj
    ) :
    m_cppObj(cppObj)
{
    m_headersMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& header : m_cppObj->response_headers())
    {
        m_headersMap->Insert(ref new Platform::String(header.first.c_str()), ref new Platform::String(header.second.c_str()));
    }

    std::vector<unsigned char> cppVec = m_cppObj->response_body_vector();
    if( cppVec.size() > 0 )
    {
        m_vec = ref new Platform::Array<byte>(&cppVec[0], static_cast<uint32>(cppVec.size()));
    }
    else
    {
        m_vec = ref new Platform::Array<byte>(nullptr, 0);
    }
}

Platform::String^ XboxLiveHttpCallResponse::ResponseBodyJson::get()
{
    return ref new Platform::String( m_cppObj->response_body_json().serialize().c_str() );
}

Platform::Array<byte>^ XboxLiveHttpCallResponse::ResponseBodyVector::get()
{
    return m_vec;
}

int XboxLiveHttpCallResponse::ErrorCode::get()
{
    return m_cppObj->err_code().value();
}

Platform::String^ XboxLiveHttpCallResponse::ErrorMessage::get()
{
    return ref new Platform::String(utility::conversions::to_utf16string(m_cppObj->err_message()).c_str());
}

int64_t XboxLiveHttpCallResponse::RetryAfterInSeconds::get()
{
    return m_cppObj->retry_after().count();
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ XboxLiveHttpCallResponse::Headers::get()
{
    return m_headersMap->GetView();
}

XboxLiveHttpCall::XboxLiveHttpCall(
    _In_ std::shared_ptr<http_call> cppObj
    ) :
    m_cppObj(cppObj)
{
}

XboxLiveHttpCall^ XboxLiveHttpCall::CreateXboxLiveHttpCall(
    _In_ Microsoft::Xbox::Services::XboxLiveContextSettings^ settings,
    _In_ Platform::String^ httpMethod,
    _In_ Platform::String^ serverName,
    _In_ Platform::String^ pathQueryFragment
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(httpMethod);
    THROW_INVALIDARGUMENT_IF_NULL(serverName);
    THROW_INVALIDARGUMENT_IF_NULL(pathQueryFragment);

    CONVERT_STD_EXCEPTION(
        auto cppObj = create_xbox_live_http_call(
            settings->GetCppObj(),
            httpMethod->Data(),
            serverName->Data(),
            web::uri(pathQueryFragment->Data())
            );

        return ref new XboxLiveHttpCall(cppObj);
    );
}

Windows::Foundation::IAsyncOperation<XboxLiveHttpCallResponse^>^
XboxLiveHttpCall::GetResponseWithAuth(
#if TV_API
    _In_ Windows::Xbox::System::User^ user,
#else
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
#endif
    _In_ HttpCallResponseBodyType httpCallResponseBodyType
    )
{
    CONVERT_STD_EXCEPTION(
        std::shared_ptr<user_context> userContext = std::make_shared<user_context>(user);
        http_call_response_body_type bodyType = static_cast<http_call_response_body_type>(httpCallResponseBodyType);

        auto task = m_cppObj->get_response_with_auth(userContext, bodyType)
        .then([](pplx::task< std::shared_ptr<http_call_response> > t)
        {
            CONVERT_STD_EXCEPTION(
                return ref new XboxLiveHttpCallResponse(t.get());
            );
        });

        return ASYNC_FROM_TASK(task);
    );
}

Windows::Foundation::IAsyncOperation<XboxLiveHttpCallResponse^>^
XboxLiveHttpCall::GetResponseWithoutAuth(
    _In_ HttpCallResponseBodyType httpCallResponseBodyType
    )
{
    CONVERT_STD_EXCEPTION(
        http_call_response_body_type bodyType = static_cast<http_call_response_body_type>(httpCallResponseBodyType);

        auto task = m_cppObj->get_response(bodyType)
        .then([](pplx::task<std::shared_ptr<http_call_response>> t)
        {
            CONVERT_STD_EXCEPTION(
                return ref new XboxLiveHttpCallResponse(t.get());
            );
        });

        return ASYNC_FROM_TASK(task);
    );
}

void XboxLiveHttpCall::SetRequestBody(
    _In_ Platform::String^ value
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(value);

    CONVERT_STD_EXCEPTION(
        return m_cppObj->set_request_body(STRING_T_FROM_PLATFORM_STRING(value));
    );
}

void XboxLiveHttpCall::SetRequestBodyArray(
    _In_ const Platform::Array<byte>^ requestBodyArray
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(requestBodyArray);

    CONVERT_STD_EXCEPTION(
        byte* bufferBytes = requestBodyArray->Data;
        std::vector<unsigned char> requestBody(bufferBytes, bufferBytes + requestBodyArray->Length);
        return m_cppObj->set_request_body(requestBody);
    );
}

void XboxLiveHttpCall::SetCustomHeader(
    _In_ Platform::String^ headerName,
    _In_opt_ Platform::String^ headerValue
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(headerName);

    CONVERT_STD_EXCEPTION(
        m_cppObj->set_custom_header(headerName->Data(), headerValue->Data());
    );
}

Platform::String^ XboxLiveHttpCall::PathQueryFragment::get()
{
    CONVERT_STD_EXCEPTION(
        return ref new Platform::String(
            m_cppObj->path_query_fragment().to_string().c_str()
            );
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END

