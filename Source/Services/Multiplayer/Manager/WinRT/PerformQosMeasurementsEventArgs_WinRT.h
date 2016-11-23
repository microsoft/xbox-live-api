//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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