//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "xsapi/system.h"
#include "a/user_impl_a.h"

using namespace std;
using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::shared_ptr<xbox_live_user>
xbox_live_user::get_last_signed_in_user()
{
    auto user = user_impl_android::get_instance();
    if (!user)
    {
        return nullptr;
    }
    return user->user().lock();
}

pplx::task<xbox_live_result<sign_in_result>> xbox_live_user::signin()
{
    std::weak_ptr<xbox_live_user> thisWeak = shared_from_this();
    m_user_impl->set_user(thisWeak);
    return m_user_impl->sign_in_impl(true, false);
}

pplx::task<xbox_live_result<sign_in_result>> xbox_live_user::signin_silently()
{
    std::weak_ptr<xbox_live_user> thisWeak = shared_from_this();
    m_user_impl->set_user(thisWeak);
    return m_user_impl->sign_in_impl(false, false);
}

pplx::task<xbox_live_result<void>> xbox_live_user::signout()
{
    return m_user_impl->signout();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
