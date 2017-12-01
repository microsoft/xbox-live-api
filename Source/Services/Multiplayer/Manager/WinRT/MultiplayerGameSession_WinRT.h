// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/multiplayer_manager.h"
#include "MultiplayerMember_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MultiplayerSessionRestriction_WinRT.h"
#include "MultiplayerSessionConstants_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Represents a multiplayer game. There are two game objects when using a multiplayer manager.
/// One represents the LobbySession() which is where friends you invite will join.
/// Another is the GameSession() which contains people that your lobby has been matched with.
/// </summary>
public ref class MultiplayerGameSession sealed
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
    /// A collection of members that are in the game. When a friend accepts a game invite,
    /// members will be added to the lobby and the game session members list.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ Members
    {
        Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ get();
    }

    /// <summary>
    /// Returns the host member for the game.
    /// There can be multiple hosts if there are multiple users on the same host device.
    /// This returns the user with the lowest index on the host device.
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
    /// Teams currently participating in this tournament game.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^ TournamentTeams
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^ get();
    }

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Tournament team results for the game.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ TournamentTeamResults
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ get();
    }

    /// <summary>
    /// Indicates if the Xbox User ID is the host.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user.</param>
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
        _In_ Platform::String^ valueJson,
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
    /// Sets the host for the game using MultiplayerSessionWriteMode::SynchronizedUpdate.
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

internal:
    MultiplayerGameSession(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> GetCppObj() const;

    void UpdateCppObj(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> cppObj
        );

private:
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> m_cppObj;
    Windows::Foundation::Collections::IVector<MultiplayerMember^>^ m_members;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ m_sessionReference;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionConstants^ m_sessionConstants;
    Platform::Collections::Map<Platform::String^, MultiplayerSessionReference^>^ m_teams;
    Platform::Collections::Map<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ m_tournamentTeamResults;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
