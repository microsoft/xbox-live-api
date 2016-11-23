//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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