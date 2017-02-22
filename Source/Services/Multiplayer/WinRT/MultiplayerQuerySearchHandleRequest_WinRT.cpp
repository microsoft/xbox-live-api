// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerQuerySearchHandleRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::multiplayer;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerQuerySearchHandleRequest::MultiplayerQuerySearchHandleRequest(
    _In_ xbox::services::multiplayer::multiplayer_query_search_handle_request cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

MultiplayerQuerySearchHandleRequest::MultiplayerQuerySearchHandleRequest(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ sessionTemplateName
    ) :
    m_cppObj(
        multiplayer_query_search_handle_request(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(sessionTemplateName)
        )
    )
{
}

const multiplayer_query_search_handle_request&
MultiplayerQuerySearchHandleRequest::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END