// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct xbl_xbox_live_user
{
    xbl_xbox_live_user(_In_ Windows::System::User^ creationContext)
        : refCount(1)
    {
        internalUser = creationContext == nullptr ?
            internalUser = xsapi_allocate_shared<xbox::services::system::xbox_live_user>() :
            internalUser = xsapi_allocate_shared<xbox::services::system::xbox_live_user>(creationContext);

        userImpl = internalUser->_User_impl();
    }
    
    std::shared_ptr<xbox::services::system::xbox_live_user> internalUser;
    std::shared_ptr<xbox::services::system::user_impl> userImpl;
    std::atomic<int> refCount;
};