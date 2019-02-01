// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/multiplayer.h"
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionMemberStatus_WinRT.h"
#include "MultiplayerQualityOfServiceMeasurements_WinRT.h"
#include "NetworkAddressTranslationSetting_WinRT.h"
#include "MultiplayerMeasurementFailure_WinRT.h"
#include "TournamentRegistrationReason_WinRT.h"
#include "TournamentRegistrationState_WinRT.h"
#include "TournamentGameResultState_WinRT.h"
#include "TournamentGameResultSource_WinRT.h"
#include "TournamentArbitrationStatus_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

// Example snippet of response
//
//        "members": 
//        {
//            "1": 
//            {
//                "constants": 
//                {
//                      "system": 
//                      {
//                            "index": 0,
//                            "xuid": "12345678",
//                    
//                            // Run initialization for this user. Defaults to false.
//                            // Ignored if no "memberInitialization" section is set for the session.
//                            "initialize": true,
//    
//                            // When match adds a user to a session, it can provide some context around how and why they were matched into the session.
//                            "matchmakingResult": 
//                            {
//                                    // This is a copy of the user's serverMeasurements from the matchmaking session.
//                                    "serverMeasurements": 
//                                    {
//                                        "east.azure.com": 
//                                        {
//                                            "latency": 233  // Milliseconds
//                                        }
//                                    }
//                            }
//                      }
//                      "custom": 
//                      {
//                      }
//                }
//                "properties": 
//                {
//                      "system": 
//                      {
//                            // These flags control the member status and "activeTitle", and are mutually exclusive (it's an error to set both to true).
//                            // For each, false is the same as not present. The default status is "inactive", i.e. neither present.
//                            "ready": true,
//                            "active": false,
//
//                            "secureDeviceAddress": "ryY=",  // Base64 blob, or not present. Empty-string is the same as not present.
//
//                            // List of members in my group, by index.
//                            // If a "initializationGroup" list is set, the member's own index will always be added if it isn't already present.
//                            // During managed initialization, if any members in the list fail, this member will also fail.
//                            "initializationGroup": [ 5 ],
//
//                            // QoS measurements by secure device address. Like all fields, "measurements" must be updated as a whole. It should be set once when measurement is complete, not incrementally.
//                            // If a "measurements" object is set, it can't contain an entry for the member's own address.
//                            "measurements": 
//                            {
//                                "e6Kv5zY=": 
//                                {
//                                    "latency": 5953,  // Milliseconds
//                                    "bandwidthDown": 19342,  // Kilobits per second
//                                    "bandwidthUp": 944,  // Kilobits per second
//                                    "custom": { }
//                                }
//                            },
//
//                            // QoS measurements by game-server connection string. Like all fields, "serverMeasurements" must be updated as a whole, so it should be set once when measurement is complete.
//                            // If empty, it means that none of the measurements completed within the "serverMeasurementTimeout".
//                            "serverMeasurements": 
//                            {
//                                "east.azure.com": 
//                                {
//                                    "latency": 233  // Milliseconds
//                                }
//                            }
//                      }
//                      "custom": 
//                      {
//                      }
//                }
//
//                // Only if the member has accepted. Optional (not set if no gamertag claim was found).
//                "gamertag": "stacy",
//
//                // This is set when the member uploads a secure device address. It's a case-insensitive string that can be used for equality comparisons.
//                "deviceToken": "9f4032ba7",
//
//                // This is set to "open", "moderate", or "strict" when the member uploads a secure device address.
//                "nat": "strict",
//
//                // This value is removed once the user does their first PUT to the session.
//                "reserved": true,
//
//                // If the member is active, this is the title in which they are active, in decimal.
//                "activeTitleId": "8397267",
//
//                // The time the user joined the session. If "reserved" is true, the time the reservation was made.
//                "joinTime": "2009-06-15T13:45:30.0900000Z",
//
//                Present if this member is in the properties/system/turn array, otherwise not.
//                "turn": true,
//                
//                // Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
//                // In order of precedence, could be set to "timeout", "latency", "bandwidthDown", "bandwidthUp", or "group".
//                // The only possible value at the end of "joining" is "group". (On timeout from "joining", the reservation is removed.)
//                "failedMetric": "latency",
//
//                // If "memberInitialization" is set and the member was added with "initialize": true, this is set to the initialization episode that the member will participate in.
//                // Zero is a special value used for the members added to a new session at create time.
//                "initializationEpisode": 0,
//
//                // The next member's index, which is the same as 'next' below if there's no more.
//                "next": 4
//            },
//            "4": { "next": 5 /* etc */ }
//        },

/// <summary>
/// Represents a reference to a member in a multiplayer session.
/// </summary>
public ref class MultiplayerSessionMember sealed
{
public:
    /// <summary>
    /// The Id for this multiplayer session member.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(MemberId, member_id, uint32);

    /// <summary>
    /// The Id of this members' team in a tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(TeamId, team_id);

    /// <summary>
    /// The initial team suggestion from SmartMatch - use groups to change teams
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(InitialTeam, initial_team);

    /// <summary>
    /// The Xbox User ID of the member.  This value is only known if the member has accepted.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// A JSON string that specifies the custom player constants for the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MemberCustomConstantsJson, member_custom_constants_json);

    /// <summary>
    /// (Optional) The base64 encoded secure device address of the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(SecureDeviceAddressBase64, secure_device_base_address64);

    /// <summary>
    /// A collection of role types to role names for this member. (Optional)
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ Roles
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

    /// <summary>
    /// A JSON string that specifies the custom player properties for the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MemberCustomPropertiesJson, member_custom_properties_json);

    // Other properties

    /// <summary>
    /// (Optional) The Gamertag of the member.  This value is only known if the member has accepted.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(Gamertag, gamertag);

    /// <summary>
    /// The status of this member.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(Status, status, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionMemberStatus);

    /// <summary>
    /// A value that indicates if this member has an available turn in the session.  
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(IsTurnAvailable, is_turn_available, bool);

    /// <summary>
    /// A value that indicates if this multiplayer session member is the player whose Xbox Live context was used to
    /// create or retrieve the current MultiplayerSession object.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(IsCurrentUser, is_current_user, bool);

    /// <summary>
    /// A value that indicates if a QoS initialization stage has been requested for this user during managed initialization.
    /// Ignored if there is not a "memberInitialization" section for the session.
    /// Defaults to false.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(InitializeRequested, initialize_requested, bool);

    /// <summary>
    /// A JSON string id that defines items that provide some matchmaking context around how and why they were matched into the session.
    /// This is a copy of the user's QoS serverMeasurements from the matchmaking session.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MatchmakingResultServerMeasurementsJson, matchmaking_result_server_measurements_json);

    /// <summary>
    /// Identifier of a JSON string defining the QoS measurements for the multiplayer session member in a session.
    /// </summary>
    /// <remarks>
    /// Like all fields, "serverMeasurements" must be updated as a whole, so it should be set once when measurement is complete.
    /// If it is empty, it means that none of the measurements completed within the "serverMeasurementTimeout".
    /// </remarks>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MemberServerMeasurementsJson, member_server_measurements_json);

    /// <summary>
    /// A collection of members in any groups to which the session member belongs. 
    /// </summary>
    /// <remarks>
    /// If a "initializationGroup" list is set, the member's own index is always added if it isn't already present.
    /// During managed initialization, if any members in the list fail, this member will also fail.
    /// </remarks>
    property Windows::Foundation::Collections::IVector<MultiplayerSessionMember^>^ MembersInGroup { Windows::Foundation::Collections::IVector<MultiplayerSessionMember^>^ get(); }

    /// <summary>
    /// QoS measurements for the member's secure device address.
    /// </summary>
    /// <remarks>
    /// Like all fields, "measurements" must be updated as a whole. It should be set once when measurement is complete, not incrementally.
    /// If a "measurements" object is set, it can't contain an entry for the member's own address.
    /// </remarks>
    property Windows::Foundation::Collections::IVector<MultiplayerQualityOfServiceMeasurements^>^ MemberMeasurements { Windows::Foundation::Collections::IVector<MultiplayerQualityOfServiceMeasurements^>^ get(); }

    /// <summary>
    /// The case-insensitive device token string for the multiplayer session member, which can be used for equality comparisons. 
    /// The token is set when the member uploads a secure device address.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(DeviceToken, device_token);

    /// <summary>
    /// The device's NAT setting when the member uploads a secure device address.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(Nat, nat, Microsoft::Xbox::Services::Multiplayer::NetworkAddressTranslationSetting);

    /// <summary>
    /// The identifier of the title for the game being played in the session, if the member is active.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(ActiveTitleId, active_title_id, uint32);

    /// <summary>
    /// The number of the initialization episode that the member participates in during managed initialization.
    /// This value is only useful to read when the title is manually managing its own QoS.
    /// </summary>
    /// <remarks>
    /// Users join sessions in batches. Each batch of users is assigned to a new initialization episode.
    /// The initialization episode number indicates the set of users that require QoS.
    /// Initialization episode 0 indicates that the member is not taking part in managed initialization.
    /// Initialization episode 1 is a special value used for the members added to a new session at create time.
    ///
    /// If the "memberInitialization" section is set and the member was added with "initialize":true, 
    /// this is set to the initialization episode that the member will participate in; otherwise it is 0.
    /// </remarks>
    DEFINE_PTR_PROP_GET_OBJ(InitializationEpisode, initialization_episode, uint32);

    /// <summary>
    /// The time the user joined the session. If "reserved" is true, this is the time when the reservation was made.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(JoinTime, join_time);

    /// <summary>
    /// The cause of why the initialization failed for the member.
    /// If there is no failure, this value is MultiplayerMeasurementFailure::None.
    /// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(InitializationFailureCause, initialization_failure_cause, Microsoft::Xbox::Services::Multiplayer::MultiplayerMeasurementFailure);

    /// <summary>
    /// The tournament team session reference.
    /// </summary>
    property MultiplayerSessionReference^ TournamentTeamSessionRef { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// Gets or sets a string vector of group names for the current user indicating which groups that user
    /// was part of during a multiplayer session.
    /// </summary>
    property Windows::Foundation::Collections::IVector<Platform::String^>^ Groups
    {
        Windows::Foundation::Collections::IVector<Platform::String^>^ get();
        void set(_In_ Windows::Foundation::Collections::IVector<Platform::String^>^ value);
    }

    /// <summary>
    /// Gets a list of group names for the current user indicating which groups that user encountered during a multiplayer session.
    /// </summary>
    property Windows::Foundation::Collections::IVector<Platform::String^>^ Encounters
    {
        Windows::Foundation::Collections::IVector<Platform::String^>^ get();
        void set(_In_ Windows::Foundation::Collections::IVector<Platform::String^>^ value);
    }

    /// <summary>
    /// The arbitration results submitted by the member.
    /// </summary>
    property Windows::Foundation::Collections::IMap<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ Results
    {
        Windows::Foundation::Collections::IMap<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ get();
    }

    /// <summary>
    /// The current status/state of the results.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(ArbitrationStatus, arbitration_status, Microsoft::Xbox::Services::Tournaments::TournamentArbitrationStatus);

internal:
    MultiplayerSessionMember(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> GetCppObj() const;
private:
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_roles;
    Windows::Foundation::Collections::IVector<MultiplayerQualityOfServiceMeasurements^>^ m_memberMeasurements;
    Windows::Foundation::Collections::IMap<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ m_results;
    MultiplayerSessionReference^ m_tournamentTeamSessionRef;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END