// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/http_call_request_message.h"
#include "HttpRequestMessageType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Represents the message of an HTTP call request.
/// </summary>
public ref class HttpCallRequestMessage sealed
{
public:
    /// <summary>
    /// The http request message if the message is a string type.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(RequestMessageString, request_message_string);

    /// <summary>
    /// The http request message if the message is a buffer.
    /// </summary>
    property Platform::Array<byte>^ RequestMessageVector { Platform::Array<byte>^ get(); }

    /// <summary>
    /// The message type.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(GetHttpRequestMessageType, get_http_request_message_type, HttpRequestMessageType);

internal:
    HttpCallRequestMessage(_In_ xbox::services::http_call_request_message cppObj);

private:
    xbox::services::http_call_request_message m_cppObj;
    Platform::Array<byte>^ m_requestMessageVector;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END