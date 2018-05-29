// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xsapi-c/xbox_live_context_c.h"
#include "user_internal_c.h"
#include "xbox_live_context_internal_c.h"
#include "user_context.h"

using namespace xbox::services;
using namespace xbox::services::system;

STDAPI
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
    (*context)->contextImpl->user_context()->set_caller_api_type(xbox::services::caller_api_type::api_c);

    return S_OK;
}
CATCH_RETURN()

STDAPI_(xbl_context_handle)
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

STDAPI_(void)
XblContextCloseHandle(
    _In_ xbl_context_handle xboxLiveContextHandle
    ) XBL_NOEXCEPT
try
{
    int refCount = --xboxLiveContextHandle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        xboxLiveContextHandle->~xbl_xbox_live_context();
        xsapi_memory::mem_free(xboxLiveContextHandle);
    }
}
CATCH_RETURN_WITH(;)

STDAPI
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

STDAPI
XblContextGetXboxUserId(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* xboxUserId
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr || xboxUserId == nullptr);
    *xboxUserId = context->xboxUserId;
    return S_OK;
}
CATCH_RETURN()