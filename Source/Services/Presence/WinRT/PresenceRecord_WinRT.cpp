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
#include "PresenceRecord_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

PresenceRecord::PresenceRecord(
    _In_ xbox::services::presence::presence_record cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
    m_presenceDeviceRecords = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceDeviceRecord, xbox::services::presence::presence_device_record>(
        m_cppObj.presence_device_records()
        )->GetView();
}

Windows::Foundation::Collections::IVectorView<PresenceDeviceRecord^>^ 
PresenceRecord::PresenceDeviceRecords::get()
{
    return m_presenceDeviceRecords;
}

bool PresenceRecord::IsUserPlayingTitle(_In_ uint32_t titleId)
{
    return m_cppObj.is_user_playing_title(titleId);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END