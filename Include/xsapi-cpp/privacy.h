// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/privacy_c.h"

namespace xbox {
namespace services {
    class xbox_live_context;
/// <summary>
/// Contains classes and enumerations that let you retrieve
/// information about a player's privacy settings from Xbox Live.
/// </summary>
namespace privacy {

/// <summary>
/// Contains the reason why permission was denied.
/// </summary>
class permission_deny_reason
{
public:
    inline permission_deny_reason(const XblPermissionDenyReasonDetails& reasonDetails);

    /// <summary>
    /// The reason why permission was denied.
    /// </summary>
    inline string_t reason() const;

    /// <summary>
    /// If the deny reason is privilege check, this indicates which privilege failed.
    /// </summary>
    inline string_t restricted_setting() const;

private:
    XblPermissionDenyReasonDetails m_reasonDetails;
};

/// <summary>
/// Contains the result of a permission check.
/// </summary>
class permission_check_result
{
public:
    inline permission_check_result() {}
    inline permission_check_result(const XblPermissionCheckResult* result);

    /// <summary>
    /// Indicates if the user is allowed the requested access.
    /// </summary>
    inline bool is_allowed() const;

    /// <summary>
    /// The permission requested.
    /// </summary>
    inline string_t permission_requested() const;

    /// <summary>
    /// If IsAllowed is false, contains the reasons why the permissions were denied.
    /// </summary>
    inline const std::vector<permission_deny_reason>& deny_reasons() const;

private:
    XblPermissionCheckResult m_result{};
    std::vector<permission_deny_reason> m_reasons;
};

/// <summary>
/// Contains the results of multiple permission checks.
/// </summary>
class multiple_permissions_check_result
{
public:
    inline multiple_permissions_check_result(const XblPermissionCheckResult* results, size_t resultCount, string_t target);

    /// <summary>
    /// Xbox User Id OR anonymous user type for the permission request.
    /// </summary>
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// Contains a collection of results returned when checking multiple permissions for a user.
    /// </summary>
    inline const std::vector<permission_check_result>& items() const;

private:
    string_t m_target;
    std::vector<permission_check_result> m_items;
};

/// <summary>
/// Manages constant values for permission IDs. 
/// </summary>
class permission_id_constants
{
public:
    /// <summary>
    /// Check whether or not the user can send a message with text content to the target user.
    /// </summary>
    static const string_t communicate_using_text() { return _T("CommunicateUsingText"); }

    /// <summary>
    /// Check whether or not the user can communicate using video with the target user.
    /// </summary>
    static const string_t communicate_using_video() { return _T("CommunicateUsingVideo"); }

    /// <summary>
    /// Check whether or not the user can communicate using voice with the target user.
    /// </summary>
    static const string_t communicate_using_voice() { return _T("CommunicateUsingVoice"); }

    /// <summary>
    /// Check whether or not the user can view the profile of the target user.
    /// </summary>
    static const string_t view_target_profile() { return _T("ViewTargetProfile"); }

    /// <summary>
    /// Check whether or not the user can view the game history of the target user.
    /// </summary>
    static const string_t view_target_game_history() { return _T("ViewTargetGameHistory"); }

    /// <summary>
    /// Check whether or not the user can view the details video watching history of the target user.
    /// </summary>
    static const string_t view_target_video_history() { return _T("ViewTargetVideoHistory"); }

    /// <summary>
    /// Check whether or not the user can view the detailed music listening history of the target user.
    /// </summary>
    static const string_t view_target_music_history() { return _T("ViewTargetMusicHistory"); }

    /// <summary>
    /// Check whether or not the user can view the exercise info of the target user.
    /// </summary>
    static const string_t view_target_exercise_info() { return _T("ViewTargetExerciseInfo"); }

    /// <summary>
    /// Check whether or not the user can view the online status of the target user.
    /// </summary>
    static const string_t view_target_presence() { return _T("ViewTargetPresence"); }

    /// <summary>
    /// Check whether or not the user can view the details of the targets video status (extended online presence).
    /// </summary>
    static const string_t view_target_video_status() { return _T("ViewTargetVideoStatus"); }

    /// <summary>
    /// Check whether or not the user can view the details of the targets music status (extended online presence).
    /// </summary>
    static const string_t view_target_music_status() { return _T("ViewTargetMusicStatus"); }

    /// <summary>
    /// Check whether or not a user can play multiplayer with the target user.
    /// </summary>
    static const string_t play_multiplayer() { return _T("PlayMultiplayer"); }

    /// <summary>
    /// Checks whether or not the user can view information about how audio buffers are broadcast for the target user.
    /// </summary>
    static const string_t broadcast_with_twitch() { return _T("BroadcastWithTwitch"); }

    /// <summary>
    /// Check whether or not the user can view the user-created content of other users.
    /// </summary>
    static const string_t view_target_user_created_content() { return _T("ViewTargetUserCreatedContent"); }
};

/// <summary>
/// Special strings that can be passed in as to check_permission APIs to check permission for different
/// classes of non-Xbox Live users.
/// </summary>
class anonymous_user_type_constants
{
public:
    /// <summary>
    /// A non Xbox Live user
    /// </summary>
    static string_t cross_network_user() { return _T("crossNetworkUser"); }

    /// <summary>
    /// A non Xbox Live user that a title recognizes as an in-game friend.
    /// </summary>
    static string_t crost_network_friend() { return _T("crossNetworkFriend"); }
};

/// <summary>
/// Provides an endpoint for managing privacy settings.
/// </summary>
class privacy_service
{
public:
    /// <summary>
    /// Get the list of Xbox Live Ids the calling user should avoid during multiplayer matchmaking.
    /// </summary>
    /// <returns>A collection of XboxUserIds that correspond to the calling user's avoid list.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/xuid({xuid})/people/avoid
    /// </remarks>
    inline pplx::task<xbox_live_result<std::vector<string_t>>> get_avoid_list();

    /// <summary>
    /// Check a single permission with a single target user.
    /// </summary>
    /// <param name="permissionId">The ID of the permission to check.
    /// See microsoft::xbox::services::privacy::permission_id_constants for the latest options.</param>
    /// <param name="target">The target user's Xbox Live ID OR an anonymous user type string from anonymous_user_type_constants.</param>
    /// <returns>The permission check result against a single user.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/{requestorId}/permission/validate
    /// </remarks>
    inline pplx::task<xbox_live_result<permission_check_result>> check_permission_with_target_user(
        _In_ const string_t& permissionId,
        _In_ const string_t& target
    );

    /// <summary>
    /// Check multiple permissions with multiple target users.
    /// </summary>
    /// <param name="permissionIds">The collection of IDs of the permissions to check.
    /// See microsoft::xbox::services::privacy::permission_id_constants for the latest options.</param>
    /// <param name="targets">The collection of target Xbox user IDs and/or anonymous user types to check permissions against.</param>
    /// <returns>A multiple permission check result which contains a collection of permission information.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 POST /users/{requestorId}/permission/validate
    /// </remarks>
    inline pplx::task<xbox_live_result<std::vector<multiple_permissions_check_result>>> check_multiple_permissions_with_multiple_target_users(
        _In_ const std::vector<string_t>& permissionIds,
        _In_ const std::vector<string_t>& targets
    );

    /// <summary>
    /// Get the list of Xbox Live Ids that the calling user should not hear (mute) during multiplayer matchmaking.
    /// </summary>
    /// <returns>The collection of Xbox user IDs that represent the mute list for a user.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/xuid({xuid})/people/mute
    /// </remarks>
    inline pplx::task<xbox_live_result<std::vector<string_t>>> get_mute_list();

    /// <summary>
    /// Get either a user's avoid list or a user's mute list.
    /// </summary>
    /// <param name="subPathName">A string indicating what type of list to get for the user.
    /// The valid values are "avoid" and "mute". </param>
    /// <returns>The collection of Xbox user IDs that represent the either the avoid list or the mute list for a user.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/xuid({xuid})/people/mute
    /// </remarks>
    inline pplx::task<xbox_live_result<std::vector<string_t>>> get_avoid_or_mute_list(_In_ const string_t& subPathName);

    inline privacy_service(const privacy_service& other);
    inline privacy_service& operator=(privacy_service other);
    inline ~privacy_service();

private:
    inline privacy_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext;

    friend xbox_live_context;
};

} } }

#include "impl/privacy.hpp"