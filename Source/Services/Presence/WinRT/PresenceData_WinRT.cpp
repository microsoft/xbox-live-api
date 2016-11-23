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
#include "PresenceData_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::presence;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

PresenceData::PresenceData(
    _In_ presence_data cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
    m_presenceTokenIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(
        m_cppObj.presence_token_ids()
        )->GetView();
}

PresenceData::PresenceData(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ presenceId
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj = presence_data(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(presenceId)
            )
        );

    m_presenceTokenIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(
        m_cppObj.presence_token_ids()
        )->GetView();
}

PresenceData::PresenceData(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ presenceId,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ presenceTokenIds
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj = presence_data(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(presenceId),
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(presenceTokenIds)
            )
        );

    m_presenceTokenIds = presenceTokenIds;
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ 
PresenceData::PresenceTokenIds::get()
{
    return m_presenceTokenIds;
}

const xbox::services::presence::presence_data& 
PresenceData::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END