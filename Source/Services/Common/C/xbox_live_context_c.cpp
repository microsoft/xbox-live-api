// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xsapi-c/xbox_live_context_c.h"
#if !XDK_API
#include "user_c.h"
#endif
#include "xbox_live_context_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::system;

XBL_API HRESULT XBL_CALLING_CONV
XblContextCreateHandle(
    _In_ xbl_user_handle user,
    _Out_ xbl_context_handle* context
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || context == nullptr);

    verify_global_init();

    void *buffer = xsapi_memory::mem_alloc(sizeof(xbl_xbox_live_context));
    *context = new (buffer) xbl_xbox_live_context(user);

    return S_OK;
}
CATCH_RETURN()

XBL_API xbl_context_handle XBL_CALLING_CONV
XblContextDuplicateHandle(
    _In_ xbl_context_handle context
    ) XBL_NOEXCEPT
try
{
    if (context == nullptr)
    {
        return nullptr;
    }

    context->refCount++;
    return context;
}
CATCH_RETURN_WITH(nullptr)

XBL_API HRESULT XBL_CALLING_CONV
XblContextCloseHandle(
    _In_ xbl_context_handle xboxLiveContextHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(xboxLiveContextHandle);

    int refCount = --xboxLiveContextHandle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        xsapi_memory::mem_free(xboxLiveContextHandle);
    }
    return S_OK;
}
CATCH_RETURN()

XBL_API HRESULT XBL_CALLING_CONV
XblContextGetUser(
    _In_ xbl_context_handle context,
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || context == nullptr);
    *user = context->user;
    return S_OK;
}
CATCH_RETURN()

XBL_API HRESULT XBL_CALLING_CONV
XblContextGetXboxUserId(
    _In_ xbl_context_handle context,
    _Out_ const char** xboxUserId
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr || xboxUserId == nullptr);
    *xboxUserId = context->xboxUserId.data();
    return S_OK;
}
CATCH_RETURN()