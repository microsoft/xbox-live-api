// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"
#include "MultiplayerMember_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when it should provide qos measurement results between
/// itself and a list of remote clients.
/// </summary>
public ref class PerformQosMeasurementsEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// A map of connection addresses and device tokens to run qos on.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ AddressToDeviceTokens
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

internal:
    /// <summary>
    /// Internal function.
    /// </summary>
    PerformQosMeasurementsEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_addressToDeviceTokenMap;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
