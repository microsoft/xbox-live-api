// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#pragma once
#include "xsapi/privacy.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

/// <summary>
/// Manages constant values for permission IDs. 
/// </summary>
public ref class PermissionIdConstants sealed
{
public:
    /// <summary>
    /// Check whether or not the user can send a message with text content to the target user.
    /// </summary>
    static property Platform::String^ CommunicateUsingText { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can communicate using video with the target user.
    /// </summary>
    static property Platform::String^ CommunicateUsingVideo { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can communicate using voice with the target user.
    /// </summary>
    static property Platform::String^ CommunicateUsingVoice { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the profile of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetProfile { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the game history of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetGameHistory { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the details video watching history of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetVideoHistory { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the detailed music listening history of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetMusicHistory { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the exercise info of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetExerciseInfo { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the online status of the target user.
    /// </summary>
    static property Platform::String^ ViewTargetPresence { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the details of the targets video status (extended online presence).
    /// </summary>
    static property Platform::String^ ViewTargetVideoStatus { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the details of the targets music status (extended online presence).
    /// </summary>
    static property Platform::String^ ViewTargetMusicStatus { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not a user can play multiplayer with the target user.
    /// </summary>
    static property Platform::String^ PlayMultiplayer { Platform::String^ get(); }
    
    /// <summary>
    /// Checks whether or not the user can view information about how audio buffers are broadcast for the target user.
    /// </summary>
    static property Platform::String^ BroadcastWithTwitch { Platform::String^ get(); }
    
    /// <summary>
    /// Check whether or not the user can view the user-created content of other users.
    /// </summary>
    static property Platform::String^ ViewTargetUserCreatedContent { Platform::String^ get(); }

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END