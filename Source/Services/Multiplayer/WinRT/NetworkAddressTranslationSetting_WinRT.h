// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Defines values that indicate the current NAT setting for a device connecting to Xbox LIVE.
/// </summary>
public enum class NetworkAddressTranslationSetting
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown = xbox::services::multiplayer::network_address_translation_setting::unknown,

    /// <summary>
    /// Can connect with any other devices regardless of their NAT setting.
    /// </summary>
    Open = xbox::services::multiplayer::network_address_translation_setting::open,

    /// <summary>
    /// Devices using moderate NAT settings can only connect with other devices using moderate or open settings.
    /// </summary>
    Moderate = xbox::services::multiplayer::network_address_translation_setting::moderate,

    /// <summary>
    /// Devices using strict NAT settings can only connect with other devices using open NAT settings.
    /// </summary>
    Strict = xbox::services::multiplayer::network_address_translation_setting::strict
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END