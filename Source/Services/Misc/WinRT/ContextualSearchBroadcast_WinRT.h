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
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Contains information about a contextual search broadcast.</summary>
public ref class ContextualSearchBroadcast sealed
{
public:
    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Returns the broadcast provider, for example "Twitch". 
    /// This is leveraged to enable differing logic to handle content from different providers
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Provider, provider);

    /// <summary>
    /// The identifier of the broadcaster on the provider. This is leveraged to created deeplinks to the broadcast.  
    /// In the example of Twitch, this is the Twitch user id.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(BroadcasterIdFromProvider, broadcaster_id_from_provider);

    /// <summary>
    /// The current number of viewers in the broadcast, per the last update from Presence.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Viewers, viewers, uint64);

    /// <summary>
    /// The date value of when the broadcast started
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartedDate, started_date);

    /// <summary>
    /// The current stats the title has configured for Contextual Search and what their current values are.  
    /// Additional information on how to surface the information is contained in the configuration handler.  
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ CurrentStats
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

internal:
    ContextualSearchBroadcast(
        _In_ xbox::services::contextual_search::contextual_search_broadcast cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_broadcast m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_currentStats;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
