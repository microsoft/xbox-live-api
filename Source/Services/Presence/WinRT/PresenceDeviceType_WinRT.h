// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary> Defines values that indicate the device type associate with a PresenceTitleRecord.</summary>
public enum class PresenceDeviceType
{
    /// <summary>Unknown device</summary>
    Unknown = xbox::services::presence::presence_device_type::unknown,

    /// <summary>Windows Phone device</summary>
    WindowsPhone = xbox::services::presence::presence_device_type::windows_phone,

    /// <summary>Windows Phone 7 device</summary>
    WindowsPhone7 = xbox::services::presence::presence_device_type::windows_phone_7,

    /// <summary>Web device (like Xbox.com)</summary>
    Web = xbox::services::presence::presence_device_type::web,

    /// <summary>Xbox360 device</summary>
    Xbox360 = xbox::services::presence::presence_device_type::xbox_360,

    /// <summary>Games for Windows LIVE device</summary>
    PC = xbox::services::presence::presence_device_type::pc,

    /// <summary>Xbox LIVE for Windows device</summary>
    Windows8 = xbox::services::presence::presence_device_type::windows_8,

    /// <summary>Xbox One device</summary>
    XboxOne = xbox::services::presence::presence_device_type::xbox_one,

    /// <summary>Windows One Core device</summary>
    WindowsOneCore = xbox::services::presence::presence_device_type::windows_one_core,

    /// <summary>Windows One Core Mobile device</summary>
    WindowsOneCoreMobile = xbox::services::presence::presence_device_type::windows_one_core_mobile,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END