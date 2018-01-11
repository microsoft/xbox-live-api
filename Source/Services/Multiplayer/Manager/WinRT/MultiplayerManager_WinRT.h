// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/multiplayer_manager.h"
#include "MultiplayerLobbySession_WinRT.h"
#include "MultiplayerGameSession_WinRT.h"
#include "MultiplayerEvent_WinRT.h"
#include "MultiplayerMember_WinRT.h"
#include "MatchStatus_WinRT.h"
#include "JoinInProgressState_WinRT.h"
#include "MultiplayerQualityOfServiceMeasurements_WinRT.h"

namespace Microsoft{ namespace Xbox{ namespace Services {
    namespace System {
        ref class XboxLiveUser;
    }
}}}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// This class contains APIs for matchmaking, player roster, and multiplayer session management.
/// </summary>
public ref class MultiplayerManager sealed
{
public:
    /// <summary>
    /// Gets the multiplayer_manager singleton instance.
    /// </summary>
    static property MultiplayerManager^ SingletonInstance { MultiplayerManager^ get(); }

    /// <summary>
    /// Initializes the object.
    /// </summary>
    /// <param name="lobbySessionTemplateName">The name of the template for the lobby session to be based on.</param>
    void Initialize(
        _In_ Platform::String^ lobbySessionTemplateName
        );

    /// <summary>
    /// Sends a matchmaking request to the server.  When a match is found, the manager will join
    /// the game and notify the title via the FindMatchCompleted event.
    /// </summary>
    /// <param name="hopperName">The name of the hopper.</param>
    /// <param name="attributes">The ticket attributes for the match. (Optional)</param>
    /// <param name="timeout">The maximum time to wait for members to join the match. (Optional)</param>
    void FindMatch(
        _In_ Platform::String^ hopperName,
        _In_opt_ Platform::String^ attributes,
        _In_opt_ Windows::Foundation::TimeSpan timeout
        );

    /// <summary>
    /// Cancels the match request on the server, if one exists.
    /// </summary>
    void CancelMatch();

    /// <summary>
    /// Joins an Xbox Live user to the lobby identified by a session handle id.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the lobby session you want to join.</param>
    /// <param name="user">The Xbox Live user joining the lobby.</param>
    /// <remarks>
    /// A handle is a service-side pointer to a game session.
    /// The handleId is a GUID identifier of the handle.  Callers will usually get the handleId from
    /// another member's MultiplayerActivityDetails. Optionally, if you haven't added the local users through
    /// lobbySession()::AddlocalUser(), you can pass in the list of users via the Joinlobby() API.
    /// The result is delivered via MultiplayerEvent callback of type JoinGameCompleted through DoWork().
    /// After joining, you can set the host via MultiplayerGameSession::SetSynchronizedHost() if one doesn't exist.
    /// </remarks>
    [Windows::Foundation::Metadata::DefaultOverloadAttribute]
    void JoinLobby(
        _In_ Platform::String^ handleId,
        _In_ XboxLiveUser_t user
        );

#if TV_API
    /// <summary>
    /// Joins a set of Xbox Live users to the lobby identified by a session handle id.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the game session you want to join.</param>
    /// <param name="users">List of XboxLiveContexts joining the lobby.</param>
    /// <remarks>
    /// A handle is a service-side pointer to a game session.
    /// The handleId is a GUID identifier of the handle.  Callers will usually get the handleId from
    /// another member's MultiplayerActivityDetails. Optionally, if you haven't added the local users through
    /// LobbySession()::AddLocalUser(), you can pass in the list of users via the JoinLobby() API.
    /// The result is delivered via MultiplayerEvent callback of type JoinGameCompleted through DoWork().
    /// After joining, you can set the host via MultiplayerGameSession::SetSynchronizedHost() if one doesn't exist.
    /// </remarks>
    void JoinLobby(
        _In_ Platform::String^ handleId,
        _In_ Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ users
        );

    /// <summary>
    /// Joins a set of Xbox Live users to a lobby via the specified IProtocolActivatedEventArgs.
    /// </summary>
    /// <param name="eventArgs">The IProtocolActivatedEventArgs when the title is protocol activated.</param>
    /// <param name="users">List of users joining the lobby.</param>
    /// <remarks>
    /// The IProtocolActivatedEventArgs provides arguments for protocol activation. If the user accepts an invite or
    /// joined a friends game via a shell UI, the title will get a protocol activation. The result is delivered via
    /// JoinLobbyCompletedEventArgs() callback through DoWork().
    ///
    /// Optionally, if you haven't added the local users through LobbySession()::AddLocalUser(), you can pass in
    /// the list of users via the JoinLobby() API. If the invited user is not added either via
    /// LobbySession()::AddLocalUser() or through JoinLobby(), then JoinLobby() will fail and provide the
    /// InvitedXboxUserId that the invite was sent for as part of the JoinLobbyCompletedEventArgs()
    /// After joining, you can set the host via MultiplayerGameSession::SetSynchronizedHost() if one doesn't exist.
    /// </remarks>
    void JoinLobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ users
        );

    /// <summary>
    /// Send invites to your party to join your game.
    /// </summary>
    void InvitePartyToGame();
#endif

    /// <summary>
    /// Joins an Xbox Live user to a lobby via the specified IProtocolActivatedEventArgs.
    /// </summary>
    /// <param name="eventArgs">The IProtocolActivatedEventArgs when the title is protocol activated.</param>
    /// <param name="user">The system User joining the lobby.</param>
    /// <remarks>
    /// The IProtocolActivatedEventArgs provides arguments for protocol activation. If the user accepts an invite or
    /// joined a friends game via a shell UI, the title will get a protocol activation. The result is delivered via
    /// JoinLobbyCompletedEventArgs() callback through DoWork().
    ///
    /// Optionally, if you haven't added the local users through LobbySession()::AddLocalUser(), you can pass in
    /// the list of users via the JoinLobby() API. If the invited user is not added either via
    /// LobbySession()::AddLocalUser() or through JoinLobby(), then JoinLobby() will fail and provide the
    /// InvitedXboxUserId that the invite was sent for as part of the JoinLobbyCompletedEventArgs()
    /// After joining, you can set the host via GameSession::SetSynchronizedHost() if one doesn't exist.
    /// </remarks>
    void JoinLobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ XboxLiveUser_t user
        );

    /// <summary>
    /// Join the lobby's game session if one exists and if there is room. If the session doesn't exist, it creates a new game session
    /// with the existing lobby members.
    /// </summary>
    /// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
    /// <remarks>
    /// The result is delivered via MultiplayerEvent callback of type JoinGameCompleted through DoWork().
    /// This does not migrate existing lobby session properties over to the game session.
    /// After joining, you can set the properties or the host via GameSession()::SetSynchronized APIs.
    /// </remarks>
    void JoinGameFromLobby(
        _In_ Platform::String^ sessionTemplateName
        );

    /// <summary>
    /// Joins a game given a globally unique session name. Callers can get the unique session name
    /// as a result of the title's third party matchmaking.
    /// </summary>
    /// <param name="sessionName">A unique name for the session.</param>
    /// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
    /// <remarks>
    /// The result is delivered via MultiplayerEvent callback of type JoinGameCompleted through DoWork().
    /// After joining, you can set the properties or the host via GameSession()::SetSynchronized APIs.
    /// </remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call JoinGame(String^, String^, IVectorView<>^) instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    void JoinGame(
        _In_ Platform::String^ sessionName,
        _In_ Platform::String^ sessionTemplateName
        );

    /// <summary>
    /// Joins a set of Xbox Live users to the specified game session, using the specified session template. Callers can get the unique session name
    /// as a result of the title's third party matchmaking.
    /// </summary>
    /// <param name="sessionName">A unique name for the session.</param>
    /// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs you want to be part of the game.</param>
    /// <remarks>
    /// The result is delivered via MultiplayerEvent callback of type JoinGameCompleted through DoWork().
    /// After joining, you can set the properties or the host via GameSession()::SetSynchronized APIs.
    /// </remarks>
    void JoinGame(
        _In_ Platform::String^ sessionName,
        _In_ Platform::String^ sessionTemplateName,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
        );

    /// <summary>
    /// Leaves the current game session and returns the users back into the lobby. This removes all local users from the game.
    /// </summary>
    void LeaveGame();

    /// <summary>
    /// Ensures proper game state updates are maintained between the title and the Xbox Live Multiplayer Service.
    /// </summary>
    /// <returns>A list of all callback events for the game to handle. Empty if no events are triggered during this update.</returns>
    /// <remarks>
    /// To ensure the best performance, DoWork() must be called frequently, at least once per frame.
    /// The title needs to be thread safe when calling DoWork() since this is when the state will change.
    /// For example, if you looping through the list of game session members, the list may change when DoWork() is called.
    /// </remarks>
    Windows::Foundation::Collections::IVectorView<MultiplayerEvent^>^ DoWork();

    /// <summary>
    /// Provides the current state of matchmaking. 'None' if no matchmaking is in progress.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::Manager::MatchStatus MatchStatus
    {
        Microsoft::Xbox::Services::Multiplayer::Manager::MatchStatus get();
    }

    /// <summary>
    /// Estimated wait time for a match request to be matched with other members.
    /// Only applies after find_match() has been called.
    /// </summary>
    property Windows::Foundation::TimeSpan EstimatedMatchWaitTime { Windows::Foundation::TimeSpan get(); }

    /// <summary>
    /// A collection of members that you have invited to your lobby. When a member accepts a game invite,
    /// they will be added to the lobby and the game session. Users found via matchmaking will not be
    /// added in the lobby.
    /// </summary>
    property MultiplayerLobbySession^ LobbySession { MultiplayerLobbySession^ get(); }

    /// <summary>
    /// A collection of members that are part of your game session. When a member accepts a game invite,
    /// they will be added to the lobby and the game session (if there is room).
    /// You can call leave_game() to leave the game.
    /// </summary>
    property MultiplayerGameSession^ GameSession { MultiplayerGameSession^ get(); }

    /// <summary>
    /// Indicates JoinInProgressState on who can join your game.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::Manager::Joinability Joinability
    {
        Microsoft::Xbox::Services::Multiplayer::Manager::Joinability get();
    }

    /// <summary>
    /// Restricts who can join the game. Defaults to "enabled", meaning only local users and users who are followed
    /// by an existing member of the lobby can join without an invite.
    /// </summary>
    /// <param name="state">Restriction on members to join the lobby.</param>
    /// <remarks>
    /// Changes are batched and written to the service on the next DoWork(). All session properties and members
    /// contain updated response returned from the server upon calling DoWork().
    /// </remarks>
    void SetJoinInProgress(
        _In_ Microsoft::Xbox::Services::Multiplayer::Manager::Joinability value
        );

    /// <summary>
    /// Indicates whether the game should auto fill open slots during gameplay.
    /// If set to true, it finds members via matchmaking to fill open slots during gameplay.
    /// This can be changed anytime.
    /// </summary>
    property bool AutoFillMembersDuringMatchmaking
    {
        bool get();
        void set(_In_ bool value);
    }

    /// <summary>
    /// Sets a collection of MultiplayerQualityOfServiceMeasurements between itself and a list of remote clients.
    /// This is only useful when the title is manually managing QoS.
    /// </summary>
    void SetQualityOfServiceMeasurements(
        _In_ Windows::Foundation::Collections::IVectorView<Microsoft::Xbox::Services::Multiplayer::MultiplayerQualityOfServiceMeasurements^>^ measurements
        );

internal:
    MultiplayerManager();

    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_manager> GetCppObj() const;

    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_manager> m_cppObj;

#if UNIT_TEST_SERVICES
    void _Shutdown();
#endif

private:
    void UpdateSessions();

    MultiplayerLobbySession^ m_multiplayerLobbySession;
    MultiplayerGameSession^ m_multiplayerGameSession;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
