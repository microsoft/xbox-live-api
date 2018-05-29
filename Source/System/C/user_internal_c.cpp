// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_internal_c.h"

xbox_live_user_t get_user_from_user_handle(xbl_user_handle userHandle)
{
#if XDK_API
    IID xboxSystemUserIID = { 0xA2C2223E, 0x2F00, 0x44E2,{ 0x89, 0x7E, 0x01, 0x5B, 0xCE, 0xB8, 0x77, 0x17 } };
    Windows::Xbox::System::User^ user;
    auto hr = userHandle->QueryInterface(xboxSystemUserIID, (void**)&user);
    if (SUCCEEDED(hr))
    {
        return user;
    }
    else
    {
        return nullptr;
    }
#else
    return userHandle->internalUser;
#endif
}

#if XDK_API
xbl_user_handle get_user_handle_from_user(xbox_live_user_t user)
{
    return reinterpret_cast<IInspectable*>(static_cast<Platform::Object^>(user));
}
#endif