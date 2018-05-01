// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/http_call_request_message.h"
#include "http_call_request_message_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

http_call_request_message::http_call_request_message(
    _In_ const http_call_request_message_internal *internalObj
    ) :
    m_httpRequestMessageType(internalObj->get_http_request_message_type())
{
    m_requestMessageString = utils::string_t_from_internal_string(internalObj->request_message_string());
    m_requestMessageVector = utils::std_vector_from_internal_vector<unsigned char>(internalObj->request_message_vector());
}

const string_t& http_call_request_message::request_message_string() const
{
    return m_requestMessageString;
}

const std::vector<unsigned char>& http_call_request_message::request_message_vector() const
{
    return m_requestMessageVector;
}

http_request_message_type http_call_request_message::get_http_request_message_type() const
{
    return m_httpRequestMessageType;
}

http_call_request_message_internal::http_call_request_message_internal()
    : m_httpRequestMessageType(http_request_message_type::empty_message)
{
}

http_call_request_message_internal::http_call_request_message_internal(
    _In_ xsapi_internal_string messageString
    ) :
    m_requestMessageString(std::move(messageString)),
    m_httpRequestMessageType(http_request_message_type::string_message)
{
}

http_call_request_message_internal::http_call_request_message_internal(
    _In_ xsapi_internal_vector<unsigned char> messageVector
    ) :
    m_requestMessageVector(std::move(messageVector)),
    m_httpRequestMessageType(http_request_message_type::vector_message)
{
}

const xsapi_internal_string& http_call_request_message_internal::request_message_string() const
{
    return m_requestMessageString;
}

const xsapi_internal_vector<unsigned char>& http_call_request_message_internal::request_message_vector() const
{
    return m_requestMessageVector;
}

http_request_message_type
http_call_request_message_internal::get_http_request_message_type() const
{
    return m_httpRequestMessageType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END