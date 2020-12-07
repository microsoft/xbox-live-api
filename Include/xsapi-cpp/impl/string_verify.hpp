// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

verify_string_result::verify_string_result() :
    m_resultCode {verify_string_result_code::success},
    m_first_offending_substring{}
{ }

verify_string_result::verify_string_result(
    XblVerifyStringResultCode resultCode,
    char const* firstOffendingSubstring
) :
    m_resultCode{ static_cast<verify_string_result_code>(resultCode)},
    m_first_offending_substring{ Utils::StringTFromUtf8(firstOffendingSubstring ? firstOffendingSubstring : "") }
{ }

verify_string_result_code verify_string_result::result_code() const
{
    return m_resultCode;
}

const string_t& verify_string_result::first_offending_substring() const
{
    return m_first_offending_substring;
}

string_service::string_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

string_service::string_service(const string_service& other) 
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

string_service& string_service::operator=(string_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

string_service::~string_service()
{
    XblContextCloseHandle(m_xblContext);
}

pplx::task<xbox_live_result<verify_string_result>> string_service::verify_string(_In_ const string_t& stringToVerify)
{
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<verify_string_result>(
        [](XAsyncBlock* async, verify_string_result& result)
    {
        size_t bufferSize{ 0 };
        auto hr = XblStringVerifyStringResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblVerifyStringResult* resultResponse;
            hr = XblStringVerifyStringResult(async, bufferSize, buffer.get(), &resultResponse, nullptr);
            if (SUCCEEDED(hr))
            {
                auto internalResult = reinterpret_cast<XblVerifyStringResult*>(buffer.get());
                result = verify_string_result(internalResult->resultCode, internalResult->firstOffendingSubstring);
            }
        }
        return hr;
    });
    auto hr = XblStringVerifyStringAsync(
        xblContext,
        Utils::StringFromStringT(stringToVerify).c_str(),
        &asyncWrapper->async
        );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<verify_string_result>>> string_service::verify_strings(_In_ const std::vector<string_t>& stringsToVerify)
{
    UTF8StringArrayRef stringsToVerifyInternal{ stringsToVerify };
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<std::vector<verify_string_result>>(
        [](XAsyncBlock* async, std::vector<verify_string_result>& results)
    {
        size_t bufferSize;
        auto hr = XblStringVerifyStringsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblVerifyStringResult* resultResponses = nullptr;
            size_t stringsCount{ 0 };
            hr = XblStringVerifyStringsResult(async, bufferSize, buffer.get(), &resultResponses, &stringsCount, nullptr);
            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < stringsCount; i++) 
                {
                    results.push_back(verify_string_result(resultResponses[i].resultCode, resultResponses[i].firstOffendingSubstring));
                }
            }
        }
        return hr;
    });


    auto hr = XblStringVerifyStringsAsync(
        xblContext,
        stringsToVerifyInternal.Data(),
        stringsToVerifyInternal.Size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END