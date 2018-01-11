// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

std::string utf8_from_utf16(_In_reads_(size) PCWSTR utf16, size_t size)
{
    // early out on empty strings since they are trivially convertible
    if (size == 0)
    {
        return "";
    }

    // query for the buffer size
    auto queryResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        utf16, static_cast<int>(size),
        nullptr, 0,
        nullptr, nullptr
    );
    if (queryResult == 0)
    {
        throw std::exception("utf8_from_utf16 failed");
    }

    // allocate the output buffer, queryResult is the required size
    std::string utf8(static_cast<size_t>(queryResult), L'\0');
    auto conversionResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        utf16, static_cast<int>(size),
        &utf8[0], static_cast<int>(utf8.size()),
        nullptr, nullptr
    );
    if (conversionResult == 0)
    {
        throw std::exception("utf8_from_utf16 failed");
    }

    return utf8;
}

std::wstring utf16_from_utf8(_In_reads_(size) PCSTR utf8, size_t size)
{
    // early out on empty strings since they are trivially convertible
    if (size == 0)
    {
        return L"";
    }

    // query for the buffer size
    auto queryResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, static_cast<int>(size),
        nullptr, 0
    );
    if (queryResult == 0)
    {
        throw std::exception("utf16_from_utf8 failed");
    }

    // allocate the output buffer, queryResult is the required size
    std::wstring utf16(static_cast<size_t>(queryResult), L'\0');
    auto conversionResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, static_cast<int>(size),
        &utf16[0], static_cast<int>(utf16.size())
    );
    if (conversionResult == 0)
    {
        throw std::exception("utf16_from_utf8 failed");
    }

    return utf16;
}

std::string utils_c::to_utf8string(const std::wstring& utf16)
{
    return utf8_from_utf16(utf16.data(), utf16.size());
}

std::wstring utils_c::to_utf16string(PCSTR utf8)
{
    return utf16_from_utf8(utf8, strlen(utf8));
}

std::wstring utils_c::to_utf16string(const std::string& utf8)
{
    return utf16_from_utf8(utf8.data(), utf8.size());
}

std::vector<utility::string_t> utils_c::to_string_vector(PCSTR* stringArray, size_t stringArrayCount)
{
    std::vector<utility::string_t> stringVector;
    for (size_t i = 0; i < stringArrayCount; ++i)
    {
        stringVector.push_back(utils_c::to_utf16string(stringArray[i]));
    }
    return stringVector;
}

time_t utils_c::time_t_from_datetime(const utility::datetime& datetime)
{
    time_t currentUtcTime = time(nullptr);
    auto diffTime = utility::datetime::utc_now() - datetime;
    return currentUtcTime - diffTime;
}

utility::datetime utils_c::datetime_from_time_t(const time_t* pTime)
{
    utility::datetime datetime = utility::datetime::utc_now();
    auto diffTime = time(nullptr) - *pTime;
    return datetime - utility::datetime::from_seconds((unsigned int)diffTime);
}

XSAPI_RESULT utils_c::std_bad_alloc_to_result(std::bad_alloc const& e, _In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] std::bad_alloc reached api boundary: %s\n    %s:%u",
    //    XSAPI_RESULT_E_HC_OUTOFMEMORY, e.what(), file, line);
    return XSAPI_RESULT_E_HC_OUTOFMEMORY;
}

XSAPI_RESULT utils_c::std_exception_to_result(std::exception const& e, _In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] std::exception reached api boundary: %s\n    %s:%u",
    //    XSAPI_RESULT_E_GENERIC_ERROR, e.what(), file, line);

    assert(false);
    return XSAPI_RESULT_E_GENERIC_ERROR;
}

XSAPI_RESULT utils_c::unknown_exception_to_result(_In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] unknown exception reached api boundary\n    %s:%u",
    //    XSAPI_RESULT_E_GENERIC_ERROR, file, line);

    assert(false);
    return XSAPI_RESULT_E_GENERIC_ERROR;
}

XSAPI_RESULT utils_c::xsapi_result_from_hc_result(HC_RESULT hcr)
{
    if (hcr == HC_OK)
    {
        return XSAPI_RESULT_OK;
    }
    return static_cast<XSAPI_RESULT>(hcr);
}

XSAPI_RESULT utils_c::xsapi_result_from_xbox_live_result_err(std::error_code errc)
{
    switch (errc.default_error_condition().value())
    {
    case (int)xbox::services::xbox_live_error_condition::no_error:
        return XSAPI_RESULT_OK;
    case (int)xbox::services::xbox_live_error_condition::auth:
        return XSAPI_RESULT_E_AUTH;
    case (int)xbox::services::xbox_live_error_condition::generic_error: 
        return XSAPI_RESULT_E_GENERIC_ERROR;
    case (int)xbox::services::xbox_live_error_condition::generic_out_of_range: 
        return XSAPI_RESULT_E_OUT_OF_RANGE;
    case (int)xbox::services::xbox_live_error_condition::http: 
        return XSAPI_RESULT_E_HTTP;
    case (int)xbox::services::xbox_live_error_condition::http_404_not_found: 
        return XSAPI_RESULT_E_HTTP_404_NOT_FOUND;
    case (int)xbox::services::xbox_live_error_condition::http_412_precondition_failed: 
        return XSAPI_RESULT_E_HTTP_412_PRECONDITION_FAILED;
    case (int)xbox::services::xbox_live_error_condition::http_429_too_many_requests: 
        return XSAPI_RESULT_E_HTTP_429_TOO_MANY_REQUESTS;
    case (int)xbox::services::xbox_live_error_condition::http_service_timeout: 
        return XSAPI_RESULT_E_HTTP_SERVICE_TIMEOUT;
    case (int)xbox::services::xbox_live_error_condition::network: 
        return XSAPI_RESULT_E_NETWORK;
    case (int)xbox::services::xbox_live_error_condition::rta: 
        return XSAPI_RESULT_E_RTA;
    default: 
        return XSAPI_RESULT_E_GENERIC_ERROR;
    }
}