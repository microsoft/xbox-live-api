// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"
#include "xbox_live_context_internal.h"
#include "xsapi-c/http_call_c.h"

using namespace xbox::services;
using namespace xbox::services::system;

// Http APIs
STDAPI XblHttpCallCreate(
    _In_ XblContextHandle xblContext,
    _In_z_ const char* method,
    _In_z_ const char* url,
    _Out_ XblHttpCallHandle* call
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();

    auto userResult = xblContext->User().Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(xblContext->Settings(), method, url, xbox_live_api::unspecified);
    if (SUCCEEDED(hr)) 
    {
        httpCall->AddRef();
        *call = httpCall.get();
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblHttpCallPerformAsync(
    _In_ XblHttpCallHandle call,
    _In_ XblHttpCallResponseBodyType type,
    _Inout_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(type);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(asyncBlock, __FUNCTION__,
        [
            call{ std::dynamic_pointer_cast<XblHttpCall>(call->shared_from_this()) }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(call->Perform(AsyncContext<HttpResult>{ TaskQueue::DeriveWorkerQueue(data->async->queue),
                [data](HttpResult httpResult)
            {
                auto hr = httpResult.Hresult();
                if (SUCCEEDED(hr))
                {
                    hr = httpResult.Payload()->Result();
                }
                XAsyncComplete(data->async, hr, 0);
            }
            }));
            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblHttpCallDuplicateHandle(
    _In_ XblHttpCallHandle call,
    _Out_ XblHttpCallHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(call == nullptr || duplicatedHandle == nullptr);
    call->AddRef();
    *duplicatedHandle = call;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblHttpCallCloseHandle(
    _In_ XblHttpCallHandle call
) XBL_NOEXCEPT
try
{
    if (call)
    {
        call->DecRef();
    }
    return;
}
CATCH_RETURN_WITH(;)

STDAPI XblHttpCallSetTracing(
    _In_ XblHttpCallHandle call,
    _In_ bool traceCall
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);

    return call->SetTracing(traceCall);
}
CATCH_RETURN()

STDAPI XblHttpCallGetRequestUrl(
    _In_ XblHttpCallHandle call,
    _Out_ const char** url
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(url);

    return call->GetRequestUrl(url);
}
CATCH_RETURN()

// HttpCallRequest Set APIs
STDAPI XblHttpCallRequestSetRequestBodyBytes(
    _In_ XblHttpCallHandle call,
    _In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes,
    _In_ uint32_t requestBodySize
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->SetRequestBody(requestBodyBytes, requestBodySize);
}
CATCH_RETURN()

STDAPI XblHttpCallRequestSetRequestBodyString(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* requestBodyString
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->SetRequestBody(requestBodyString);
}
CATCH_RETURN()

STDAPI XblHttpCallRequestSetHeader(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* headerName,
    _In_z_ const char* headerValue,
    _In_ bool allowTracing
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(headerName);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(headerValue);
    const xsapi_internal_string headerNameString{ headerName };
    const xsapi_internal_string headerValueString{ headerValue };

    return call->SetHeader(headerNameString, headerValueString, allowTracing);
}
CATCH_RETURN()

STDAPI XblHttpCallRequestSetRetryAllowed(
    _In_ XblHttpCallHandle call,
    _In_ bool retryAllowed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);

    return call->SetRetryAllowed(retryAllowed);
}
CATCH_RETURN()

STDAPI XblHttpCallRequestSetRetryCacheId(
    _In_ XblHttpCallHandle call,
    _In_ uint32_t retryAfterCacheId
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);

    // values 1-1000 are reserved for XSAPI
    if (retryAfterCacheId <= 1000)
    {
        return E_INVALIDARG;
    }

    return call->SetRetryCacheId(retryAfterCacheId);
}
CATCH_RETURN()

STDAPI XblHttpCallRequestSetLongHttpCall(
    _In_ XblHttpCallHandle call,
    _In_ bool longHttpCall
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);

    call->SetLongHttpCall(longHttpCall);
    return S_OK;
}
CATCH_RETURN()

// HttpCallResponse Get APIs
STDAPI XblHttpCallGetResponseString(
    _In_ XblHttpCallHandle call,
    _Out_ const char** responseString
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->GetResponseString(responseString);
}
CATCH_RETURN()

STDAPI XblHttpCallGetResponseBodyBytesSize(
    _In_ XblHttpCallHandle call,
    _Out_ size_t* bufferSize
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->GetResponseBodyBytesSize(bufferSize);
}
CATCH_RETURN()

STDAPI XblHttpCallGetResponseBodyBytes(
    _In_ XblHttpCallHandle call,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) uint8_t* buffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->GetResponseBodyBytes(bufferSize, buffer, bufferUsed);
}
CATCH_RETURN()

STDAPI XblHttpCallGetStatusCode(
    _In_ XblHttpCallHandle call,
    _Out_ uint32_t* statusCode
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(statusCode);

    *statusCode = call->HttpStatus();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblHttpCallGetNetworkErrorCode(
    _In_ XblHttpCallHandle call,
    _Out_ HRESULT* networkErrorCode,
    _Out_ uint32_t* platformNetworkErrorCode
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->GetNetworkErrorCode(networkErrorCode, platformNetworkErrorCode);
}
CATCH_RETURN()

STDAPI XblHttpCallGetPlatformNetworkErrorMessage(
    _In_ XblHttpCallHandle call,
    _Out_ const char** platformNetworkErrorMessage
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->GetPlatformNetworkErrorMessage(platformNetworkErrorMessage);
}
CATCH_RETURN()

STDAPI XblHttpCallGetHeader(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* headerName,
    _Out_ const char** headerValue
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->ResponseGetHeader(headerName, headerValue);
}
CATCH_RETURN()

STDAPI XblHttpCallGetNumHeaders(
    _In_ XblHttpCallHandle call,
    _Out_ uint32_t* numHeaders
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->ResponseGetNumHeaders(numHeaders);
}
CATCH_RETURN()

STDAPI XblHttpCallGetHeaderAtIndex(
    _In_ XblHttpCallHandle call,
    _In_ uint32_t headerIndex,
    _Out_ const char** headerName,
    _Out_ const char** headerValue
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(call);
    return call->ResponseGetHeaderAtIndex(headerIndex, headerName, headerValue);
}
CATCH_RETURN()
