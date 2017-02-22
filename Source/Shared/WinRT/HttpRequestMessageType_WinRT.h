// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Enumeration values that indicate the message type of an HTTP request.
/// </summary>
public enum class HttpRequestMessageType
{
    /// <summary>
    /// The message is empty.
    /// </summary>
    EmptyMessage = xbox::services::http_request_message_type::empty_message,

    /// <summary>
    /// The message is of type string.
    /// </summary>
    StringMessage = xbox::services::http_request_message_type::string_message,

    /// <summary>
    /// The message is of type vector, and acts as a memory buffer.
    /// </summary>
    VectorMessage = xbox::services::http_request_message_type::vector_message
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END