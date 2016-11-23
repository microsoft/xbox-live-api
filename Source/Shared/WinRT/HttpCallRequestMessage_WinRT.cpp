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