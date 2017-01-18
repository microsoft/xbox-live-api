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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename T>
XBOX_LIVE_NAMESPACE::xbox_live_result<T>

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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
