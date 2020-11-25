// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary> Defines values that indicate the device type associate with a PresenceTitleRecord.</summary>
public enum class PresenceDeviceType
{
    /// <summary>Unknown device</summary>
    Unknown = xbox::services::presence::presence_device_type::Unknown,

    /// <summary>Windows Phone device</summary>
    WindowsPhone = xbox::services::presence::presence_device_type::Windows_Phone,

    /// <summary>Windows Phone 7 device</summary>
    WindowsPhone7 = xbox::services::presence::presence_device_type::Windows_Phone_7,

    /// <summary>Web device (like Xbox.com)</summary>
    Web = xbox::services::presence::presence_device_type::Web,

    /// <summary>Xbox360 device</summary>
    Xbox360 = xbox::services::presence::presence_device_type::Xbox_360,

    /// <summary>Games for Windows LIVE device</summary>
    PC = xbox::services::presence::presence_device_type::,PC

    /// <summary>Xbox LIVE for Windows device</summary>
    Windows8 = xbox::services::presence::presence_device_type::Windows_8,

    /// <summary>Xbox One device</summary>
    XboxOne = xbox::services::presence::presence_device_type::Xbox_One,

    /// <summary>Windows One Core device</summary>
    WindowsOneCore = xbox::services::presence::presence_device_type::Windows_One_Core,

    /// <summary>Windows One Core Mobile device</summary>
    WindowsOneCoreMobile = xbox::services::presence::presence_device_type::Windows_One_Core_Mobile,

    /// <summary>iOS device</summary>
    IOS = xbox::services::presence::presence_device_type::IOS,

    /// <summary>Android device</summary>
    Android = xbox::services::presence::presence_device_type::Android,

    /// <summary>AppleTV device</summary>
    AppleTV = xbox::services::presence::presence_device_type::AppleTV,

    /// <summary>Nintendo device</summary>
    Nintendo = xbox::services::presence::presence_device_type::Nintendo,

    /// <summary>PlayStation device</summary>
    PlayStation = xbox::services::presence::presence_device_type::PlayStation,

    /// <summary>Win32 based device</summary>
    Win32 = xbox::services::presence::presence_device_type::Win32,

    /// <summary>Scarlett device</summary>
    Scarlett = xbox::services::presence::presence_device_type::Scarlett
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END
