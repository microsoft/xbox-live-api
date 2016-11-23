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

multiplayer_transfer_handle_post_request::multiplayer_transfer_handle_post_request() :
    m_version(0)
{
}

multiplayer_transfer_handle_post_request::multiplayer_transfer_handle_post_request(
    _In_ multiplayer_session_reference targetSessionReference,
    _In_ multiplayer_session_reference originSessionReference,
    _In_ uint32_t version
    ) :
    m_targetSessionReference(std::move(targetSessionReference)),
    m_originSessionReference(std::move(originSessionReference)),
    m_version(version)
{
    XSAPI_ASSERT(!m_targetSessionReference.is_null());
    XSAPI_ASSERT(!m_originSessionReference.is_null());
}

const multiplayer_session_reference&
multiplayer_transfer_handle_post_request::origin_session_reference() const
{
    return m_originSessionReference;
}

const multiplayer_session_reference&
multiplayer_transfer_handle_post_request::target_session_reference() const
{
    return m_targetSessionReference;
}

uint32_t 
multiplayer_transfer_handle_post_request::version() const
{
    return m_version;
}

web::json::value 
multiplayer_transfer_handle_post_request::serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("type")] = web::json::value::string(_T("transfer"));
    serializedObject[_T("sessionRef")] = m_targetSessionReference._Serialize();
    serializedObject[_T("version")] = web::json::value(m_version);
    serializedObject[_T("originSessionRef")] = m_originSessionReference._Serialize();

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END