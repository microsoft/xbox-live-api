// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/xbox_live_context_c.h"
#include "xbox_live_context_impl.h"

struct XSAPI_XBOX_LIVE_CONTEXT_IMPL
{
public:
    XSAPI_XBOX_LIVE_CONTEXT_IMPL(_In_ CONST XSAPI_XBOX_LIVE_USER* pUser, _In_ XSAPI_XBOX_LIVE_CONTEXT *pContext);
    std::shared_ptr<xbox::services::xbox_live_context_impl> cppObject();

private:
    XSAPI_XBOX_LIVE_CONTEXT *m_pContext;
    std::shared_ptr<xbox::services::xbox_live_context_impl> m_cppContext;
};
