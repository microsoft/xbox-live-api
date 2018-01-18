// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/http_call_request_message.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

http_call_request_message::http_call_request_message() :
    m_httpRequestMessageType(http_request_message_type::empty_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ string_t messageString
) :
    m_requestMessageString(utils::internal_string_from_external_string(messageString)),
    m_httpRequestMessageType(http_request_message_type::string_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ xsapi_internal_string messageString
    ) : 
    m_requestMessageString(std::move(messageString)),
    m_httpRequestMessageType(http_request_message_type::string_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ std::vector<unsigned char> messageVector
    ) :
    m_requestMessageVector(utils::internal_vector_from_std_vector(messageVector)),
    m_httpRequestMessageType(http_request_message_type::vector_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ xsapi_internal_vector<unsigned char> messageVector
    ) :
    m_requestMessageVector(std::move(messageVector)),
    m_httpRequestMessageType(http_request_message_type::vector_message)
{
}

string_t http_call_request_message::request_message_string() const
{
    return utils::external_string_from_internal_string(m_requestMessageString);
}

std::vector<unsigned char> http_call_request_message::request_message_vector() const
{
    return std::vector<unsigned char>(m_requestMessageVector.begin(), m_requestMessageVector.end());
}

http_request_message_type
http_call_request_message::get_http_request_message_type() const
{
    return m_httpRequestMessageType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END