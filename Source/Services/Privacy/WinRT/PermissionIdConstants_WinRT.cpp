// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "PermissionIdConstants_WinRT.h"
#include "Macros_WinRT.h"

using namespace Microsoft::Xbox::Services::Privacy;
using namespace Microsoft::Xbox::Services;
using namespace Platform;
using namespace xbox::services::privacy;

Platform::String^
PermissionIdConstants::CommunicateUsingText::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::communicate_using_text());
}

Platform::String^
PermissionIdConstants::CommunicateUsingVideo::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::communicate_using_video());
}

Platform::String^
PermissionIdConstants::CommunicateUsingVoice::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::communicate_using_voice());
}

Platform::String^
PermissionIdConstants::ViewTargetProfile::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_profile());
}

Platform::String^
PermissionIdConstants::ViewTargetGameHistory::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_game_history());
}

Platform::String^
PermissionIdConstants::ViewTargetVideoHistory::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_video_history());
}

Platform::String^
PermissionIdConstants::ViewTargetMusicHistory::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_music_history());
}

Platform::String^
PermissionIdConstants::ViewTargetExerciseInfo::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_exercise_info());
}

Platform::String^
PermissionIdConstants::ViewTargetPresence::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_presence());
}

Platform::String^
PermissionIdConstants::ViewTargetVideoStatus::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_video_status());
}

Platform::String^
PermissionIdConstants::ViewTargetMusicStatus::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_music_status());
}

Platform::String^
PermissionIdConstants::PlayMultiplayer::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::play_multiplayer());
}

Platform::String^
PermissionIdConstants::BroadcastWithTwitch::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::broadcast_with_twitch());
}

Platform::String^
PermissionIdConstants::ViewTargetUserCreatedContent::get()
{
    return PLATFORM_STRING_FROM_STRING_T(permission_id_constants::view_target_user_created_content());
}