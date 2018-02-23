// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class http_call_request_message_internal
{
public:
    http_call_request_message_internal();

    http_call_request_message_internal(_In_ xsapi_internal_string messageString);

    http_call_request_message_internal(_In_ xsapi_internal_vector<unsigned char> messageVector);

    _XSAPIIMP const xsapi_internal_string& request_message_string() const;

    _XSAPIIMP const xsapi_internal_vector<unsigned char>& request_message_vector() const;

    _XSAPIIMP http_request_message_type get_http_request_message_type() const;

private:
    xsapi_internal_vector<unsigned char> m_requestMessageVector;
    xsapi_internal_string m_requestMessageString;
    http_request_message_type m_httpRequestMessageType;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END