// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "MultiplayerEventArgs_WinRT.h"
#include "MultiplayerMember_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when a new game member joins the game. 
/// </summary>
public ref class PerformQosMeasurementsEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// A list of members that joined the game.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ AddressToDeviceTokens
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

internal:
    PerformQosMeasurementsEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::perform_qos_measurements_event_args> m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_addressToDeviceTokenMap;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END