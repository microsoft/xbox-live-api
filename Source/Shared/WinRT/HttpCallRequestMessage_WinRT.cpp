// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "HttpCallRequestMessage_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

HttpCallRequestMessage::HttpCallRequestMessage(
    _In_ xbox::services::http_call_request_message cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
    auto nativeRequestVector = m_cppObj.request_message_vector();
    if(!nativeRequestVector.empty())
    {
        m_requestMessageVector = ref new Platform::Array<byte>(&nativeRequestVector[0], static_cast<uint32>(nativeRequestVector.size()));
    }
}

Platform::Array<byte>^
HttpCallRequestMessage::RequestMessageVector::get()
{
    return m_requestMessageVector;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END