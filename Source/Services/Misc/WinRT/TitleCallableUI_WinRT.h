// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/system.h"
#include "xsapi/title_callable_ui.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>List of gaming privilege that a user can have.</summary>
public enum class GamingPrivilege sealed
{
    /// <summary>The user can broadcast live gameplay.</summary>
    Broadcast = xbox::services::system::gaming_privilege::broadcast,

    /// <summary>The user can view other user's friends list if this privilege is present.</summary>
    ViewFriendsList = xbox::services::system::gaming_privilege::view_friends_list,

    /// <summary>The user can upload recorded in-game videos to the cloud if this privilege is present. Viewing GameDVRs is subject to privacy controls.</summary>
    GameDVR = xbox::services::system::gaming_privilege::game_dvr,

    /// <summary>Kinect recorded content can be uploaded to the cloud for the user and made accessible to anyone if this privilege is present. Viewing other user's Kinect content is subject to a privacy setting.</summary>
    ShareKinectContent = xbox::services::system::gaming_privilege::share_kinect_content,

    /// <summary>The user can join a party session if this privilege is present</summary>
    MultiplayerParties = xbox::services::system::gaming_privilege::multiplayer_parties,

    /// <summary>The user can participate in voice chat during parties and multiplayer game sessions if this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    CommunicationVoiceIngame = xbox::services::system::gaming_privilege::communication_voice_ingame,

    /// <summary>The user can use voice communication with Skype on Xbox One if this privilege is present</summary>
    CommunicationVoiceSkype = xbox::services::system::gaming_privilege::communication_voice_skype,

    /// <summary>The user can allocate a cloud compute cluster and manage a cloud compute cluster for a hosted game session if this privilege is present</summary>
    CloudGamingManageSession = xbox::services::system::gaming_privilege::cloud_gaming_manage_session,

    /// <summary>The user can join a cloud compute session if this privilege is present</summary>
    CloudGamingJoinSession = xbox::services::system::gaming_privilege::cloud_gaming_join_session,

    /// <summary>The user can save games in cloud title storage if this privilege is present</summary>
    CloudSavedGames = xbox::services::system::gaming_privilege::cloud_saved_games,

    /// <summary>The user can share content with others if this privilege is present</summary>
    ShareContent = xbox::services::system::gaming_privilege::share_content,

    /// <summary>The user can purchase, download and launch premium content available with the Xbox LIVE Gold subscription if this privilege is present</summary>
    PremiumContent = xbox::services::system::gaming_privilege::premium_content,

    /// <summary>The user can purchase and download premium subscription content and use premium subscription features when this privilege is present</summary>
    SubscriptionContent = xbox::services::system::gaming_privilege::subscription_content,

    /// <summary>The user is allowed to share progress information on social networks when this privilege is present</summary>
    SocialNetworkSharing = xbox::services::system::gaming_privilege::social_network_sharing,

    /// <summary>The user can access premium video services if this privilege is present</summary>
    PremiumVideo = xbox::services::system::gaming_privilege::premium_video,

    /// <summary>The user can use video communication with Skype or other providers when this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    VideoCommunications = xbox::services::system::gaming_privilege::video_communications,

    /// <summary>The user is authorized to purchase content when this privilege is present</summary>
    PurchaseContent = xbox::services::system::gaming_privilege::purchase_content,

    /// <summary>The user is authorized to download and view online user created content when this privilege is present.</summary>
    UserCreatedContent = xbox::services::system::gaming_privilege::user_created_content,

    /// <summary>The user is authorized to view other user's profiles when this privilege is present. Viewing other user's profiles is subject to additional privacy checks</summary>
    ProfileViewing = xbox::services::system::gaming_privilege::profile_viewing,

    /// <summary>The user can use asynchronous text messaging with anyone when this privilege is present. Extra privacy permissions checks are required to determine who the user is authorized to communicate with. Communicating with other users is subject to additional privacy permission checks</summary>
    Communications = xbox::services::system::gaming_privilege::communications,

    /// <summary>The user can join a multiplayer sessions for a game when this privilege is present.</summary>
    MultiplayerSessions = xbox::services::system::gaming_privilege::multiplayer_sessions,

    /// <summary>The user can follow other Xbox LIVE users and add Xbox LIVE friends when this privilege is present.</summary>
    AddFriend = xbox::services::system::gaming_privilege::add_friend
};

/// <summary>
/// Represents a class that contains static functions used for displaying stock UI during a game such as showing a people picker.
/// </summary>
public ref class TitleCallableUI sealed
{
public:

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
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// The result contains the list of users that were selected by the player.
    /// </returns>
    static Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^
    ShowPlayerPickerUI(
        _In_ Platform::String^ promptDisplayText,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ preselectedXboxUserIds,
        _In_ uint32_t minSelectionCount,
        _In_ uint32_t maxSelectionCount
        );

    /// <summary>
    /// Shows a picker UI populated from the selected user's friend list and suggested friend list. 
    /// After selection, the user can send an invitation to play a game and/or party chat for a 
    /// specified game session to the selected people.
    /// </summary>
    /// <param name="sessionReference">A reference to the multiplayer session to invite people to.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined 
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard 
    /// invitation text. The ID string must be prefixed with "///".  Pass an empty string if 
    /// you don't want a custom string added to the invite.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowGameInviteUIAsync(
        _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
        _In_ Platform::String^ contextStringId
        );

    /// <summary>
    /// Shows UI displaying the profile card for a specified user.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowProfileCardUIAsync(
        _In_ Platform::String^ targetXboxUserId
        );

    /// <summary>
    /// Shows UI for adding or removing a specified person to or from the requesting user's friend list.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowChangeFriendRelationshipUIAsync(
        _In_ Platform::String^ targetXboxUserId
        );

    /// <summary>
    /// Shows UI presenting the requesting user's achievements for the specified title.
    /// </summary>
    /// <param name="titleId">The Xbox titleId to show information about.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowTitleAchievementsUIAsync(
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Checks if the current user has a specific privilege
    /// </summary>
    /// <param name="privilege">The privilege to check.</param>
    /// <returns>
    /// A boolean which is true if the current user has the privilege.
    /// </returns>
    static bool CheckGamingPrivilegeSilently(
        _In_ GamingPrivilege privilege
        );

    /// <summary>
    /// Checks if the current user has a specific privilege and if it doesn't, it shows UI 
    /// </summary>
    /// <param name="privilege">The privilege to check.</param>
    /// <param name="friendlyMessage">Text to display in addition to the stock text about the privilege</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// A boolean which is true if the current user has the privilege.
    /// </returns>
    static Windows::Foundation::IAsyncOperation<bool>^
    CheckGamingPrivilegeWithUI(
        _In_ GamingPrivilege privilege,
        _In_opt_ Platform::String^ friendlyMessage
        );

#if UWP_API
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
    /// <param name="user">System user that identifies the user to show the UI on behalf of</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// The result contains the list of users that were selected by the player.
    /// </returns>
    static Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^
    ShowPlayerPickerUIForUser(
        _In_ Platform::String^ promptDisplayText,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ preselectedXboxUserIds,
        _In_ uint32_t minSelectionCount,
        _In_ uint32_t maxSelectionCount,
        _In_ Windows::System::User^ user
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
    /// <param name="user">System user that identifies which user is sending the invite</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowGameInviteUIForUserAsync(
        _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
        _In_ Platform::String^ invitationDisplayText,
        _In_ Platform::String^ contextStringId,
        _In_ Windows::System::User^ user
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
    /// <param name="user">System user that identifies which user is sending the invite</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowGameInviteUIForUserAsync(
        _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
        _In_ Platform::String^ invitationDisplayText,
        _In_ Windows::System::User^ user
    );

    /// <summary>
    /// Shows UI displaying the profile card for a specified user.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <param name="user">System user that identifies the user to show the UI on behalf of</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowProfileCardUIForUserAsync(
        _In_ Platform::String^ targetXboxUserId,
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Shows UI for adding or removing a specified person to or from the requesting user's friend list.
    /// </summary>
    /// <param name="targetXboxUserId">The Xbox User ID to show information about.</param>
    /// <param name="user">System user that identifies the user to show the UI on behalf of</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowChangeFriendRelationshipUIForUserAsync(
        _In_ Platform::String^ targetXboxUserId,
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Shows UI presenting the requesting user's achievements for the specified title.
    /// </summary>
    /// <param name="titleId">The Xbox titleId to show information about.</param>
    /// <param name="user">System user that identifies the user to show the UI on behalf of</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^ ShowTitleAchievementsUIForUserAsync(
        _In_ uint32_t titleId,
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Checks if the current user has a specific privilege
    /// </summary>
    /// <param name="privilege">The privilege to check.</param>
    /// <param name="user">System user that identifies the user to check the privilege on behalf of</param>
    /// <returns>
    /// A boolean which is true if the current user has the privilege.
    /// </returns>
    static bool CheckGamingPrivilegeSilentlyForUser(
        _In_ GamingPrivilege privilege,
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Checks if the current user has a specific privilege and if it doesn't, it shows UI 
    /// </summary>
    /// <param name="privilege">The privilege to check.</param>
    /// <param name="friendlyMessage">Text to display in addition to the stock text about the privilege</param>
    /// <param name="user">System user that identifies the user to show the UI on behalf of</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// A boolean which is true if the current user has the privilege.
    /// </returns>
    static Windows::Foundation::IAsyncOperation<bool>^
    CheckGamingPrivilegeWithUIForUser(
        _In_ GamingPrivilege privilege,
        _In_opt_ Platform::String^ friendlyMessage,
        _In_ Windows::System::User^ user
        );

#if defined(_APISET_TARGET_VERSION_WIN10_RS3)
    /// <summary>
    /// Shows UI displaying the friend finder app, so the user can get more friends
    /// </summary>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^
    ShowFriendFinderForUser(
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Shows UI displaying the title app for the calling application.
    /// </summary>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^
    ShowTitleHubForUser(
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Shows UI displaying the user settings
    /// </summary>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The operation completes when the UI is closed.
    /// </returns>
    static Windows::Foundation::IAsyncAction^
    ShowUserSettingsForUser(
        _In_ Windows::System::User^ user
        );

    /// <summary>
    /// Shows UI displaying a dialog to customize the user's profile
    /// </summary>
    /// <returns>
    /// result.err() contains the error based on what happened in the case of an error.
    /// </returns>
    static Windows::Foundation::IAsyncAction^
    ShowCustomizeUserProfileForUser(
        _In_opt_ Windows::System::User^ user
        );
#endif
#endif

internal:
    TitleCallableUI();
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
