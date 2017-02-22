// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "UserPresenceState_WinRT.h"
#include "SocialManagerPresenceTitleRecord_WinRT.h"
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public ref class SocialManagerPresenceRecord sealed
{
public:
    /// <summary>
    /// The user's presence state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(UserState, user_state, Microsoft::Xbox::Services::Presence::UserPresenceState);

    /// <summary>
    /// Collection of presence title record objects returned by a request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<SocialManagerPresenceTitleRecord^>^ PresenceTitleRecords
    {
        Windows::Foundation::Collections::IVectorView<SocialManagerPresenceTitleRecord^>^ get();
    };

    /// <summary>
    /// Returns whether the user is playing this title id
    /// </summary>
    bool IsUserPlayingTitle(_In_ uint32_t titleId);

internal:
    SocialManagerPresenceRecord(
        _In_ xbox::services::social::manager::social_manager_presence_record cppObj
        );

private:
    xbox::services::social::manager::social_manager_presence_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END