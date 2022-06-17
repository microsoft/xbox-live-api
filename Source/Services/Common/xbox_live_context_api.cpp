// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services;

STDAPI XblContextCreateHandle(
    _In_ XblUserHandle user,
    _Out_ XblContextHandle* context
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || context == nullptr);
    auto globalState{ GlobalState::Get() };
    if (!globalState)
    {
        return E_XBL_NOT_INITIALIZED;
    }

    auto wrapUserResult{ User::WrapHandle(user) };
    RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

    auto xboxLiveContext = XblContext::Make(wrapUserResult.ExtractPayload());

    HRESULT hr = xboxLiveContext->Initialize(globalState->RTAManager());
    if (SUCCEEDED(hr))
    {
        xboxLiveContext->AddRef();
        *context = xboxLiveContext.get();
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblContextDuplicateHandle(
    _In_ XblContextHandle handle,
    _Out_ XblContextHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);

    handle->AddRef();
    *duplicatedHandle = handle;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblContextCloseHandle(
    _In_ XblContextHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblContextGetUser(
    _In_ XblContextHandle context,
    _Out_ XblUserHandle* user
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || context == nullptr);
    return XalUserDuplicateHandle(context->User().Handle(), user);
}
CATCH_RETURN()

STDAPI XblContextGetXboxUserId(
    _In_ XblContextHandle context,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || xuid == nullptr);
    *xuid = context->Xuid();
    return S_OK;
}
CATCH_RETURN()

void XblSetApiType(
    _In_ XblApiType apiType
) XBL_NOEXCEPT
try
{
    auto state = GlobalState::Get();
    if (state)
    {
        state->ApiType = apiType;
    }
}
CATCH_RETURN_WITH(;)