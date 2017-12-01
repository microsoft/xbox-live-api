// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/multiplayer_manager.h"
#include "MultiplayerMember_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MultiplayerSessionRestriction_WinRT.h"
#include "MultiplayerSessionConstants_WinRT.h"
#include "MultiplayerActivityDetails_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

namespace Microsoft{ namespace Xbox{ namespace Services {
    namespace System {
        ref class XboxLiveUser;
    }
}}}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Represents a multiplayer lobby. This is also where you manage members that are local to this device.
/// There are two game objects when using a multiplayer manager.
/// One represents the LobbySession() which is where friends you invite will join.
/// Another is the GameSession() which contains people that your lobby has been matched with.
/// </summary>
public ref class MultiplayerLobbySession sealed
{
public:

    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(CorrelationId, correlation_id);

    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ SessionReference
    {
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ get();
    }

    /// <summary>
    /// A collection of members that are local to this device.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ LocalMembers
    {
        Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ get();
    }

    /// <summary>
    /// A collection of members that are in the lobby. When a friend accepts a game invite,
    /// members will be added to the lobby.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ Members
    {
        Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ get();
    }

    /// <summary>
    /// Returns the host member for the lobby.
    /// There can be multiple hosts if there are multiple users on the same host device.
    /// This returns the user with the lowest index for that host device.
    /// </summary>
    property MultiplayerMember^ Host { MultiplayerMember^ get(); }

    /// <summary>
    /// JSON string that specify the custom properties for the game.  These can be changed anytime.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(Properties, properties);

    /// <summary>
    /// A set of constants associated with this session. These can only be set through the session template.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionConstants^ SessionConstants
    {
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionConstants^ get();
    }

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// The known last team result of the tournament.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ LastTournamentTeamResult
    {
        Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ get();
    }

    /// <summary>
    /// Hosts a new lobby when the first user is added. Any additional users are added to the existing lobby
    // as secondary users. This API will also advertise the lobby for friends to join.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User.</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    ///
    /// You can send invites, set lobby properties, and access lobby members only once you've added the local user.
    /// While joining a lobby via an invite, or a HandleId, you can skip adding the local user to avoid creating a lobby and
    /// instead pass in the list of users in the JoinLobby() API.
    ///</remarks>
    void AddLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary>
    /// Removes the local user from the lobby and game session.
    /// After this method is called, if no local users are active, title will not be able to perform any further multiplayer operations.
    /// You can join another game or re-add the local user.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User.</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void RemoveLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary>
    /// Set a custom property on the local member to the specified JSON string.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User you want to set the property for.</param>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void SetLocalMemberProperties(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_opt_ Platform::String^ valueJson,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Delete a custom property on the local member.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User you want to delete the property for.</param>
    /// <param name="name">The name of the property to delete.</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void DeleteLocalMemberProperties(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Set the connection address for the local member. The address can be used for network and secure socket connections.
    /// </summary>
    /// <param name="user">The associated system User you want to set the property for.</param>
    /// <param name="connectionAddress">The network connection address to set.</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void SetLocalMemberConnectionAddress(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ connectionAddress,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Indicates if the Xbox User ID is the host.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user</param>
    bool IsHost(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Set a custom game property to the specified JSON string.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void SetProperties(
        _In_ Platform::String^ name,
        _In_opt_ Platform::String^ valueJson,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Sets a custom property to the specified JSON string using MultiplayerSessionWriteMode::SynchronizedUpdate.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Use this method to resolve any conflicts between devices while trying to set properties to a shared portion that other
    /// devices can also modify. It ensures that updates to the session are atomic. If writing to non-sharable properties, use SetProperties() instead.
    ///
    /// The service may reject your request if a race condition occurred (due to a conflict) resulting in ErrorCode
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    ///
    /// The result is delivered via MultiplayerEvent callback of type WriteSynchronizedPropertiesCompleted through DoWork().
    /// </remarks>
    void SetSynchronizedProperties(
        _In_ Platform::String^ name,
        _In_opt_ Platform::String^ valueJson,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Sets the host for the game using multiplayer_session_write_mode::synchronized_update. 
    /// </summary>
    /// <param name="gameHost">The host member.</param>
    /// <param name="context">The application-defined data to correlate the MultiplayerEvent to the initiating call. (Optional)</param>
    /// <remarks>
    /// Use this method to resolve any conflicts between devices trying to set the host at the same time.
    /// It ensures that updates to the session are atomic.
    ///
    /// The service may reject your request if a race condition occurred(due to a conflict) resulting in ErrorCode
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    ///
    /// The result is delivered via MultiplayerEvent callback of type WriteSynchronizedHostCompleted through DoWork().
    /// </remarks>
    void SetSynchronizedHost(
        _In_ MultiplayerMember^ gameHost,
        _In_opt_ context_t context
        );

    /// <summary>
    /// Displays the invite UI and allows the user to select people from the user's people list and invite them to join the user's party
    /// If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard
    /// invitation text. The ID string must be prefixed with "///".  Pass an empty string if
    /// you don't want a custom string added to the invite.</param>
    /// <param name="customActivationContext">The activation context string. Pass an empty string if
    /// you don't want a custom context added to the invite.</param>
    void InviteFriends(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ contextStringId,
        _In_ Platform::String^ customActivationContext
        );

    /// <summary>
    /// Invites the specified users to a game.  This will result in a notification being shown to
    /// each invited user using standard invite text.  If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="user">The associated XboxLiveContext for the User.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs who will be invited.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard
    /// invitation text. The ID string must be prefixed with "///".  Pass an empty string if
    /// you don't want a custom string added to the invite.</param>
    /// <param name="customActivationContext">The activation context string. Pass an empty string if
    /// you don't want a custom context added to the invite.</param>
    /// <returns>The async object for notifying when the operation is completed. </returns>
    void InviteUsers(
        _In_ XboxLiveUser_t user,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ Platform::String^ contextStringId,
        _In_ Platform::String^ customActivationContext
        );

internal:
    MultiplayerLobbySession(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> GetCppObj() const;

    void UpdateCppObj(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> cppObj
        );

private:
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> m_cppObj;
    Windows::Foundation::Collections::IVector<MultiplayerMember^>^ m_lobbyMembers;
    Windows::Foundation::Collections::IVector<MultiplayerMember^>^ m_localMembers;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ m_sessionReference;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionConstants^ m_sessionConstants;
    Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ m_lastTournamentTeamResult;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
