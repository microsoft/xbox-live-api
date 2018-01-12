// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

XBL_RESULT utils_c::std_bad_alloc_to_result(std::bad_alloc const& e, _In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] std::bad_alloc reached api boundary: %s\n    %s:%u",
    //    XBL_RESULT_E_HC_OUTOFMEMORY, e.what(), file, line);
    return XBL_RESULT_E_HC_OUTOFMEMORY;
}

XBL_RESULT utils_c::std_exception_to_result(std::exception const& e, _In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] std::exception reached api boundary: %s\n    %s:%u",
    //    XBL_RESULT_E_GENERIC_ERROR, e.what(), file, line);

    assert(false);
    return XBL_RESULT_E_GENERIC_ERROR;
}

XBL_RESULT utils_c::unknown_exception_to_result(_In_z_ char const* file, uint32_t line)
{
    //HC_TRACE_ERROR(XSAPI_C_TRACE, "[%d] unknown exception reached api boundary\n    %s:%u",
    //    XBL_RESULT_E_GENERIC_ERROR, file, line);

    assert(false);
    return XBL_RESULT_E_GENERIC_ERROR;
}

XBL_RESULT utils_c::xsapi_result_from_hc_result(HC_RESULT hcr)
{
    if (hcr == HC_OK)
    {
        return XBL_RESULT_OK;
    }
    return static_cast<XBL_RESULT>(hcr);
}

XBL_RESULT utils_c::xsapi_result_from_xbox_live_result_err(std::error_code errc)
{
    switch (errc.default_error_condition().value())
    {
    case (int)xbox::services::xbox_live_error_condition::no_error:
        return XBL_RESULT_OK;
    case (int)xbox::services::xbox_live_error_condition::auth:
        return XBL_RESULT_E_AUTH;
    case (int)xbox::services::xbox_live_error_condition::generic_error: 
        return XBL_RESULT_E_GENERIC_ERROR;
    case (int)xbox::services::xbox_live_error_condition::generic_out_of_range: 
        return XBL_RESULT_E_OUT_OF_RANGE;
    case (int)xbox::services::xbox_live_error_condition::http: 
        return XBL_RESULT_E_HTTP;
    case (int)xbox::services::xbox_live_error_condition::http_404_not_found: 
        return XBL_RESULT_E_HTTP_404_NOT_FOUND;
    case (int)xbox::services::xbox_live_error_condition::http_412_precondition_failed: 
        return XBL_RESULT_E_HTTP_412_PRECONDITION_FAILED;
    case (int)xbox::services::xbox_live_error_condition::http_429_too_many_requests: 
        return XBL_RESULT_E_HTTP_429_TOO_MANY_REQUESTS;
    case (int)xbox::services::xbox_live_error_condition::http_service_timeout: 
        return XBL_RESULT_E_HTTP_SERVICE_TIMEOUT;
    case (int)xbox::services::xbox_live_error_condition::network: 
        return XBL_RESULT_E_NETWORK;
    case (int)xbox::services::xbox_live_error_condition::rta: 
        return XBL_RESULT_E_RTA;
    default: 
        return XBL_RESULT_E_GENERIC_ERROR;
    }
}