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

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>Defines values that indicate the title's view state as last reported to the Presence Service.</summary>
public enum class PresenceTitleViewState
{
    /// <summary>Unknown view state.</summary>
    Unknown = xbox::services::presence::presence_title_view_state::unknown,

    /// <summary>The title's view is using the full screen.</summary>
    FullScreen = xbox::services::presence::presence_title_view_state::full_screen,

    /// <summary>The title's view is using part of the screen with another application snapped.</summary>
    Filled = xbox::services::presence::presence_title_view_state::filled,

    /// <summary>The title's view is snapped with another application using a part of the screen.</summary>
    Snapped = xbox::services::presence::presence_title_view_state::snapped,

    /// <summary>The title's running in the background and is not visible.</summary>
    Background = xbox::services::presence::presence_title_view_state::background
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END