// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_context_c.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "user_impl_c.h"
#include "xbox_live_context_impl_c.h"

XSAPI_XBOX_LIVE_CONTEXT_IMPL::XSAPI_XBOX_LIVE_CONTEXT_IMPL(_In_ CONST XSAPI_XBOX_LIVE_USER *user, _In_ XBL_XBOX_LIVE_CONTEXT* pContext) 
    : m_pContext(pContext)
{
    m_cppContext = std::make_shared<xbox::services::xbox_live_context_impl>(user->pImpl->cppUser());
    m_cppContext->init();

    pContext->pUser = user;
    pContext->xboxUserId = user->xboxUserId;

    GetXboxLiveAppConfigSingleton(&(pContext->pAppConfig));
}

std::shared_ptr<xbox::services::xbox_live_context_impl> XSAPI_XBOX_LIVE_CONTEXT_IMPL::cppObject()
{
    return m_cppContext;
}