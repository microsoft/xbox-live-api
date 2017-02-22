// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"
#include "UserPresenceState_WinRT.h"
#include "PresenceTitleRecord_WinRT.h"
#include "PresenceDeviceRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents a record for Rich Presence. 
/// </summary>

public ref class PresenceRecord sealed
{
public:
    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The user's presence state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(UserState, user_state, UserPresenceState);

    /// <summary>
    /// Collection of PresenceTitleRecord objects returned by a request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<PresenceDeviceRecord^>^ PresenceDeviceRecords { Windows::Foundation::Collections::IVectorView<PresenceDeviceRecord^>^ get(); }

    /// <summary>
    /// Returns whether the user is playing this title id
    /// </summary>
    bool IsUserPlayingTitle(_In_ uint32_t titleId);

internal:
    PresenceRecord(_In_ xbox::services::presence::presence_record cppObj);

private:
    xbox::services::presence::presence_record m_cppObj;
    Windows::Foundation::Collections::IVectorView<PresenceDeviceRecord^>^ m_presenceDeviceRecords;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END