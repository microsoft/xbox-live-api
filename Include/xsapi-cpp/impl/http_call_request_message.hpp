// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

http_call_request_message::http_call_request_message()
    : m_httpRequestMessageType(http_request_message_type::empty_message)
{
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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END