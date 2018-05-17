// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct xbl_xbox_live_user
{
    xbl_xbox_live_user(_In_opt_ Windows::System::User^ creationContext)
        : refCount(1)
    {
        internalUser = creationContext == nullptr ?
            internalUser = xsapi_allocate_shared<xbox::services::system::xbox_live_user>() :
            internalUser = xsapi_allocate_shared<xbox::services::system::xbox_live_user>(creationContext);

        userImpl = internalUser->_User_impl();

        auto singleton = xbox::services::get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_trackingUsersLock);
        singleton->m_userHandlesMap[internalUser] = this;
    }

    ~xbl_xbox_live_user()
    {
        auto singleton = xbox::services::get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_trackingUsersLock);
        singleton->m_userHandlesMap.erase(internalUser);
    }

    std::shared_ptr<xbox::services::system::xbox_live_user> internalUser;
    std::shared_ptr<xbox::services::system::user_impl> userImpl;
    std::atomic<int> refCount;
};