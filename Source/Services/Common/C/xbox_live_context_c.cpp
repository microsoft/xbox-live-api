// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xsapi-c/xbox_live_context_c.h"
#include "user_impl_c.h"
#include "xbox_live_context_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::system;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextCreateHandle(
    _In_ CONST XBL_XBOX_LIVE_USER* user,
    _Out_ XBL_XBOX_LIVE_CONTEXT_HANDLE* xboxLiveContextHandle
    ) XBL_NOEXCEPT
try
{
    if (user == nullptr || xboxLiveContextHandle == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }

    verify_global_init();

    void *buffer = xsapi_memory::mem_alloc(sizeof(xbl_xbox_live_context));
    xbl_xbox_live_context *context = new (buffer) xbl_xbox_live_context(user);
    *xboxLiveContextHandle = context;

    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API XBL_XBOX_LIVE_CONTEXT_HANDLE XBL_CALLING_CONV
XblXboxLiveContextDuplicateHandle(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle
    ) XBL_NOEXCEPT
try
{
    if (xboxLiveContextHandle == nullptr)
    {
        return nullptr;
    }

    xboxLiveContextHandle->refCount++;
    return xboxLiveContextHandle;
}
CATCH_RETURN_WITH(nullptr)

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextCloseHandle(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle
    ) XBL_NOEXCEPT
try
{
    if (xboxLiveContextHandle == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }

    int refCount = --xboxLiveContextHandle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        xsapi_memory::mem_free(xboxLiveContextHandle);
    }
    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetUser(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ CONST XBL_XBOX_LIVE_USER** user
    ) XBL_NOEXCEPT
try
{
    if (xboxLiveContextHandle == nullptr || user == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }
    *user = xboxLiveContextHandle->user;
    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetXboxUserId(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ PCSTR* xboxUserId
    ) XBL_NOEXCEPT
try
{
    if (xboxLiveContextHandle == nullptr || xboxUserId == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }
    *xboxUserId = xboxLiveContextHandle->xboxUserId.data();
    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetAppConfig(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ CONST XBL_XBOX_LIVE_APP_CONFIG** appConfig
    ) XBL_NOEXCEPT
try
{
    if (xboxLiveContextHandle == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }
    return XblGetXboxLiveAppConfigSingleton(appConfig);
}
CATCH_RETURN()
