// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"
#include "PresenceTitleRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents a device record for Rich Presence. 
/// </summary>

public ref class PresenceDeviceRecord sealed
{
public:
    /// <summary>
    /// The device type associated with this record.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(DeviceType, device_type, PresenceDeviceType);

    property Windows::Foundation::Collections::IVectorView<PresenceTitleRecord^>^ PresenceTitleRecords { Windows::Foundation::Collections::IVectorView<PresenceTitleRecord^>^ get(); }

internal:
    PresenceDeviceRecord(_In_ xbox::services::presence::presence_device_record cppObj);

private:
    xbox::services::presence::presence_device_record m_cppObj;
    Windows::Foundation::Collections::IVectorView<PresenceTitleRecord^>^ m_presenceTitleRecords;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END