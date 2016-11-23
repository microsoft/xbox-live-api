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

multiplayer_activity_handle_post_request::multiplayer_activity_handle_post_request() :
    m_version(0)
{
}

multiplayer_activity_handle_post_request::multiplayer_activity_handle_post_request(
    _In_ multiplayer_session_reference sessionReference, 
    _In_ uint32_t version
    ) :
    m_sessionReference(std::move(sessionReference)),
    m_version(version)
{
    XSAPI_ASSERT(!m_sessionReference.is_null());
}

const multiplayer_session_reference&
multiplayer_activity_handle_post_request::session_reference() const
{
    return m_sessionReference;
}

uint32_t 
multiplayer_activity_handle_post_request::version() const
{
    return m_version;
}

web::json::value 
multiplayer_activity_handle_post_request::serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("type")] = web::json::value::string(_T("activity"));
    serializedObject[_T("sessionRef")] = m_sessionReference._Serialize();
    serializedObject[_T("version")] = web::json::value(m_version);

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END