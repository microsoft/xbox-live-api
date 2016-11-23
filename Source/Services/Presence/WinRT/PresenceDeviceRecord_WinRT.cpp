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
#include "PresenceDeviceRecord_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

PresenceDeviceRecord::PresenceDeviceRecord(
    _In_ xbox::services::presence::presence_device_record cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_presenceTitleRecords = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceTitleRecord, xbox::services::presence::presence_title_record>(
        m_cppObj.presence_title_records()
        )->GetView();
}

Windows::Foundation::Collections::IVectorView<PresenceTitleRecord^>^
PresenceDeviceRecord::PresenceTitleRecords::get()
{
    return m_presenceTitleRecords;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END