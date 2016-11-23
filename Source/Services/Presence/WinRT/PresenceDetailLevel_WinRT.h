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

/// <summary> Defines values that determine the level of presence detail return from the service. Choosing proper detail level could help the performance of the API. </summary>
public enum class PresenceDetailLevel
{
    /// <summary>Default detail level.</summary>
    Default = xbox::services::presence::presence_detail_level::default_level,

    /// <summary>User detail level. User presence info only, no device, title, or rich presence info.</summary>
    User = xbox::services::presence::presence_detail_level::user,

    /// <summary>Device detail level. User and device presence info only, no title, or rich presence info.</summary>
    Device = xbox::services::presence::presence_detail_level::device,

    /// <summary>Title detail level. User, device, and title presence info only, no rich presence info.</summary>
    Title = xbox::services::presence::presence_detail_level::title,

    /// <summary>All detail possible. User, device, title, and rich presence info will be provided.</summary>
    All = xbox::services::presence::presence_detail_level::all
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END