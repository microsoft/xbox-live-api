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
#include "MultiplayerSession_WinRT.h"
#include "WriteSessionStatus_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents the result of a write session async.
/// </summary>
public ref class WriteSessionResult sealed
{
public:

    /// <summary>
    /// Indicates whether the WriteSessionAsync was successful.
    /// </summary>
    property bool Succeeded { bool get(); }

    /// <summary>
    /// The MultiplayerSession returned from the WriteSessionAsync (if any).
    /// </summary>
    property MultiplayerSession^ Session { MultiplayerSession^ get(); }

    /// <summary>
    /// A WriteSessionStatus enum which indicates the status of the session write operation.
    /// </summary>
    property WriteSessionStatus Status { WriteSessionStatus get(); }

internal:

    WriteSessionResult();

    WriteSessionResult(
        _In_ bool succeeded,
        _In_opt_ MultiplayerSession^ session,
        _In_ WriteSessionStatus writeSessionStatus
        );

    static WriteSessionStatus ConvertHttpStatusToWriteSessionStatus(
        _In_ int32_t httpStatusCode
        );

private:
    bool m_succeeded;
    WriteSessionStatus m_status;
    MultiplayerSession^ m_session;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END