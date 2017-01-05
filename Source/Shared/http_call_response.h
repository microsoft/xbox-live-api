//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/http_call_request_message.h"
#include "xsapi/http_call.h"
#include "shared_macros.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

template<typename T>
#if BEAM_API
xbox::services::beam::xbox_live_result<T>
#else
xbox::services::xbox_live_result<T>
#endif
get_xbl_result_from_response(_In_ std::shared_ptr<http_call_response> response, _In_ std::function<T(_In_ const web::json::value&)> deserializeFn)
{
    if (response->response_body_json().size() != 0)
    {
        return xbox::services::xbox_live_result<T>(deserializeFn(response->response_body_json()), response->err_code(), response->err_message());
    }
    else
    {
        return xbox::services::xbox_live_result<T>(response->err_code(), response->err_message());
    }
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif
