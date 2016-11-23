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
#include "MultiplayerSessionReference_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionReference::MultiplayerSessionReference(
xbox::services::multiplayer::multiplayer_session_reference cppObj
    ) :
    m_cppObj(cppObj)
{
}

MultiplayerSessionReference::MultiplayerSessionReference(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ sessionTemplateName,
    _In_ Platform::String^ sessionName
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    THROW_INVALIDARGUMENT_IF_NULL(sessionTemplateName);
    THROW_INVALIDARGUMENT_IF_NULL(sessionName);

    m_cppObj = xbox::services::multiplayer::multiplayer_session_reference(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(sessionTemplateName),
        STRING_T_FROM_PLATFORM_STRING(sessionName)
        );
}

Platform::String^ 
MultiplayerSessionReference::ToUriPath()
{
    CONVERT_STD_EXCEPTION(
        return ref new Platform::String(m_cppObj.to_uri_path().c_str())
    );
}

MultiplayerSessionReference^ 
MultiplayerSessionReference::ParseFromUriPath(
    _In_ Platform::String^ uriPath
    )
{
    CONVERT_STD_EXCEPTION(
        return ref new MultiplayerSessionReference(
            xbox::services::multiplayer::multiplayer_session_reference::parse_from_uri_path(
                STRING_T_FROM_PLATFORM_STRING(uriPath)
                )
            )
        );
}

const xbox::services::multiplayer::multiplayer_session_reference&
MultiplayerSessionReference::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END