// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/system_c.h"
#include "xbox_live_context_impl.h"
#include "user_impl_c.h"

struct xbl_xbox_live_context
{
    xbl_xbox_live_context(_In_ const XBL_XBOX_LIVE_USER *_user)
        : user(_user),
        refCount(1),
        contextImpl(nullptr),
        xboxUserId(_user->xboxUserId)
    {
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(user->pImpl->cppUser());
        contextImpl->init();
    }

    const XBL_XBOX_LIVE_USER *user;
    std::shared_ptr<xbox::services::xbox_live_context_impl> contextImpl;
    xsapi_internal_string xboxUserId;
    std::atomic<int> refCount;
};
