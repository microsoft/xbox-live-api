// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xbox_live_context_impl.h"
#include "user_internal_c.h"
#if !XDK_API
#include "system_c.h"
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
        userInternal = get_user_from_user_handle(user);
        xboxUserIdString = xbox::services::utils::internal_string_from_utf16(userInternal->XboxUserId->Data());
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(userInternal);
#else
        xboxUserIdString = user->userImpl->xbox_user_id();
        contextImpl = xsapi_allocate_shared<xbox::services::xbox_live_context_impl>(user->internalUser);
#endif
        xboxUserId = xbox::services::utils::internal_string_to_uint64(xboxUserIdString);
        contextImpl->init();
    }

#if XDK_API
    IInspectable* user;
    Windows::Xbox::System::User^ userInternal;
#else
    xbl_xbox_live_user* user;
#endif
    std::shared_ptr<xbox::services::xbox_live_context_impl> contextImpl;
    xsapi_internal_string xboxUserIdString;
    uint64_t xboxUserId;
    std::atomic<int> refCount;
};
