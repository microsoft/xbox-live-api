// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "string_service_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

StringService::StringService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) :
    m_user{ std::move(user) },
    m_contextSettings{ std::move(contextSettings) }
{
}

HRESULT StringService::VerifyStrings(
    _In_ const xsapi_internal_vector<xsapi_internal_string> stringsToVerify,
    _In_ AsyncContext<Result<xsapi_internal_vector<VerifyStringResult>>> async
)
{
    if (stringsToVerify.size() == 0) {
        return E_INVALIDARG;
    }

    JsonDocument request(rapidjson::kObjectType);
    JsonValue stringsJson(rapidjson::kArrayType); // = request[_T("stringstoVerify")];
    for (const auto& stringToVerify : stringsToVerify)
    {
        stringsJson.PushBack(JsonValue(stringToVerify.c_str(), request.GetAllocator()).Move(), request.GetAllocator());
    }
    request.AddMember("stringsToVerify", stringsJson, request.GetAllocator());

    xsapi_internal_vector<VerifyStringResult> result;
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_contextSettings,
        "POST",
        XblHttpCall::BuildUrl("client-strings", "/system/strings/validate"),
        xbox_live_api::verify_strings
    );

    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(2));
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(request)));

    return httpCall->Perform({
       async.Queue(),
       [
           async,
           result
       ]
    (HttpResult httpResult)
    {
       HRESULT hr { Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
       if (FAILED(hr))
       {
            return async.Complete(hr);
       }

       return async.Complete(VerifyStringResult::DeserializeVerifyStringsResult(
           httpResult.Payload()->GetResponseBodyJson()));
    } });
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

STDAPI XblStringVerifyStringAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ const char* stringToVerify,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || stringToVerify == nullptr || async == nullptr);

    return XblStringVerifyStringsAsync(xboxLiveContextHandle, &stringToVerify, 1, async);
}
CATCH_RETURN()

STDAPI XblStringVerifyStringResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblStringVerifyStringResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblVerifyStringResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblVerifyStringResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblStringVerifyStringsAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ const char** stringsToVerify,
    _In_ const uint64_t stringsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    xsapi_internal_vector<xsapi_internal_string> strings;
    for (uint32_t i = 0; i < stringsCount; i++) {
        strings.push_back(xsapi_internal_string(stringsToVerify[i]));
    }
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || stringsToVerify == nullptr || async == nullptr);
    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            strings,
            verifyStringResults = xsapi_internal_vector<VerifyStringResult>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            HRESULT hr = xboxLiveContext->StringService()->VerifyStrings(
                std::move(strings),
                AsyncContext<Result<xsapi_internal_vector<VerifyStringResult>>>{
                data->async->queue,
                    [
                        &verifyStringResults,
                        asyncBlock{ data->async }
                    ]
                (Result<xsapi_internal_vector<VerifyStringResult>> result)
                {
                    if (Succeeded(result))
                    {
                        auto payload = result.ExtractPayload();
                        size_t bufferSize = 0;
                        if (!payload.empty())
                        {
                            for (size_t i = 0; i < payload.size(); i++)
                            {
                                verifyStringResults.push_back(payload[i]);
                                bufferSize += payload[i].SizeOf();
                            }

                            // Add some padding at the end of the buffer to store the number of XblVerifyStringResult
                            // objects since there is no way to deduce it just from the buffer size
                            bufferSize += sizeof(size_t);

                            // size must be rounded up to support word aligned data because of the arbitrary length string packing
                            // we won't actually use the extra space, but this will report that it is how much is needed
                            bufferSize = static_cast<size_t>((bufferSize + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;

                            XAsyncComplete(asyncBlock, result.Hresult(), bufferSize);
                        }
                        else {
                            XAsyncComplete(asyncBlock, E_FAIL, 0);
                        }
                    }
                    else
                    {
                        XAsyncComplete(asyncBlock, result.Hresult(), 0);
                    }
                }
            });

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        case XAsyncOp::GetResult:
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            auto resultPtr = reinterpret_cast<XblVerifyStringResult*>(buffer);
            auto stringPtr = buffer + (sizeof(XblVerifyStringResult)*verifyStringResults.size());
            size_t bufferSize{ sizeof(size_t) };

            for (auto& verifyStringResult : verifyStringResults)
            {
                bufferSize += verifyStringResult.SizeOf();
                resultPtr->resultCode = verifyStringResult.ResultCode();
                if (resultPtr->resultCode != XblVerifyStringResultCode::Success)
                {
                    utils::strcpy(stringPtr, verifyStringResult.FirstOffendingSubstring().size() + 1, verifyStringResult.FirstOffendingSubstring().c_str());
                    resultPtr->firstOffendingSubstring = stringPtr;
                    stringPtr += verifyStringResult.FirstOffendingSubstring().size() + 1;
                }
                else
                {
                    resultPtr->firstOffendingSubstring = nullptr;
                }
                
                resultPtr++;
            }

            // Calculate buffer used plus padding (to stay word aligned, see above)
            bufferSize = static_cast<size_t>((bufferSize + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
            size_t* resultArrSize = reinterpret_cast<size_t*>(buffer + bufferSize) - 1;
            *resultArrSize = verifyStringResults.size();
        }

        default: return S_OK;

        }
    });
}
CATCH_RETURN()

STDAPI XblStringVerifyStringsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblStringVerifyStringsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblVerifyStringResult** ptrToBuffer,
    _Out_ size_t* stringsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);

    size_t bufferUsedTemp{};
    if (bufferUsed == nullptr)
    {
        bufferUsed = &bufferUsedTemp;
    }

    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblVerifyStringResult*>(buffer);
        auto sizePtr = reinterpret_cast<size_t*>(static_cast<char*>(buffer) + *bufferUsed) - 1;
        *stringsCount = *sizePtr;
    }
    return hr;
}
CATCH_RETURN()
