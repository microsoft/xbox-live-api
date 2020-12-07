// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Enumerates the type of data contained in the http request body.
/// </summary>
enum http_request_message_type
{
    /// <summary>
    /// No message.
    /// </summary>
    empty_message,

    /// <summary>
    /// The message is of type string.
    /// </summary>
    string_message,

    /// <summary>
    /// The message is of type vector, and acts as a memory buffer.
    /// </summary>
    vector_message
};

/// <summary>
/// Represents an http request message.
/// </summary>
class http_call_request_message
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline http_call_request_message();

    /// <summary>
    /// The http request message if it is a string type.
    /// </summary>
    inline const string_t& request_message_string() const;

    /// <summary>
    /// The http request message if it is a buffer.
    /// </summary>
    inline const std::vector<unsigned char>& request_message_vector() const;

    /// <summary>
    /// The type of message.
    /// </summary>
    inline http_request_message_type get_http_request_message_type() const;

private:
    std::vector<unsigned char> m_requestMessageVector;
    string_t m_requestMessageString;
    http_request_message_type m_httpRequestMessageType;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#include "impl/http_call_request_message.hpp"