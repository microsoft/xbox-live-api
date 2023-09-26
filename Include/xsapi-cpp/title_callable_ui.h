// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !XSAPI_NO_PPL
#include "xsapi-cpp/multiplayer.h"
#endif // !XSAPI_NO_PPL

namespace xbox { namespace services { namespace system {

/// <summary>List of gaming privilege that a user can have.</summary>
enum class gaming_privilege
{
    /// <summary>The user can broadcast live gameplay.</summary>
    broadcast = 190,

    /// <summary>The user can view other user's friends list if this privilege is present.</summary>
    view_friends_list = 197,

    /// <summary>The user can upload recorded in-game videos to the cloud if this privilege is present. Viewing GameDVRs is subject to privacy controls.</summary>
    game_dvr = 198,

    /// <summary>Kinect recorded content can be uploaded to the cloud for the user and made accessible to anyone if this privilege is present. Viewing other user's Kinect content is subject to a privacy setting.</summary>
    share_kinect_content = 199,

    /// <summary>The user can join a party session if this privilege is present</summary>
    multiplayer_parties = 203,

    /// <summary>The user can participate in voice chat during parties and multiplayer game sessions if this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    communication_voice_ingame = 205,

    /// <summary>The user can use voice communication with Skype on Xbox One if this privilege is present</summary>
    communication_voice_skype = 206,

    /// <summary>The user can allocate a cloud compute cluster and manage a cloud compute cluster for a hosted game session if this privilege is present</summary>
    cloud_gaming_manage_session = 207,

    /// <summary>The user can join a cloud compute session if this privilege is present</summary>
    cloud_gaming_join_session = 208,

    /// <summary>The user can save games in cloud title storage if this privilege is present</summary>
    cloud_saved_games = 209,

    /// <summary>The user can share content with others if this privilege is present</summary>
    share_content = 211,

    /// <summary>The user can purchase, download and launch premium content available with the Xbox LIVE Gold subscription if this privilege is present</summary>
    premium_content = 214,

    /// <summary>The user can purchase and download premium subscription content and use premium subscription features when this privilege is present</summary>
    subscription_content = 219,

    /// <summary>The user is allowed to share progress information on social networks when this privilege is present</summary>
    social_network_sharing = 220,

    /// <summary>The user can access premium video services if this privilege is present</summary>
    premium_video = 224,

    /// <summary>The user can use video communication with Skype or other providers when this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    video_communications = 235,

    /// <summary>The user is authorized to purchase content when this privilege is present</summary>
    purchase_content = 245,

    /// <summary>The user is authorized to download and view online user created content when this privilege is present.</summary>
    user_created_content = 247,

    /// <summary>The user is authorized to view other user's profiles when this privilege is present. Viewing other user's profiles is subject to additional privacy checks</summary>
    profile_viewing = 249,

    /// <summary>The user can use asynchronous text messaging with anyone when this privilege is present. Extra privacy permissions checks are required to determine who the user is authorized to communicate with. Communicating with other users is subject to additional privacy permission checks</summary>
    communications = 252,

    /// <summary>The user can join a multiplayer sessions for a game when this privilege is present.</summary>
    multiplayer_sessions = 254,

    /// <summary>The user can follow other Xbox LIVE users and add Xbox LIVE friends when this privilege is present.</summary>
    add_friend = 255
};

#if !XSAPI_NO_PPL
/// <summary>This class contains functions used for displaying stock UI during a game such as showing a people picker.</summary>
class title_callable_ui
{
public:
#if HC_PLATFORM != HC_PLATFORM_UWP
    /// <summary>
    /// Shows a picker UI that allows a person playing the game to select players
    /// from a presented list of other people.
    /// After the operation is complete, the list of selected Xbox User IDs is returned to the calling app.
    /// </summary>
    /// <param name="promptDisplayText">The prompt display text.</param>
    /// <param name="xboxUserIds">A list of Xbox User IDs which the user can select from.</param>
    /// <param name="preselectedXboxUserIds">A list of Xbox User IDs which will be pre-selected.</param>
    /// <param name="minSelectionCount">The minimum number of people the user must select.</param>
    /// <param name="maxSelectionCount">The maximum number of people the user can select.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.payload() contains the list of users that were selected by the player.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<std::vector<string_t>>>
    show_player_picker_ui(
        _In_ const string_t& promptDisplayText,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const std::vector<string_t>& preselectedXboxUserIds,
        _In_ uint32_t minSelectionCount,
        _In_ uint32_t maxSelectionCount
        );

    /// <summary>
    /// Shows a picker UI populated from the selected user's friend list and suggested friend list.
    /// After selection, the user can send an invitation to play a game and/or party chat for a
    /// specified game session to the selected people.
    /// </summary>
    /// <param name="sessionReference">A reference to the multiplayer session to invite people to.</param>
    /// <param name="invitationDisplayText">The ID of the custom invite string that is displayed with 
    /// the invite notification.The ID must match the ID that is assigned to the custom invite string 
    /// in the title's multiplayer service configuration. The format of the parameter is "///{id}", 
    /// where {id} is replaced with the ID of the custom string. For example, if the ID of the custom string 
    /// "Play Capture the Flag" is 1, then you would set this parameter to "///1" in order to display the 
    /// "Play Capture the Flag" custom string in the game invite. 
    /// Pass an empty string if you don't want a custom string added to the invite.</param>
    /// <param name="contextStringId">The custom activation context that is available to the invitee in the 
    /// activation URI for an invite. The custom activation context string must be URL-safe and binary content 
    /// should be encoded with URL-safe base64 encoding. The maximum length is 160 characters.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_game_invite_ui(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference,
        _In_ const string_t& invitationDisplayText,
        _In_ const string_t& contextStringId = string_t()
        );

    /// <summary>
    /// Shows UI displaying the profile card for a specified user.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_profile_card_ui(
        _In_ const string_t& targetXboxUserId
#if !HC_PLATFORM_IS_MICROSOFT
        , _In_ xbox_live_user_t user
#endif
        );

    /// <summary>
    /// Shows UI for adding or removing a specified person to or from the requesting user's friend list.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_change_friend_relationship_ui(
        _In_ const string_t& targetXboxUserId
        );

    /// <summary>
    /// Shows UI presenting the requesting user's achievements for the specified title.
    /// </summary>
    /// <param name="titleId">The Xbox titleId to show information about.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_title_achievements_ui(
        _In_ uint32_t titleId
        );
#endif

#if defined(_APISET_TARGET_VERSION_WIN10_RS3)
    /// <summary>
    /// Shows UI displaying the friend finder app, so the user can get more friends
    /// </summary>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The task completes when the UI is closed.
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_friend_finder_ui(
    );

    /// <summary>
    /// Invokes the Xbox App to show full user profile for the target user
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox target xuid to show the profile for.</param>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_user_profile_ui(_In_ const string_t& targetXboxUserId);

    /// <summary>
    /// Shows UI displaying the title app for the calling application.
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_title_hub_ui(
#if HC_PLATFORM == HC_PLATFORM_UWP
        _In_opt_ Windows::System::User^ user = nullptr
#endif
    );

    /// <summary>
    /// Shows UI displaying the user settings
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_user_settings_ui(
    );

    /// <summary>
    /// Shows UI displaying a dialog to customize the user's profile
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
        show_customize_user_profile_ui(
        );
#elif !HC_PLATFORM_IS_MICROSOFT

    /// <summary>
    /// Invokes the Xbox App to show full user profile for the target user
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox target xuid to show the profile for.</param>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_user_profile_ui(_In_ const string_t& targetXboxUserId);

    /// <summary>
    /// Invokes the Xbox App to show the title app for the calling application.
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_title_hub_ui();

    /// <summary>
    /// Invokes the Xbox App to show the user settings
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_user_settings_ui();
#endif

#if !HC_PLATFORM_IS_MICROSOFT
    /// <summary>
    /// Invokes the Xbox App to show add friends functionality.
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static pplx::task<xbox::services::xbox_live_result<void>>
    show_add_friends_ui();
#endif

private:
};
#endif // !XSAPI_NO_PPL

}}}


