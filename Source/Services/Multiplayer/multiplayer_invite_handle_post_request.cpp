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
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_invite_handle_post_request::multiplayer_invite_handle_post_request() :
    m_version(0),
    m_titleId(0)
{
}

multiplayer_invite_handle_post_request::multiplayer_invite_handle_post_request(
    _In_ multiplayer_session_reference sessionReference,
    _In_ uint32_t version,
    _In_ string_t invitedXboxUserId,
    _In_ uint32_t titleId,
    _In_ string_t contextString,
    _In_ string_t customActivationContext
    ) :
    m_sessionReference(std::move(sessionReference)),
    m_version(version),
    m_invitedXboxUserId(std::move(invitedXboxUserId)),
    m_titleId(titleId),
    m_contextString(std::move(contextString)),
    m_customActivationContext(std::move(customActivationContext))
{
    XSAPI_ASSERT(!m_sessionReference.is_null());
    XSAPI_ASSERT(!m_invitedXboxUserId.empty());
}

const multiplayer_session_reference& 
multiplayer_invite_handle_post_request::session_reference() const
{
    return m_sessionReference;
}

uint32_t 
multiplayer_invite_handle_post_request::version() const
{
    return m_version;
}

const string_t& 
multiplayer_invite_handle_post_request::invited_xbox_user_id() const
{
    return m_invitedXboxUserId;
}

uint32_t 
multiplayer_invite_handle_post_request::title_id() const
{
    return m_titleId;
}

const string_t&
multiplayer_invite_handle_post_request::context_string() const
{
    return m_contextString;
}

const string_t&
multiplayer_invite_handle_post_request::custom_activation_context() const
{
    return m_customActivationContext;
}

web::json::value
multiplayer_invite_handle_post_request::serialize() const
{
    web::json::value serializedObject;
    serializedObject[_T("type")] = web::json::value::string(_T("invite"));
    serializedObject[_T("sessionRef")] = m_sessionReference._Serialize();
    serializedObject[_T("version")] = web::json::value(m_version);
    serializedObject[_T("invitedXuid")] = web::json::value(m_invitedXboxUserId);

    web::json::value jsonInviteAttributes;
    stringstream_t titleIdStream;
    titleIdStream << m_titleId;
    jsonInviteAttributes[_T("titleId")] = web::json::value::string(titleIdStream.str());
    
    if (!m_contextString.empty())
    {
        jsonInviteAttributes[_T("contextString")] = web::json::value::string(m_contextString);
    }

    if (!m_customActivationContext.empty())
    {
        jsonInviteAttributes[_T("context")] = web::json::value::string(m_customActivationContext);
    }
    serializedObject[_T("inviteAttributes")] = std::move(jsonInviteAttributes);

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END