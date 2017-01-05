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
#include "utils.h"
#include "xsapi/system.h"
#include "user_impl_ios.h"
#import "XBLiOSGlobalState.h"

using namespace std;

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

static std::weak_ptr<xbox_live_user> user;

#if XSAPI_I

std::shared_ptr<xbox_live_user>
xbox_live_user::get_last_signed_in_user()
{    
    return user.lock();
}

pplx::task<xbox_live_result<sign_in_result>>
xbox_live_user::signin()
{
    user = shared_from_this();
    return m_user_impl->sign_in_impl(true, false);
}

pplx::task<xbox_live_result<sign_in_result>>
xbox_live_user::signin_silently()
{
    user = shared_from_this();
    return m_user_impl->sign_in_impl(false, false);
}

pplx::task<xbox_live_result<void>>
xbox_live_user::signout()
{
    return m_user_impl->signout();
}

#endif

#ifdef BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif
