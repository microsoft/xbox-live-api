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
#include "shared_macros.h"
#include "xsapi/http_call_request_message.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

http_call_request_message::http_call_request_message() :
    m_httpRequestMessageType(http_request_message_type::empty_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ string_t messageString
    ) : 
    m_requestMessageString(std::move(messageString)),
    m_httpRequestMessageType(http_request_message_type::string_message)
{
}

http_call_request_message::http_call_request_message(
    _In_ std::vector<unsigned char> messageVector
    ) :
    m_requestMessageVector(std::move(messageVector)),
    m_httpRequestMessageType(http_request_message_type::vector_message)
{
}

const string_t&
http_call_request_message::request_message_string() const
{
    return m_requestMessageString;
}

const std::vector<unsigned char>&
http_call_request_message::request_message_vector() const
{
    return m_requestMessageVector;
}

http_request_message_type
http_call_request_message::get_http_request_message_type() const
{
    return m_httpRequestMessageType;
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif