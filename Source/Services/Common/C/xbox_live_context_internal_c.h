// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/system_c.h"
#include "xbox_live_context_impl.h"
#if !XDK_API
#include "user_c.h"
#include "user_impl.h"
#endif

struct xbl_xbox_live_context
{
    xbl_xbox_live_context(_In_ xbl_user_handle _user)
        : user(_user),
        refCount(1),
        contextImpl(nullptr)
    {
#if XDK_API
        xboxUserId = xbox::services::utils::internal_string_from_utf16(user->XboxUserId->Data());
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(_user);
#else
        xboxUserId = user->userImpl->xbox_user_id();
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(user->internalUser);
#endif
        contextImpl->init();
    }


    xbl_user_handle user;
    std::shared_ptr<xbox::services::xbox_live_context_impl> contextImpl;
    xsapi_internal_string xboxUserId;
    std::atomic<int> refCount;
};
