// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerGetSessionsRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::multiplayer;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerGetSessionsRequest::MultiplayerGetSessionsRequest(
    _In_ xbox::services::multiplayer::multiplayer_get_sessions_request cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

MultiplayerGetSessionsRequest::MultiplayerGetSessionsRequest(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ uint32 maxItems
    ) :
    m_cppObj(
        multiplayer_get_sessions_request(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            maxItems
            )
        )
{
}

IVectorView<Platform::String^>^
MultiplayerGetSessionsRequest::XboxUserIdsFilter::get()
{
    return m_xboxUserIds;
}

void
MultiplayerGetSessionsRequest::XboxUserIdsFilter::set(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
    )
{
    m_xboxUserIds = std::move(xboxUserIds);
    m_cppObj.set_xbox_user_ids_filter(
        Microsoft::Xbox::Services::System::UtilsWinRT::CovertVectorViewToStdVectorString(m_xboxUserIds)
        );
}

multiplayer_get_sessions_request
MultiplayerGetSessionsRequest::GetCppObj()
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END