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

class http_call_impl;

/// <summary>
/// Represents an http request message.
/// </summary>
class http_call_request_message
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    http_call_request_message();

    /// <summary>
    /// Internal function
    /// </summary>
    http_call_request_message(_In_ string_t messageString);

    /// <summary>
    /// Internal function
    /// </summary>
    http_call_request_message(_In_ xsapi_internal_string messageString);

    /// <summary>
    /// Internal function
    /// </summary>
    http_call_request_message(_In_ std::vector<unsigned char> messageVector);

    /// <summary>
    /// Internal function
    /// </summary>
    http_call_request_message(_In_ xsapi_internal_vector<unsigned char> messageVector);
    
    /// <summary>
    /// The http request message if it is a string type.
    /// </summary>
    _XSAPIIMP const string_t& request_message_string() const;

    /// <summary>
    /// The http request message if it is a buffer.
    /// </summary>
    _XSAPIIMP const std::vector<unsigned char>& request_message_vector() const;

    /// <summary>
    /// The type of message.
    /// </summary>
    _XSAPIIMP http_request_message_type get_http_request_message_type() const;

private:
    xsapi_internal_vector<unsigned char> m_requestMessageVector;
    xsapi_internal_string m_requestMessageString;
    http_request_message_type m_httpRequestMessageType;

    friend http_call_impl;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END