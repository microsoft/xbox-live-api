// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/multiplayer_manager.h"
#include "MultiplayerEventType_WinRT.h"
#include "MultiplayerSessionType_WinRT.h"
#include "MultiplayerEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Represents a multiplayer event. 
/// Based on the MultiplayerEventType, you must cast the EventArgs to the appropriate class.
/// </summary>
public ref class MultiplayerEvent sealed
{
public:
    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    property int ErrorCode
    {
        int get();
    };

    /// <summary>
    /// Returns call specific debug information if join fails.
    /// This value is not localized, so only use for debugging purposes.
    /// </summary>
    property Platform::String^ ErrorMessage
    {
        Platform::String^ get();
    };

    /// <summary>
    /// A pointer to the application-defined data passed into the initiating method.
    /// </summary>
    property context_t Context
    {
        context_t get();
    };

    /// <summary>
    /// Type of the event triggered.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(EventType, event_type, Microsoft::Xbox::Services::Multiplayer::Manager::MultiplayerEventType);

    /// <summary>
    /// You must cast this to one of the event arg classes to retrieve the data for that particular event.
    /// </summary>
    property MultiplayerEventArgs^ EventArgs
    {
        MultiplayerEventArgs^ get();
    }

    /// <summary>
    /// The multiplayer session type this event was triggered for. Depending upon the session type,
    /// you can then retrieve the latest lobby or game session from the MultiplayerManager class.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(SessionType, session_type, Microsoft::Xbox::Services::Multiplayer::Manager::MultiplayerSessionType);

internal:
    MultiplayerEvent(
        _In_ xbox::services::multiplayer::manager::multiplayer_event cppObj
        );

    const xbox::services::multiplayer::manager::multiplayer_event& GetCppObj() const;

private:
    xbox::services::multiplayer::manager::multiplayer_event m_cppObj;
    Platform::Exception^ m_errorCode;
    Platform::String^ m_errorMessage;
    context_t m_context;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
