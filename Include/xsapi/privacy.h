// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

namespace xbox {
namespace services {
    class xbox_live_context_impl;
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
    permission_deny_reason();

    /// <summary>
    /// The reason why permission was denied.
    /// </summary>
    _XSAPIIMP const string_t& reason() const;

    /// <summary>
    /// If the deny reason is privilege check, this indicates which privilege failed.
    /// </summary>
    _XSAPIIMP const string_t& restricted_setting() const;

    static xbox_live_result<permission_deny_reason> _Deserializer(_In_ const web::json::value& json);

private:
    string_t m_reason;
    string_t m_restrictedSetting;
};

/// <summary>
/// Contains the result of a permission check.
/// </summary>
class permission_check_result
{
public:

    /// <summary>
    /// Creates a new permission_check_result object.
    /// </summary>
    _XSAPIIMP permission_check_result();

    /// <summary>
    /// Indicates if the user is allowed the requested access.
    /// </summary>
    _XSAPIIMP bool is_allowed() const;

    /// <summary>
    /// The permission requested.
    /// </summary>
    _XSAPIIMP const string_t& permission_requested() const;

    /// <summary>
    /// If IsAllowed is false, contains the reasons why the permissions were denied.
    /// </summary>
    _XSAPIIMP const std::vector<permission_deny_reason>& deny_reasons() const;

    /// <summary>
    /// Internal function
    /// </summary>
    _XSAPIIMP void initialize(_In_ const string_t& permissionIdRequested);

    static xbox_live_result<permission_check_result> _Deserializer(_In_ const web::json::value& json);

private:
    bool m_isAllowed;
    std::vector<permission_deny_reason> m_denyReasons;
    string_t m_permissionRequested;
};

/// <summary>
/// Contains the results of multiple permission checks.
/// </summary>
class multiple_permissions_check_result
{
public:

    /// <summary>
    /// Creates a new multiple_permissions_check_result object.
    /// </summary>
    _XSAPIIMP multiple_permissions_check_result();

    /// <summary>
    /// Xbox User Id for the target user of the permission request.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// Contains a collection of results returned when checking multiple permissions for a user.
    /// </summary>
    _XSAPIIMP const std::vector<permission_check_result>& items() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void initialize(
        _In_ uint32_t index,
        _In_ const string_t& permissionIdRequested
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<multiple_permissions_check_result> _Deserializer(_In_ const web::json::value& json);

private:
    string_t m_xboxUserId;
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
    _XSAPIIMP static const string_t communicate_using_text() { return _T("CommunicateUsingText"); }

    /// <summary>
    /// Check whether or not the user can communicate using video with the target user.
    /// </summary>
    _XSAPIIMP static const string_t communicate_using_video() { return _T("CommunicateUsingVideo"); }

    /// <summary>
    /// Check whether or not the user can communicate using voice with the target user.
    /// </summary>
    _XSAPIIMP static const string_t communicate_using_voice() { return _T("CommunicateUsingVoice"); }

    /// <summary>
    /// Check whether or not the user can view the profile of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_profile() { return _T("ViewTargetProfile"); }

    /// <summary>
    /// Check whether or not the user can view the game history of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_game_history() { return _T("ViewTargetGameHistory"); }

    /// <summary>
    /// Check whether or not the user can view the details video watching history of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_video_history() { return _T("ViewTargetVideoHistory"); }

    /// <summary>
    /// Check whether or not the user can view the detailed music listening history of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_music_history() { return _T("ViewTargetMusicHistory"); }

    /// <summary>
    /// Check whether or not the user can view the exercise info of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_exercise_info() { return _T("ViewTargetExerciseInfo"); }

    /// <summary>
    /// Check whether or not the user can view the online status of the target user.
    /// </summary>
    _XSAPIIMP static const string_t view_target_presence() { return _T("ViewTargetPresence"); }

    /// <summary>
    /// Check whether or not the user can view the details of the targets video status (extended online presence).
    /// </summary>
    _XSAPIIMP static const string_t view_target_video_status() { return _T("ViewTargetVideoStatus"); }

    /// <summary>
    /// Check whether or not the user can view the details of the targets music status (extended online presence).
    /// </summary>
    _XSAPIIMP static const string_t view_target_music_status() { return _T("ViewTargetMusicStatus"); }

    /// <summary>
    /// Check whether or not a user can play multiplayer with the target user.
    /// </summary>
    _XSAPIIMP static const string_t play_multiplayer() { return _T("PlayMultiplayer"); }

    /// <summary>
    /// Checks whether or not the user can view information about how audio buffers are broadcast for the target user.
    /// </summary>
    _XSAPIIMP static const string_t broadcast_with_twitch() { return _T("BroadcastWithTwitch"); }

    /// <summary>
    /// Check whether or not the user can view the user-created content of other users.
    /// </summary>
    _XSAPIIMP static const string_t view_target_user_created_content() { return _T("ViewTargetUserCreatedContent"); }
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
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<string_t>>> get_avoid_list();

    /// <summary>
    /// Check a single permission with a single target user.
    /// </summary>
    /// <param name="permissionId">The ID of the permission to check.
    /// See microsoft::xbox::services::privacy::permission_id_constants for the latest options.</param>
    /// <param name="targetXboxUserId">The target user's xbox Live ID for validation</param>
    /// <returns>The permission check result against a single user.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/{requestorId}/permission/validate
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<permission_check_result>> check_permission_with_target_user(
        _In_ const string_t& permissionId,
        _In_ const string_t& targetXboxUserId
        );

    /// <summary>
    /// Check multiple permissions with multiple target users.
    /// </summary>
    /// <param name="permissionIds">The collection of IDs of the permissions to check.
    /// See microsoft::xbox::services::privacy::permission_id_constants for the latest options.</param>
    /// <param name="targetXboxUserIds">The collection of target Xbox user IDs to check permissions against.</param>
    /// <returns>A multiple permission check result which contains a collection of permission information.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 POST /users/{requestorId}/permission/validate
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<multiple_permissions_check_result>>> check_multiple_permissions_with_multiple_target_users(
        _In_ std::vector<string_t> permissionIds,
        _In_ std::vector<string_t> targetXboxUserIds
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
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<string_t>>> get_mute_list();

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
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<string_t>>> get_avoid_or_mute_list(_In_ const string_t& subPathName);

private:
    privacy_service() {};

    privacy_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    static const string_t avoid_mute_list_sub_path(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& subPathName
        );

    static const string_t permission_validate_sub_path(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& setting,
        _In_ const string_t& targetXboxUserId
        );

    static const string_t permission_batch_validate_sub_path(
        _In_ const string_t& xboxUserId
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend xbox_live_context_impl;
};

} } }