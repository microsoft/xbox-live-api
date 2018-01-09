// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xsapi-c/xbox_live_context_c.h"
#include "user_impl_c.h"
#include "xbox_live_context_impl.h"

using namespace xbox::services;

XBL_API XSAPI_RESULT XBL_CALLING_CONV
XboxLiveContextCreate(
    _In_ CONST XSAPI_XBOX_LIVE_USER *user,
    _Out_ XSAPI_XBOX_LIVE_CONTEXT** ppContext
    )
{
    if (user == nullptr || ppContext == nullptr)
    {
        return XSAPI_RESULT_E_HC_INVALIDARG;
    }

    verify_global_init();

    auto context = new XSAPI_XBOX_LIVE_CONTEXT();
    context->pImpl = new XSAPI_XBOX_LIVE_CONTEXT_IMPL(user, context);

    *ppContext = context;

    return XSAPI_RESULT_OK;
}

XBL_API void XBL_CALLING_CONV
XboxLiveContextDelete(
    XSAPI_XBOX_LIVE_CONTEXT *context
    )
{
    delete context->pImpl;
    delete context;
}
