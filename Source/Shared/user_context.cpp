// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "shared_macros.h"

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation::Collections;
#if !XSAPI_CPP
using namespace Microsoft::Xbox::Services::System;
#endif
#endif

using namespace xbox::services::system;


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

const string_t&
user_context::caller_context() const
{
    return m_callerContext;
}

caller_context_type
user_context::caller_context_type() const
{
    return m_callerContextType;
}

void
user_context::set_caller_context_type(xbox::services::caller_context_type context)
{
    m_callerContextType = context;
    if (context == caller_context_type::multiplayer_manager)
    {
        m_callerContext = _T("MultiplayerManager");
    }
    else if (context == caller_context_type::social_manager)
    {
        m_callerContext = _T("SocialManager");
    }
}

const string_t& user_context::xbox_user_id() const
{
    return m_xboxUserId;
}

user_context_auth_result::user_context_auth_result()
{
}

user_context_auth_result::user_context_auth_result(
    _In_ string_t token,
    _In_ string_t signature) :
    m_token(std::move(token)),
    m_signature(std::move(signature))
{
}

const string_t& user_context_auth_result::token() const
{
    return m_token;
}

const string_t& user_context_auth_result::signature() const
{
    return m_signature;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
