// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/system_c.h"
#include "xbox_live_context_impl.h"
#if !XDK_API
#include "user_impl_c.h"
#endif

struct xbl_xbox_live_context
{
    xbl_xbox_live_context(_In_ XBL_XBOX_LIVE_USER_PTR _user)
        : user(_user),
        refCount(1),
        contextImpl(nullptr)
    {
#if XDK_API
        xboxUserId = xbox::services::utils::internal_string_from_utf16(user->XboxUserId->Data());
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(_user);
#else
        xboxUserId = _user->xboxUserId;
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(user->pImpl->cppUser());
#endif
        contextImpl->init();
    }


    XBL_XBOX_LIVE_USER_PTR user;
    std::shared_ptr<xbox::services::xbox_live_context_impl> contextImpl;
    xsapi_internal_string xboxUserId;
    std::atomic<int> refCount;
};
