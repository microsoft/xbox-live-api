// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <fstream>
#define TEST_CLASS_OWNER L"adityat"
#define TEST_CLASS_AREA L"Multiplayer"
#include "UnitTestIncludes.h"
#include "xsapi/game_server_platform.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"
#include "xsapi/xbox_live_context.h"
#include "XboxLiveContext_WinRT.h"
#include "utils.h"
#include "Utils_WinRT.h"
#include "MultiplayerSessionWriteMode_WinRT.h"
#include "RtaTestHelper.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Microsoft::Xbox::Services::Tournaments;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const uint32 DefaultLargeSessionMemberIndex = 1958;
const uint32 DefaultLargeSessionNextMemberIndex = 7240;
const uint32 DefaultLargeSessionMemberCount = 3400;
const uint32 DefaultMaxMembersInSession = 50;
const uint32 MaxMembersInLargeSession = 10000;

struct MultiplayerSessionTestCreateInput
{
    MultiplayerSessionReference^ multiplayerSessionReference;
    uint32 maxMembersInSession;
    bool clientMatchmakingCapable;
    bool isLargeSession;
    MultiplayerSessionVisibility multiplayerSessionVisibility;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds;
    Platform::String^ sessionCustomConstantsJson;

    // SetTimeouts
    Windows::Foundation::TimeSpan MemberReservedTimeout;
    Windows::Foundation::TimeSpan MemberInactiveTimeout;
    Windows::Foundation::TimeSpan MemberReadyTimeout;
    Windows::Foundation::TimeSpan sessionEmpty;

    // SetArbitrationTimeouts
    Windows::Foundation::TimeSpan ArbitrationTimeout;
    Windows::Foundation::TimeSpan ForfeitTimeout;

    // SetQualityOfServiceConnectivityMetrics
    bool EnableLatencyMetric;
    bool EnableBandwidthDownMetric;
    bool EnableBandwidthUpMetric;
    bool EnableCustomMetric;

    // SetManagedInitialization
    Windows::Foundation::TimeSpan JoinTimeout;
    Windows::Foundation::TimeSpan MeasurementTimeout;
    Windows::Foundation::TimeSpan EvaluationTimeout;
    bool AutoEvalute;
    bool ExternalEvaluation;
    uint32 MembersNeededToStart;

    // SetPeerToPeerRequirements
    Windows::Foundation::TimeSpan PeerToPeerRequirementsLatencyMaximum;
    uint32 PeerToPeerRequirementsBandwidthMinimumInKilobitsPerSecond;

    // SetPeerToHostRequirements
    Windows::Foundation::TimeSpan PeerToHostRequirementsLatencyMaximum;
    uint32 PeerToHostRequirementsBandwidthDownMinimumInKilobitsPerSecond;
    uint32 PeerToHostRequirementsBandwidthUpMinimumInKilobitsPerSecond;
    MultiplayMetrics PeerToHostRequirementsHostSelectionMetric;

    // SetMeasurementServerAddresses
    Windows::Foundation::Collections::IVectorView<Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^>^ MeasurementServerAddresses;

    // SetInitializationStatus
    bool InitializationSucceeded;

    // SetHostDeviceToken
    Platform::String^ HostDeviceToken;

    // SetMatchmakingServerConnectionPath
    Platform::String^ MatchmakingServerConnectionPath;

    // SetMatchmakingResubmit
    bool MatchmakingMatchResubmit;

    // ServerConnectionStringCandidates
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ ServerConnectionStringCandidates;

    // SetCurrentUserQualityOfServiceMeasurements
    Windows::Foundation::Collections::IVectorView<MultiplayerQualityOfServiceMeasurements^>^ Measurements;

    // SetCurrentUserQualityOfServiceServerMeasurementsJson
    Platform::String^ ServerMeasurementsJson;
};

struct MultiplayerSessionResponseMemberTestData
{
    Platform::String^ memberId;
    uint32 constants_system_index;
    Platform::String^ constants_system_xuid;
    Platform::String^ constants_custom;
    bool properties_system_active;
    Platform::String^ properties_system_secureDeviceAddress;
    unsigned int properties_system_group1;
    unsigned int properties_system_group2;
    Platform::String^ properties_custom;
    Platform::String^ gamertag;
    bool properties_system_reserved;
    bool turn;
    Platform::String^ next;

    // 105 contract

    uint32 activeTitleId;
    Windows::Foundation::DateTime joinTime;
    Platform::String^ failedMetric;
    unsigned int initializationEpisode;
    bool constants_system_initialize;
    int constants_system_matchmakingResult_serverMeasurements_test1_latency;
    bool properties_system_ready;
    int properties_system_group_index1;
    bool properties_system_measurements_index3_local;
    int properties_system_measurements_index3_latency;
    int properties_system_measurements_index3_bandwidthDown;
    int properties_system_measurements_index3_bandwidthUp;
    Platform::String^ properties_system_measurements_index3_custom;
    int properties_system_serverMeasurements_test1_latency;
    NetworkAddressTranslationSetting nat;
    Platform::String^ deviceToken;
    Platform::String^ initializationFailure;
    int properties_system_measurements_test1_latency;
    int properties_system_measurements_test1_bandwidthDown;
    int properties_system_measurements_test1_bandwidthUp;
    Platform::String^ properties_system_measurements_test1_custom;
};

struct MultiplayerSessionResponseTestData
{
    int contractVersion;
    Platform::String^ multiplayerCorrelationId;

    int constants_system_version;
    unsigned int constants_system_maxMembersCount;
    bool constants_system_capabilities_clientMatchmaking;
    Platform::String^ constants_system_visibility;
    Platform::String^ constants_system_initiators_array1;
    Platform::String^ constants_system_initiators_array2;
    Platform::String^ constants_custom;

    Platform::String^ properties_system_keywords_array1;
    Platform::String^ properties_system_keywords_array2;
    Platform::String^ properties_system_host;
    Platform::String^ properties_system_join_restriction;
    unsigned int properties_system_turn_array1;
    unsigned int properties_system_turn_array2;
    Platform::String^ properties_system_matchmaking_clientResult_status;
    Platform::String^ properties_system_matchmaking_clientResult_statusDetails;
    int properties_system_matchmaking_clientResult_typicalWait;
    Platform::String^ properties_system_matchmaking_clientResult_targetSessionRef_scid;
    Platform::String^ properties_system_matchmaking_clientResult_targetSessionRef_templateName;
    Platform::String^ properties_system_matchmaking_clientResult_targetSessionRef_name;
    Platform::String^ properties_system_matchmaking_targetSessionConstants;
    Platform::String^ properties_system_matchmaking_serverConnectionString;
    Platform::String^ properties_custom;

    MultiplayerSessionResponseMemberTestData member1;
    MultiplayerSessionResponseMemberTestData member2;

    int membersInfo_first;
    int membersInfo_next;
    unsigned int membersInfo_count;
    unsigned int membersInfo_accepted;

    Platform::String^ servers;

    // 105 contract
    Windows::Foundation::DateTime startTime;
    Windows::Foundation::DateTime dateOfNextTimer;
    Platform::String^ initialization_stage;
    Windows::Foundation::DateTime initialization_stageStartTime;
    unsigned int initialization_episode;
    std::vector<std::wstring> hostCandidates;

    bool constants_system_capabilities_connectivity;
    bool constants_system_capabilities_suppressPresenceActivityCheck;
    bool constants_system_capabilities_gameplay;
    bool constants_system_capabilities_large;

    int constants_system_timeouts_reserved;
    int constants_system_timeouts_inactive;
    int constants_system_timeouts_ready;
    int constants_system_timeouts_sessionEmpty;
    bool constants_system_metrics_latency;
    bool constants_system_metrics_bandwidthDown;
    bool constants_system_metrics_bandwidthUp;
    bool constants_system_metrics_custom;
    int constants_system_managedInitialization_joinTimeout;
    int constants_system_managedInitialization_measurementTimeout;
    int constants_system_managedInitialization_evaluationTimeout;
    bool constants_system_managedInitialization_autoEvaluate;
    unsigned int constants_system_managedInitialization_membersNeededToStart;
    unsigned int constants_system_peerToPeerRequirements_latencyMaximum;
    unsigned int constants_system_peerToPeerRequirements_bandwidthMinimum;
    unsigned int constants_system_peerToHostRequirements_latencyMaximum;
    unsigned int constants_system_peerToHostRequirements_bandwidthDownMinimum;
    unsigned int constants_system_peerToHostRequirements_bandwidthUpMinimum;
    Platform::String^ constants_system_peerToHostRequirements_hostSelectionMetric;
    Platform::String^ constants_system_measurementServerAddresses_test1_secureDeviceAddress;
    Platform::String^ constants_system_measurementServerAddresses_test2_secureDeviceAddress;
    bool properties_system_initializationSucceeded;
    bool properties_system_matchmaking_resubmit;
    Platform::String^ properties_system_serverConnectionStringCandidates_1;
    Platform::String^ properties_system_serverConnectionStringCandidates_2;
};

Platform::String^ TestDataToJson(_In_ Platform::String^ str)
{
    Platform::String^ output = "{ \"foo\": \"";
    output += str;
    output += "\" }";
    return output;
}

MultiplayerSessionTestCreateInput GetDefaultMultiplayerSessionTestCreateInput(uint32 maxMembersInSession = 50)
{
    MultiplayerSessionTestCreateInput input;
    input.multiplayerSessionReference = ref new MultiplayerSessionReference(
        "361D0DAA-620E-4975-B64C-0C32500D41EF", // serviceConfigurationId
        "MySessionTemplate", // sessionTemplateName
        "32A53A76-9802-42C7-A28E-4FD483301D8B" // sessionName
        );
    input.maxMembersInSession = maxMembersInSession;
    input.clientMatchmakingCapable = true;

    if (maxMembersInSession > 100)
    {
        input.isLargeSession = true;
    }

    input.multiplayerSessionVisibility = MultiplayerSessionVisibility::Full;
    Vector<Platform::String^>^ initiatorIds = ref new Vector<Platform::String^>();
    initiatorIds->Append("12323");
    input.initiatorXboxUserIds = initiatorIds->GetView();
    input.sessionCustomConstantsJson = "test1010";

    // SetTimeouts
    input.MemberReservedTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001));
    input.MemberInactiveTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002));
    input.MemberReadyTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3003));
    input.sessionEmpty = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3004));

    // SetArbitrationTimeouts
    input.ArbitrationTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001));
    input.ForfeitTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002));

    // SetQualityOfServiceConnectivityMetrics
    input.EnableLatencyMetric = false;
    input.EnableBandwidthDownMetric = true;
    input.EnableBandwidthUpMetric = false;
    input.EnableCustomMetric = true;

    // SetManagedInitialization
    input.JoinTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(4001));
    input.MeasurementTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(4002));
    input.EvaluationTimeout = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(4003));
    input.AutoEvalute = false;
    input.ExternalEvaluation = true;
    input.MembersNeededToStart = 4004;

    // SetPeerToPeerRequirements
    input.PeerToPeerRequirementsLatencyMaximum = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::milliseconds(5001));
    input.PeerToPeerRequirementsBandwidthMinimumInKilobitsPerSecond = 5002;

    // SetPeerToHostRequirements
    input.PeerToHostRequirementsLatencyMaximum = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::milliseconds(6001));
    input.PeerToHostRequirementsBandwidthDownMinimumInKilobitsPerSecond = 6002;
    input.PeerToHostRequirementsBandwidthUpMinimumInKilobitsPerSecond = 6003;
    input.PeerToHostRequirementsHostSelectionMetric = MultiplayMetrics::Bandwidth;

    // SetMeasurementServerAddresses
    Platform::Collections::Vector<Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^>^ measurementServerAddresses =
        ref new Platform::Collections::Vector<Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^>();

    auto qosServerCpp = xbox::services::game_server_platform::quality_of_service_server(
        L"Test",
        L"test1_sda",
        L"TEST1_target"
        );
    Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^ server = ref new Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer(qosServerCpp);
    measurementServerAddresses->Append(server);

    qosServerCpp = xbox::services::game_server_platform::quality_of_service_server(
        L"Test",
        L"test2_sda",
        L"TEST2_target"
        );
    server = ref new Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer(qosServerCpp);
    measurementServerAddresses->Append(server);

    input.MeasurementServerAddresses = measurementServerAddresses->GetView();

    // SetInitializationStatus
    input.InitializationSucceeded = true;

    // SetHostDeviceToken
    input.HostDeviceToken = "7001";

    // SetMatchmakingServerConnectionPath
    input.MatchmakingServerConnectionPath = "8001";

    // SetMatchmakingResubmit
    input.MatchmakingMatchResubmit = true;

    // ServerConnectionStringCandidates
    Platform::Collections::Vector<Platform::String^>^ serverConnectionStringCandidates = ref new Platform::Collections::Vector<Platform::String^>();
    serverConnectionStringCandidates->Append("9001");
    serverConnectionStringCandidates->Append("9002");
    serverConnectionStringCandidates->Append("9003");
    input.ServerConnectionStringCandidates = serverConnectionStringCandidates->GetView();

    // SetCurrentUserQualityOfServiceMeasurements    
    Platform::Collections::Vector<MultiplayerQualityOfServiceMeasurements^>^ measurements = ref new Platform::Collections::Vector<MultiplayerQualityOfServiceMeasurements^>();
    //measurements->Append(). TODO: test
    input.Measurements = measurements->GetView();

    // SetCurrentUserQualityOfServiceServerMeasurementsJson
    input.ServerMeasurementsJson = "10001";

    return input;
}

MultiplayerSessionResponseTestData GetDefaultMultiplayerSessionResponseTestData(unsigned int numHostCandidates, bool isLargeSession)
{
    MultiplayerSessionResponseTestData data;

    //    {
    //        "contractVersion": 105,
    //        "correlationId": "0FE81338-EE96-46E3-A3B5-2DBBD6C41C3B",
    //        "startTime": "2009-06-15T13:45:30.0900000Z",
    //        "nextTimer": "2009-06-15T13:45:30.0900000Z",
    //        "initializing": 
    //        {
    //            "stage": "measuring",
    //            "stageStartTime": "2009-06-15T13:45:30.0900000Z",
    //            "episode": 0
    //        },
    //        "hostCandidates": [ "ab90a362", "99582e67" ],
    data.contractVersion = 105;
    data.multiplayerCorrelationId = "0FE81338-EE96-46E3-A3B5-2DBBD6C41C3B";
    data.startTime.UniversalTime = (int64)1233 * (int64)10000000;
    data.dateOfNextTimer.UniversalTime = (int64)1234 * (int64)10000000;
    data.initialization_stage = "measuring";
    data.initialization_stageStartTime.UniversalTime = (int64)1235 * (int64)10000000;
    data.initialization_episode = 102;

    for (unsigned int i = 0; i < numHostCandidates; i++)
    {
        std::wstring hostCandidate = L"host_";
        hostCandidate += i.ToString()->Data();
        data.hostCandidates.push_back(hostCandidate);
    }

    //    /constants/system 
    //    {
    //        "version": 1,
    //        "maxMembersCount": 100,  // Defaults to 100 if not set on creation. Must be between 1 and 100.
    //        "visibility": "private",  // Or "visible" or "open", defaults to "open" if not set on creation.
    //        "initiators": [ "1234" ],  // If specified on a new session, the creator's xuid must be in the list (or the creator must be a server).
    //        "inviteProtocol": "party",  // Optional URI scheme of the launch URI for invite toasts.
    //        "capabilities": 
    //        {
    //            "clientMatchmaking": true,
    //            "large" : false       // if true, connectivity must be false
    //            "connectivity": true  // false if for large session
    //        },
    data.constants_system_version = 1;

    if (isLargeSession)
    {
        data.constants_system_maxMembersCount = MaxMembersInLargeSession;
    }
    else
    {
        data.constants_system_maxMembersCount = 100;
    }
    data.constants_system_visibility = "private";
    data.constants_system_initiators_array1 = "1234";
    data.constants_system_initiators_array2 = "2345";
    data.constants_system_capabilities_clientMatchmaking = true;

    data.constants_system_capabilities_connectivity = !isLargeSession;  // large sessions must switch off the connectivity flag
    data.constants_system_capabilities_large = isLargeSession;


    //    /constants/system 
    //    {
    //        "reservedRemovalTimeout": 10000,  // Default is 10 seconds. Member is removed from the session.
    //        "inactiveRemovalTimeout": 7200000,  // Default is two hours. Member is removed from the session.
    //        "readyRemovalTimeout": 60000,  // Default is one minute. Member becomes inactive.
    //        "sessionEmptyTimeout": 0,  // Default is zero. Session is deleted.
    //
    //        "metrics": 
    //        {
    //            "latency": true,
    //            "bandwidthDown": true,
    //            "bandwidthUp": true,
    //            "custom": true
    //        },
    //        "memberInitialization": 
    //        {
    //            "joinTimeout": 4000,  // Milliseconds. Default is 4 seconds. Overrides "reservationTimeout" for the initial members.
    //            "measurementTimeout": 5000,  // Milliseconds. Default is based on which of the metrics and/or server ping addresses are set.
    //            "evaluationTimeout": 5000,  
    //            "externalEvaluation": true,
    //            "membersNeededToStart": 2  // Defaults to 2. Must be between 1 and maxMemberCount. Only applies to initialization episode zero.
    //        },
    data.constants_system_timeouts_reserved = 1001;
    data.constants_system_timeouts_inactive = 1002;
    data.constants_system_timeouts_ready = 1003;
    data.constants_system_timeouts_sessionEmpty = 1004;
    data.constants_system_metrics_latency = true;
    data.constants_system_metrics_bandwidthDown = true;
    data.constants_system_metrics_bandwidthUp = true;
    data.constants_system_metrics_custom = true;

    // managed initialization is only important for non large sessions... but since this is mock test data, it's okay to leave it in
    data.constants_system_managedInitialization_joinTimeout = 2001;
    data.constants_system_managedInitialization_measurementTimeout = 2002;
    data.constants_system_managedInitialization_evaluationTimeout = 2004;
    data.constants_system_managedInitialization_autoEvaluate = false;
    data.constants_system_managedInitialization_membersNeededToStart = 2003;

    //    /constants/system 
    //    {
    //        "peerToPeerRequirements": 
    //        {
    //            "latencyMaximum": 250,  // Milliseconds
    //            "bandwidthDownMinimum": 10000  // Kilobits per second
    //        },
    //        "peerToHostRequirements": 
    //        {
    //            "latencyMaximum": 250,  // Milliseconds
    //            "bandwidthDownMinimum": 100000,  // Kilobits per second
    //            "bandwidthUpMinimum": 1000,  // Kilobits per second
    //            "hostSelectionMetric": "bandwidthUp"  // Or "bandwidthDown" or "latency". Not specified is the same as "latency".
    //        },
    //        "measurementServerAddresses": 
    //        {
    //            "east.azure.com": 
    //            {
    //                "secureDeviceAddress": "r5Y="  // Base-64 encoded secure-device-address
    //            },
    //        }
    //    }
    data.constants_system_peerToPeerRequirements_latencyMaximum = 3001;
    data.constants_system_peerToPeerRequirements_bandwidthMinimum = 3002;
    data.constants_system_peerToHostRequirements_latencyMaximum = 3001;
    data.constants_system_peerToHostRequirements_bandwidthDownMinimum = 3002;
    data.constants_system_peerToHostRequirements_bandwidthUpMinimum = 3003;
    data.constants_system_peerToHostRequirements_hostSelectionMetric = "bandwidthUp";
    data.constants_system_measurementServerAddresses_test1_secureDeviceAddress = "test1_addr";
    data.constants_system_measurementServerAddresses_test2_secureDeviceAddress = "test2_addr";

    //    /properties/system 
    //    {
    //        "keywords": [ "hello" ],  // Optional array of case-insensitive strings. Cannot be set if the session's visibility is "private"
    //        "turn": [ 0 ],  // Array of integer member indicies whose turn it is. Defaults to empty.
    //        "joinRestriction": "local", //Restricts who can join "open" sessions. 
    //        "host": "99e4c701",
    //        "initializationSucceeded": true,
    //        "serverConnectionStringCandidates": [ "west.azure.com", "east.azure.com" ],
    //        "matchmaking": 
    //        {
    //            "clientResult": 
    //            {  
    //                // Requires the clientMatchmaking property.
    //                "status": "searching",  // Or "expired", "found", "failed", or "canceled". 
    //                "statusDetails": "Description",  // Default is empty string.
    //                "typicalWait": 30,  // The expected number of seconds waiting as a non-negative integer.
    //                "targetSessionRef": 
    //                {
    //                    "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
    //                    "templateName": "capture-the-flag",
    //                    "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
    //                }
    //            },
    //            "targetSessionConstants": { },
    //            "serverConnectionString": "west.azure.com"
    //        },
    //        "matchmakingResubmit": true
    //    }
    data.properties_system_keywords_array1 = "key1";
    data.properties_system_keywords_array2 = "key2";
    data.properties_system_join_restriction = "local";
    data.properties_system_host = "99e4c701";
    data.properties_system_turn_array1 = 3;
    data.properties_system_turn_array2 = 0;
    data.properties_system_initializationSucceeded = true;
    data.properties_system_matchmaking_clientResult_status = "Searching";
    data.properties_system_matchmaking_clientResult_statusDetails = "test1";
    data.properties_system_matchmaking_clientResult_typicalWait = 30;
    data.properties_system_matchmaking_clientResult_targetSessionRef_scid = "1ECFDB89-36EB-4E59-8901-11F7393689AE";
    data.properties_system_matchmaking_clientResult_targetSessionRef_templateName = "capture";
    data.properties_system_matchmaking_clientResult_targetSessionRef_name = "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23";
    data.properties_system_matchmaking_targetSessionConstants = "test1002";
    data.properties_system_matchmaking_serverConnectionString = "serverConnectionString_test1";
    data.properties_system_matchmaking_resubmit = true;
    data.properties_system_serverConnectionStringCandidates_1 = "serverConnectionString_test2";
    data.properties_system_serverConnectionStringCandidates_2 = "serverConnectionString_test3";
    data.properties_custom = "test1004";

    data.constants_custom = "test1001";

    if (isLargeSession)
    {
        //        "members": 
        //        {
        //            "me": 
        //            {
        //                "constants": { /* Property Bag */ },
        //                "properties": { /* Property Bag */ },
        //                "gamertag" : "gamerTag1",
        //                "deviceToken" : "9f4032ba7",
        //                "nat" : "strict",
        //                "reserved" : true,
        //                "activeTitleId" : "8397267",
        //                "joinTime" : "2009-06-15T13:45:30.0900000Z",
        //                "turn" : true,
        //                "initializationFailure" : "latency",
        //                "initializationEpisode" : 1,
        //                "next": 7448
        //            },        
        //        },
        data.member1.memberId = "me";
    }
    else
    {
        //        "members": 
        //        {
        //            "3": 
        //            {
        //                "constants": { /* Property Bag */ },
        //                "properties": { /* Property Bag */ },
        //                "gamertag" : "stacy",
        //                "deviceToken" : "9f4032ba7",
        //                "nat" : "strict",
        //                "reserved" : true,
        //                "activeTitleId" : "8397267",
        //                "joinTime" : "2009-06-15T13:45:30.0900000Z",
        //                "turn" : true,
        //                "initializationFailure" : "latency",
        //                "initializationEpisode" : 1,
        //                "next": 4
        //            },
        //            "4": { "next": 5 /* etc */ }
        //        },
        data.member1.memberId = "3";
    }

    data.member1.gamertag = "gamerTag1";
    data.member1.properties_system_reserved = true;
    data.member1.activeTitleId = 8397267;
    data.member1.joinTime.UniversalTime = (int64)1237 * (int64)10000000;
    data.member1.turn = true;
    data.member1.failedMetric = "latency";
    data.member1.initializationEpisode = 5003;
    data.member1.properties_system_measurements_index3_custom = "test1234";
    data.member1.nat = NetworkAddressTranslationSetting::Strict;
    data.member1.deviceToken = "test1_devtok";
    data.member1.initializationFailure = "latency";
    data.member1.next = nullptr;
    data.member2.memberId = nullptr; // skip member2

    //    /members/{index}/constants/system 
    //    {
    //        "index": 3
    //        "xuid": "12345678",
    //        "initialize": true,
    //        "matchmakingResult": 
    //        {
    //            "serverMeasurements": 
    //            {
    //                "east.azure.com": 
    //                {
    //                    "latency": 233  // Milliseconds
    //                }
    //            }
    //        }
    //    }

    data.member1.constants_system_index = isLargeSession ? DefaultLargeSessionMemberIndex : 3;
    data.member1.constants_system_xuid = (Platform::String^)"1245";
    data.member1.constants_system_initialize = true;
    data.member1.constants_system_matchmakingResult_serverMeasurements_test1_latency = 6001;
    data.member1.constants_custom = "test1005";

    //    /members/{index}/properties/system 
    //    {
    //        "ready": true,
    //        "active": false,
    //        "secureDeviceAddress": "ryY=",
    //        "initializationGroup": [ 5 ],
    //        "measurements": 
    //        {
    //            "5": 
    //            {
    //                "latency": 5953,  // Milliseconds
    //                "bandwidthDown": 19342,  // Kilobits per second
    //                "bandwidthUp": 944,  // Kilobits per second
    //                "custom": { }
    //            }
    //        },
    //        "serverMeasurements": 
    //        {
    //            "east.azure.com": 
    //            {
    //                "latency": 233  // Milliseconds
    //            }
    //        }
    //    }
    data.member1.properties_system_ready = true;
    data.member1.properties_system_active = true;
    data.member1.properties_system_secureDeviceAddress = "ryY=";
    data.member1.properties_system_group1 = 3;
    data.member1.properties_system_group2 = 3;
    data.member1.properties_system_group_index1 = 3;
    data.member1.properties_system_measurements_index3_local = true;
    data.member1.properties_system_measurements_index3_latency = 6001;
    data.member1.properties_system_measurements_index3_bandwidthDown = 6001;
    data.member1.properties_system_measurements_index3_bandwidthUp = 6002;
    data.member1.properties_system_measurements_index3_custom = "test7007";
    data.member1.properties_system_serverMeasurements_test1_latency = 233;
    data.member1.properties_custom = "test1006";
    data.member1.properties_system_measurements_test1_latency = 6003;
    data.member1.properties_system_measurements_test1_bandwidthDown = 6004;
    data.member1.properties_system_measurements_test1_bandwidthUp = 6005;
    data.member1.properties_system_measurements_test1_custom = "test1007";

    data.member1.gamertag = "gamerTag1";
    data.member1.properties_system_active = false;
    data.member1.properties_system_reserved = false;
    data.member1.properties_system_ready = true;
    data.member1.turn = true;
    data.member1.next = nullptr;

    //        "membersInfo": 
    //        {
    //            "first": 3,  // The first member's index.
    //            "next": 5,  // The index that the next member added will get.
    //            "count": 2,  // The number of members.
    //            "accepted": 1  // The number of members that are no longer 'reserved'.
    //        },

    if (isLargeSession)
    {
        data.membersInfo_first = DefaultLargeSessionMemberIndex;
        data.membersInfo_next = DefaultLargeSessionNextMemberIndex;
        data.membersInfo_count = DefaultLargeSessionMemberCount;
    }
    else
    {
        data.membersInfo_first = 3;
        data.membersInfo_next = 4;
        data.membersInfo_count = 1;
    }

    data.membersInfo_accepted = 1;

    //    /servers/{server-name}/constants/system 
    //    {
    //    }
    //    /servers/{server-name}/properties/system 
    //    {
    //        "lockId": "opaque56789",  // If set, a matchmaking service is servicing this session.
    //        "status": "searching",  // Or "expired", "found", "failed", or "canceled". Optional.
    //        "statusDetails": "Description",  // Optional free-form text. Default is empty string.
    //        "typicalWait": 30,  // Optional. The expected number of seconds waiting as a non-negative integer.
    //        "targetSessionRef": 
    //        {  
    //            // Optional.
    //            "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
    //            "templateName": "capture-the-flag",
    //            "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
    //        }
    //    }
    data.servers = "test1007";

    return data;
}

MultiplayerSession^ TestCreateSessionHelper(MultiplayerSessionTestCreateInput input, XboxLiveContext^ context)
{
    MultiplayerSession^ multiplayerSession = ref new MultiplayerSession(
        context,
        input.multiplayerSessionReference,
        input.maxMembersInSession,
        input.multiplayerSessionVisibility,
        input.initiatorXboxUserIds,
        input.sessionCustomConstantsJson->IsEmpty() ? nullptr : TestDataToJson(input.sessionCustomConstantsJson)
        );

    return multiplayerSession;
}

// TODO 781944: Add in additional phase 2 unit tests
DEFINE_TEST_CLASS(MultiplayerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MultiplayerTests)

    const string_t filePath = _T("\\TestResponses\\Multiplayer.json");
    web::json::value testResponseJsonFromFile = utils::read_test_response_file(filePath);
    const string_t rtaConnectionIdJson = testResponseJsonFromFile[L"rtaConnectionIdJson"].serialize();
    const string_t defaultMultiplayerResponse = testResponseJsonFromFile[L"defaultMultiplayerResponse"].serialize();

    void VerifyMultiplayerSessionReference(MultiplayerSessionReference^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL(result->ServiceConfigurationId->Data(), resultToVerify[L"scid"].as_string());
        VERIFY_ARE_EQUAL(result->SessionTemplateName->Data(), resultToVerify[L"templateName"].as_string());
        VERIFY_ARE_EQUAL(result->SessionName->Data(), resultToVerify[L"name"].as_string());
    }

    void VerifyMultiplayerPeerToHostRequirements(MultiplayerPeerToHostRequirements^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->LatencyMaximum,
            resultToVerify[_T("latencyMaximum")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_UINT(
            result->BandwidthDownMinimumInKilobitsPerSecond,
            resultToVerify[L"bandwidthDownMinimum"].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_UINT(
            result->BandwidthUpMinimumInKilobitsPerSecond,
            resultToVerify[L"bandwidthUpMinimum"].as_number().to_uint64()
            );
    }

    void VerifyMultiplayerPeerToPeerRequirements(MultiplayerPeerToPeerRequirements^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->LatencyMaximum,
            resultToVerify[_T("latencyMaximum")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_UINT(
            result->BandwidthMinimumInKilobitsPerSecond,
            resultToVerify[L"bandwidthMinimum"].as_number().to_uint64()
            );
    }

    void VerifyMultiplayerManagedInitialization(MultiplayerManagedInitialization^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->JoinTimeout,
            resultToVerify[_T("joinTimeout")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->MeasurementTimeout,
            resultToVerify[_T("measurementTimeout")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->EvaluationTimeout,
            resultToVerify[_T("evaluationTimeout")].as_number().to_uint64()
            );
        
        VERIFY_ARE_EQUAL(result->AutoEvaluate, !resultToVerify[L"externalEvaluation"].as_bool());
        VERIFY_ARE_EQUAL_INT(result->MembersNeededToStart, resultToVerify[L"membersNeededToStart"].as_integer());
    }

    void VerifyMultiplayerMemberInitialization(MultiplayerMemberInitialization^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->JoinTimeout,
            resultToVerify[_T("joinTimeout")].as_number().to_uint64()
        );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->MeasurementTimeout,
            resultToVerify[_T("measurementTimeout")].as_number().to_uint64()
        );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->EvaluationTimeout,
            resultToVerify[_T("evaluationTimeout")].as_number().to_uint64()
        );

        VERIFY_ARE_EQUAL(result->ExternalEvaluation, resultToVerify[L"externalEvaluation"].as_bool());
        VERIFY_ARE_EQUAL_INT(result->MembersNeededToStart, resultToVerify[L"membersNeededToStart"].as_integer());
    }

    void VerifyMultiplayerSessionProperties(MultiplayerSessionProperties^ result, web::json::value resultToVerify)
    {
        web::json::value propertiesSystemJson = resultToVerify[L"system"];
        web::json::value propertiesSystemMatchmakingJson = propertiesSystemJson[L"matchmaking"];
        web::json::value propertiesSystemMatchmakingClientResultJson = propertiesSystemMatchmakingJson[L"clientResult"];

        web::json::value closed = propertiesSystemJson[L"closed"];
        VERIFY_ARE_EQUAL(result->Closed, closed.as_bool());
        VERIFY_ARE_EQUAL(result->Locked, propertiesSystemJson[L"locked"].as_bool());
        VERIFY_ARE_EQUAL(result->AllocateCloudCompute, propertiesSystemJson[L"allocateCloudCompute"].as_bool());

        web::json::array keywords = propertiesSystemJson[L"keywords"].as_array();
        uint32 counter = 0;
        for (auto keyword : result->Keywords)
        {
            VERIFY_ARE_EQUAL(keyword->Data(), keywords[counter].as_string());
            ++counter;
        }

        VERIFY_ARE_EQUAL(result->JoinRestriction.ToString()->Data(), propertiesSystemJson[L"joinRestriction"].as_string());

        VERIFY_ARE_EQUAL(result->ReadRestriction.ToString()->Data(), propertiesSystemJson[L"readRestriction"].as_string());
    
        string_t resultCustomProperties = result->SessionCustomPropertiesJson->Data();
        string_t systemJsonCustomProperties = resultToVerify[L"custom"].serialize();
        VERIFY_ARE_EQUAL(resultCustomProperties, systemJsonCustomProperties);

        VERIFY_ARE_EQUAL(result->HostDeviceToken->Data(), propertiesSystemJson[L"host"].as_string());
        VERIFY_ARE_EQUAL(result->MatchmakingServerConnectionString->Data(), propertiesSystemMatchmakingJson[L"serverConnectionString"].as_string());
        
        web::json::array candidatesJson = propertiesSystemJson[L"serverConnectionStringCandidates"].as_array();
        counter = 0;
        for (auto candidate : result->ServerConnectionStringCandidates)
        {
            VERIFY_ARE_EQUAL(candidate->Data(), candidatesJson[counter].as_string());
            ++counter;
        }

    }

    void VerifyMultiplayerSessionMember(MultiplayerSessionMember^ result, web::json::value resultToVerify)
    {
        web::json::value constantsJson = resultToVerify[L"constants"];
        web::json::value constantsSystemJson = constantsJson[L"system"];
        web::json::value constantsSystemMatchmakingResultJson = constantsSystemJson[L"matchmakingResult"];
        web::json::value propertiesJson = resultToVerify[L"properties"];
        web::json::value rolesJson = resultToVerify[L"roles"];
        web::json::value propertiesSystemJson = propertiesJson[L"system"];
        web::json::value propertiesSystemSubscriptionJson = propertiesSystemJson[L"subscription"];

        switch (result->Status)
        {
            case MultiplayerSessionMemberStatus::Active:
                VERIFY_IS_TRUE(propertiesSystemJson[L"active"].as_bool());
                break;
            case MultiplayerSessionMemberStatus::Ready:
                VERIFY_IS_TRUE(propertiesSystemJson[L"ready"].as_bool());
                break;
            case MultiplayerSessionMemberStatus::Reserved:
                VERIFY_IS_TRUE(resultToVerify[L"reserved"].as_bool());
                break;
            case MultiplayerSessionMemberStatus::Inactive:
                VERIFY_IS_FALSE(resultToVerify[L"reserved"].as_bool());
                VERIFY_IS_FALSE(propertiesSystemJson[L"active"].as_bool());
                VERIFY_IS_FALSE(propertiesSystemJson[L"ready"].as_bool());
                break;
            default:
                throw std::invalid_argument("Enum value out of range");
        }

        VERIFY_ARE_EQUAL(result->XboxUserId->Data(), constantsSystemJson[L"xuid"].as_string());
        VERIFY_ARE_EQUAL(result->InitializeRequested, constantsSystemJson[L"initialize"].as_bool());

        string_t resultCustomProperties = result->MemberCustomPropertiesJson->Data();
        string_t systemJsonCustomProperties = propertiesJson[L"custom"].serialize().c_str();
        VERIFY_ARE_EQUAL(resultCustomProperties, systemJsonCustomProperties);

        string_t resultCustomConstants = result->MemberCustomConstantsJson->Data();
        string_t systemJsonCustomConstants = constantsJson[L"custom"].serialize().c_str();
        VERIFY_ARE_EQUAL(resultCustomConstants, systemJsonCustomConstants);

        VERIFY_ARE_EQUAL(result->Gamertag->Data(), resultToVerify[L"gamertag"].as_string());
        VERIFY_ARE_EQUAL(result->DeviceToken->Data(), resultToVerify[L"deviceToken"].as_string());
        VERIFY_ARE_EQUAL(result->Nat.ToString()->Data(), resultToVerify[L"nat"].as_string());
        VERIFY_ARE_EQUAL(result->IsTurnAvailable, resultToVerify[L"turn"].as_bool());
        VERIFY_ARE_EQUAL(result->SecureDeviceAddressBase64->Data(), propertiesSystemJson[L"secureDeviceAddress"].as_string());

        if (!rolesJson.is_null())
        {
            web::json::object roleObj = rolesJson.as_object();
            for (const auto& role : roleObj)
            {
                auto key = ref new Platform::String(role.first.c_str());
                auto value = ref new Platform::String(role.second.as_string().c_str());
                VERIFY_ARE_EQUAL(result->Roles->Lookup(key), value);
            }
        }

        string_t resultServerMeasurements = result->MemberServerMeasurementsJson->Data();
        string_t systemServerMeasurements = propertiesSystemJson[L"serverMeasurements"].serialize().c_str();
        VERIFY_ARE_EQUAL(resultServerMeasurements, systemServerMeasurements);

        VERIFY_ARE_EQUAL_INT(result->ActiveTitleId, std::stoul(resultToVerify[L"activeTitleId"].as_string()));

        TEST_LOG(DateTimeToString(result->JoinTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());
        TEST_LOG(resultToVerify[L"joinTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());

        VERIFY_ARE_EQUAL(
            DateTimeToString(result->JoinTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND), 
            resultToVerify[L"joinTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND)
            );
        VERIFY_ARE_EQUAL(result->InitializationFailureCause.ToString()->Data(), resultToVerify[L"initializationFailure"].as_string());
        VERIFY_ARE_EQUAL_INT(result->InitializationEpisode, resultToVerify[L"initializationEpisode"].as_integer());

        string_t resultMatchmaking = result->MatchmakingResultServerMeasurementsJson->Data();
        string_t systemMatchmaking = constantsSystemMatchmakingResultJson[L"serverMeasurements"].serialize().c_str();
        TEST_LOG(resultMatchmaking.c_str());
        TEST_LOG(systemMatchmaking.c_str());

        VERIFY_ARE_EQUAL(resultMatchmaking, systemMatchmaking);

        web::json::array changeTypes = propertiesSystemSubscriptionJson[L"changeTypes"].as_array();

        uint32_t changeTypeEnum = result->GetCppObj()->_Subscribed_change_types();
        for (auto changeType : changeTypes)
        {
            string_t changeTypeString = changeType.as_string();
            if (utils::str_icmp(changeTypeString, _T("everything")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::Everything)) == MultiplayerSessionChangeTypes::Everything);
            }
            else if (utils::str_icmp(changeTypeString, _T("host")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::HostDeviceTokenChange)) == MultiplayerSessionChangeTypes::HostDeviceTokenChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("initialization")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::InitializationStateChange)) == MultiplayerSessionChangeTypes::InitializationStateChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("matchmakingStatus")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::MatchmakingStatusChange)) == MultiplayerSessionChangeTypes::MatchmakingStatusChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("membersList")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::MemberListChange)) == MultiplayerSessionChangeTypes::MemberListChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("membersStatus")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::MemberStatusChange)) == MultiplayerSessionChangeTypes::MemberStatusChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("joinability")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::SessionJoinabilityChange)) == MultiplayerSessionChangeTypes::SessionJoinabilityChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("customProperty")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::CustomPropertyChange)) == MultiplayerSessionChangeTypes::CustomPropertyChange);
            }
            else if (utils::str_icmp(changeTypeString, _T("membersCustomProperty")) == 0)
            {
                VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(changeTypeEnum & static_cast<uint32_t>(MultiplayerSessionChangeTypes::MemberCustomPropertyChange)) == MultiplayerSessionChangeTypes::MemberCustomPropertyChange);
            }
        }
    }

    void VerifyMultiplayerSessionConstants(MultiplayerSessionConstants^ result, web::json::value resultToVerify)
    {
        web::json::value systemJson = resultToVerify[L"system"];
        VERIFY_ARE_EQUAL_INT(result->MaxMembersInSession, systemJson[L"maxMembersCount"].as_integer());
        VERIFY_ARE_EQUAL(result->MultiplayerSessionVisibility.ToString()->Data(), systemJson[L"visibility"].as_string());

        web::json::array initiatorsArray = systemJson[L"initiators"].as_array();
        uint32 counter = 0;
        for (auto initiators : result->InitiatorXboxUserIds)
        {
            VERIFY_ARE_EQUAL(initiators->Data(), initiatorsArray[counter].as_string());
            ++counter;
        }

        string_t resultCustomConstants = result->CustomConstantsJson->Data();
        string_t systemJsonCustomConstants = resultToVerify[L"custom"].serialize().c_str();
        VERIFY_ARE_EQUAL(resultCustomConstants, systemJsonCustomConstants);

        web::json::value systemCapabilitiesJson = systemJson[L"capabilities"];
        VERIFY_ARE_EQUAL(result->CapabilitiesConnectivity, systemCapabilitiesJson[L"connectivity"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesSuppressPresenceActivityCheck, systemCapabilitiesJson[L"suppressPresenceActivityCheck"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesGameplay, systemCapabilitiesJson[L"gameplay"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesLarge, systemCapabilitiesJson[L"large"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesConnectionRequiredForActiveMember, systemCapabilitiesJson[L"connectionRequiredForActiveMembers"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesCrossplay, systemCapabilitiesJson[L"crossplay"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesUserAuthorizationStyle, systemCapabilitiesJson[L"userAuthorizationStyle"].as_bool());
        VERIFY_ARE_EQUAL(result->CapabilitiesSearchable, systemCapabilitiesJson[L"searchable"].as_bool());

        TEST_LOG(result->MemberReservationTimeout.Duration.ToString()->Data());

        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->MemberReservationTimeout,
            systemJson[_T("reservedRemovalTimeout")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->MemberInactiveTimeout,
            systemJson[_T("inactiveRemovalTimeout")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->MemberReadyTimeout,
            systemJson[_T("readyRemovalTimeout")].as_number().to_uint64()
            );
        VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
            result->SessionEmptyTimeout,
            systemJson[_T("sessionEmptyTimeout")].as_number().to_uint64()
            );

        web::json::value systemArbitrationTimeoutsJson = systemJson[L"arbitration"];
        if (!systemArbitrationTimeoutsJson.is_null())
        {
            VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
                result->ArbitrationTimeout,
                systemArbitrationTimeoutsJson[_T("arbitrationTimeout")].as_number().to_uint64()
                );
            VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(
                result->ForfeitTimeout,
                systemArbitrationTimeoutsJson[_T("forfeitTimeout")].as_number().to_uint64()
                );
        }

        web::json::value systemMetricsJson = systemJson[L"metrics"];
        VERIFY_ARE_EQUAL(result->EnableMetricsLatency, systemMetricsJson[L"latency"].as_bool());
        VERIFY_ARE_EQUAL(result->EnableMetricsBandwidthDown, systemMetricsJson[L"bandwidthDown"].as_bool());
        VERIFY_ARE_EQUAL(result->EnableMetricsBandwidthUp, systemMetricsJson[L"bandwidthUp"].as_bool());
        VERIFY_ARE_EQUAL(result->EnableMetricsCustom, systemMetricsJson[L"custom"].as_bool());

        VerifyMultiplayerManagedInitialization(result->ManagedInitialization, systemJson[L"memberInitialization"]);
        VerifyMultiplayerMemberInitialization(result->MemberInitialization, systemJson[L"memberInitialization"]);

        VerifyMultiplayerPeerToPeerRequirements(result->PeerToPeerRequirements, systemJson[L"peerToPeerRequirements"]);

        VerifyMultiplayerPeerToHostRequirements(result->PeerToHostRequirements, systemJson[L"peerToHostRequirements"]);

    }

    void VerifyMultiplayerSessionRoleTypes(MultiplayerSessionRoleTypes^ result, web::json::value resultToVerify)
    {
        web::json::object roleTypesInfoJsonObj = resultToVerify[L"roleTypes"].as_object();
        for (const auto& roleTypeJson : roleTypesInfoJsonObj)
        {
            auto key = ref new Platform::String(roleTypeJson.first.c_str());
            VerifyMultiplayerRoleType(result->RoleTypes->Lookup(key), roleTypeJson.second);
        }
    }

    void VerifyMultiplayerSession(MultiplayerSession^ result, web::json::value resultToVerify)
    {
        web::json::value initializingJson = utils::extract_json_field(resultToVerify, _T("initializing"), false);
        web::json::value memberInfoJson = utils::extract_json_field(resultToVerify, _T("membersInfo"), false);

        VERIFY_ARE_EQUAL(result->Branch->Data(), resultToVerify[L"branch"].as_string());
        VERIFY_ARE_EQUAL_INT(result->ChangeNumber, resultToVerify[L"changeNumber"].as_integer());

        VERIFY_ARE_EQUAL(result->MultiplayerCorrelationId->Data(), resultToVerify[L"correlationId"].as_string());
        TEST_LOG(DateTimeToString(result->StartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());
        TEST_LOG(resultToVerify[L"startTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());

        VERIFY_ARE_EQUAL(DateTimeToString(result->StartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"startTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        VERIFY_ARE_EQUAL(DateTimeToString(result->StartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"nextTimer"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        VerifyMultiplayerSessionRoleTypes(result->SessionRoleTypes, resultToVerify);

        VerifyMultiplayerSessionConstants(result->SessionConstants, resultToVerify[L"constants"]);

        TEST_LOG(result->InitializationStage.ToString()->Data());
        TEST_LOG(initializingJson[L"stage"].as_string().c_str());
        VERIFY_ARE_EQUAL(result->InitializationStage.ToString()->Data(), initializingJson[L"stage"].as_string());

        TEST_LOG(DateTimeToString(result->InitializingStageStartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());
        TEST_LOG(initializingJson[L"stageStartTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());

        VERIFY_ARE_EQUAL(DateTimeToString(result->InitializingStageStartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            initializingJson[L"stageStartTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL_INT(result->InitializingEpisode, initializingJson[L"episode"].as_integer());
        
        web::json::array hostCandidateJson = resultToVerify[L"hostCandidates"].as_array();
        VERIFY_ARE_EQUAL_INT(result->HostCandidates->Size, hostCandidateJson.size());
        TEST_LOG(hostCandidateJson.size().ToString()->Data());

        uint32 counter = 0;
        for (auto hostCandidate : result->HostCandidates)
        {
            VERIFY_ARE_EQUAL(hostCandidate->Data(), hostCandidateJson[counter].as_string());
            ++counter;
        }

        web::json::value membersJson = resultToVerify[L"members"];
        uint32 memberCount = memberInfoJson[L"count"].as_integer();
        uint32 memberFirst = memberInfoJson[L"first"].as_integer();
        for (uint32 i = memberFirst; i < memberCount; ++i)
        {
            stringstream_t stream;
            stream << i;
            VerifyMultiplayerSessionMember(result->Members->GetAt(i), membersJson[stream.str()]);
        }

        VerifyMultiplayerSessionProperties(result->SessionProperties, resultToVerify[L"properties"]);

        VERIFY_ARE_EQUAL_INT(result->MembersAccepted, memberInfoJson[L"accepted"].as_integer());
        TEST_LOG(resultToVerify[L"servers"].serialize().c_str());
        TEST_LOG(result->ServersJson->Data());
        string_t test0 = result->ServersJson->Data();
        string_t test1 = resultToVerify[L"servers"].serialize().c_str();
        VERIFY_ARE_EQUAL(test0, test1);
    }

    void VerifyMultiplayerSessionStates(MultiplayerSessionStates^ result, web::json::value resultToVerify)
    {
        web::json::value sessionRefJson = resultToVerify[L"sessionRef"];
        VerifyMultiplayerSessionReference(result->SessionReference, sessionRefJson);

        VERIFY_ARE_EQUAL(result->IsMyTurn, resultToVerify[L"myTurn"].as_bool());
        VERIFY_ARE_EQUAL(result->Status.ToString()->Data(), resultToVerify[L"status"].as_string());
        VERIFY_ARE_EQUAL(result->Visibility.ToString()->Data(), resultToVerify[L"visibility"].as_string());
        VERIFY_ARE_EQUAL(result->JoinRestriction.ToString()->Data(), resultToVerify[L"joinRestriction"].as_string());
        VERIFY_ARE_EQUAL(DateTimeToString(result->StartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"startTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(result->XboxUserId->Data(), resultToVerify[L"xuid"].as_string());
        VERIFY_ARE_EQUAL_INT(result->AcceptedMemberCount, resultToVerify[L"accepted"].as_integer());
        
        web::json::array keywordsJson = resultToVerify[L"keywords"].as_array();
        uint32 counter = 0;
        for (auto keyword : result->Keywords)
        {
            VERIFY_ARE_EQUAL(keyword->Data(), keywordsJson[counter].as_string());
            ++counter;
        }
    }

    void VerifyActivityDetails(MultiplayerActivityDetails^ result, web::json::value resultToVerify)
    {
        web::json::value sessionRefJson = resultToVerify[L"sessionRef"];
        VerifyMultiplayerSessionReference(result->SessionReference, sessionRefJson);

        web::json::value relatedInfoJson = resultToVerify[L"relatedInfo"];

        string_t titleId = resultToVerify[L"titleId"].as_string();
        VERIFY_ARE_EQUAL_INT(result->TitleId, _wtoi(titleId.c_str()));
        VERIFY_ARE_EQUAL(result->OwnerXboxUserId->Data(), resultToVerify[L"ownerXuid"].as_string());
        VERIFY_ARE_EQUAL(result->JoinRestriction.ToString()->Data(), relatedInfoJson[L"joinRestriction"].as_string());
        VERIFY_ARE_EQUAL(result->Closed, relatedInfoJson[L"closed"].as_bool());
        VERIFY_ARE_EQUAL_INT(result->MaxMembersCount, relatedInfoJson[L"maxMembersCount"].as_integer());
        VERIFY_ARE_EQUAL_INT(result->MembersCount, relatedInfoJson[L"membersCount"].as_integer());

        web::json::value customPropertiesJson = resultToVerify[L"customProperties"];
        if (!customPropertiesJson.is_null())
        {
            VERIFY_ARE_EQUAL(result->CustomSessionPropertiesJson->Data(), customPropertiesJson.serialize());
        }
    }

    void VerifyMultiplayerRoleInfo(MultiplayerRoleInfo^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_INT(result->MaxMembersCount, resultToVerify[L"max"].as_integer());
        VERIFY_ARE_EQUAL_INT(result->MembersCount, resultToVerify[L"count"].as_integer());

        if(!resultToVerify[L"target"].is_null())
        {
            VERIFY_ARE_EQUAL_INT(result->TargetCount, resultToVerify[L"target"].as_integer());
        }

        if (!resultToVerify[L"memberXuids"].is_null())
        {
            web::json::array memberXuidsJson = resultToVerify[L"memberXuids"].as_array();
            uint32 counter = 0;
            for (const auto& xuid : result->MemberXboxUserIds)
            {
                VERIFY_ARE_EQUAL(xuid->Data(), memberXuidsJson[counter].as_string());
                ++counter;
            }
        }
    }

    void VerifyMultiplayerRoleType(MultiplayerRoleType^ result, web::json::value resultToVerify)
    {
        if (!resultToVerify[L"ownerManaged"].is_null())
        {
            VERIFY_ARE_EQUAL(result->OwnerManaged, resultToVerify[L"ownerManaged"].as_bool());
        }
        
        if (!resultToVerify[L"mutableRoleSettings"].is_null())
        {
            web::json::array mutableSettingsJson = resultToVerify[L"mutableRoleSettings"].as_array();
            uint32 counter = 0;
            for (const auto& setting : result->MutableRoleSettings)
            {
                switch (setting)
                {
                case MutableRoleSetting::Max:
                    VERIFY_ARE_EQUAL(L"max", mutableSettingsJson[counter].as_string());
                    break;
                case MutableRoleSetting::Target:
                    VERIFY_ARE_EQUAL(L"target", mutableSettingsJson[counter].as_string());
                    break;
                default:
                    throw std::invalid_argument("Enum value out of range");
                }
                ++counter;
            }
        }

        web::json::object rolesJsonObj = resultToVerify[L"roles"].as_object();
        for (const auto& roleJson : rolesJsonObj)
        {
            auto key = ref new Platform::String(roleJson.first.c_str());
            VerifyMultiplayerRoleInfo(result->Roles->Lookup(key), roleJson.second);
        }
    }

    void VerifySearchHandleDetails(MultiplayerSearchHandleDetails^ result, web::json::value resultToVerify)
    {
        web::json::value sessionRefJson = resultToVerify[L"sessionRef"];
        VerifyMultiplayerSessionReference(result->SessionReference, sessionRefJson);
        VERIFY_ARE_EQUAL(result->HandleId->Data(), resultToVerify[L"id"].as_string());
        VERIFY_ARE_EQUAL(DateTimeToString(result->HandleCreationTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            resultToVerify[L"createTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        web::json::value searchAttributesJson = resultToVerify[L"searchAttributes"];
        web::json::array tagsJson = searchAttributesJson[L"tags"].as_array();
        uint32 counter = 0;
        for (const auto& tag : result->Tags)
        {
            VERIFY_ARE_EQUAL(tag->Data(), tagsJson[counter].as_string());
            ++counter;
        }

        web::json::object stringMetadataObj = searchAttributesJson[L"strings"].as_object();
        for (const auto& stringsMetadata : stringMetadataObj)
        {
            auto key = ref new Platform::String(stringsMetadata.first.c_str());
            auto value = ref new Platform::String(stringsMetadata.second.as_string().c_str());
            VERIFY_ARE_EQUAL(result->StringsMetadata->Lookup(key), value);
        }

        web::json::object numberMetadataObj = searchAttributesJson[L"numbers"].as_object();
        for (const auto& numbersMetadata : numberMetadataObj)
        {
            auto key = ref new Platform::String(numbersMetadata.first.c_str());
            VERIFY_ARE_EQUAL(result->NumbersMetadata->Lookup(key), numbersMetadata.second.as_double());
        }

        web::json::value customPropertiesJson = resultToVerify[L"customProperties"];
        if (!customPropertiesJson.is_null())
        {
            VERIFY_ARE_EQUAL(result->CustomSessionPropertiesJson->Data(), customPropertiesJson.serialize());
        }

        web::json::value relatedInfoJson = resultToVerify[L"relatedInfo"];
        VERIFY_ARE_EQUAL(result->JoinRestriction.ToString()->Data(), relatedInfoJson[L"joinRestriction"].as_string());
        VERIFY_ARE_EQUAL(result->Closed, relatedInfoJson[L"closed"].as_bool());
        VERIFY_ARE_EQUAL_INT(result->MaxMembersCount, relatedInfoJson[L"maxMembersCount"].as_integer());
        VERIFY_ARE_EQUAL_INT(result->MembersCount, relatedInfoJson[L"membersCount"].as_integer());
        web::json::array sessionOwnersJson = relatedInfoJson[L"sessionOwners"].as_array();
        counter = 0;
        for (const auto& tag : result->SessionOwnerXboxUserIds)
        {
            VERIFY_ARE_EQUAL(tag->Data(), sessionOwnersJson[counter].as_string());
            ++counter;
        }

        web::json::value roleInfoJson = resultToVerify[L"roleInfo"];
        if (!roleInfoJson.is_null())
        {
            web::json::object roleTypesInfoJsonObj = roleInfoJson[L"roleTypes"].as_object();
            for (const auto& roleTypeJson : roleTypesInfoJsonObj)
            {
                auto key = ref new Platform::String(roleTypeJson.first.c_str());
                VerifyMultiplayerRoleType(result->RoleTypes->Lookup(key), roleTypeJson.second);
            }
        }
    }

    void GetSessionsAsyncHelper(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ sessionTemplateNameFilter,
        _In_opt_ Platform::String^ xboxUserIdFilter,
        _In_opt_ Platform::String^ keywordFilter,
        _In_ MultiplayerSessionVisibility visibilityFilter,
        _In_ uint32 contractVersionFilter,
        _In_ bool includePrivateSessions,
        _In_ bool includeReservations,
        _In_ bool includeInactiveSessions,
        _In_ uint32 maxItems,
        _In_ string_t pathAndQueryString
        )
    {
        const string_t defaultMultiplayerStatesResponse = testResponseJsonFromFile[L"defaultMultiplayerStatesResponse"].serialize();
        auto responseJson = web::json::value::parse(defaultMultiplayerStatesResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto getSessionsRequest = ref new MultiplayerGetSessionsRequest(
            serviceConfigurationId,
            maxItems
            );

#pragma warning(suppress: 6387)
        getSessionsRequest->SessionTemplateNameFilter = sessionTemplateNameFilter;
#pragma warning(suppress: 6387)
        getSessionsRequest->XboxUserIdFilter = xboxUserIdFilter;
#pragma warning(suppress: 6387)
        getSessionsRequest->KeywordFilter = keywordFilter;
        getSessionsRequest->VisibilityFilter = visibilityFilter;
        getSessionsRequest->ContractVersionFilter = contractVersionFilter;
        getSessionsRequest->IncludePrivateSessions = includePrivateSessions;
        getSessionsRequest->IncludeReservations = includeReservations;
        getSessionsRequest->IncludeInactiveSessions = includeInactiveSessions;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto results = create_task(
            xboxLiveContext->MultiplayerService->GetSessionsAsync(
                getSessionsRequest
                )
            ).get();
        
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            pathAndQueryString,
            httpCall->PathQueryFragment.to_string()
            );

        uint32 counter = 0;
        for (auto result : results)
        {
            VerifyMultiplayerSessionStates(result, responseJson[L"results"][counter]);
            ++counter;
        }
    }

    void GetSessionsWithUsersFilterAsyncHelper(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ sessionTemplateNameFilter,
        _In_opt_ IVectorView<Platform::String^>^ xboxUserIds,
        _In_opt_ Platform::String^ keywordFilter,
        _In_ MultiplayerSessionVisibility visibilityFilter,
        _In_ uint32 contractVersionFilter,
        _In_ bool includePrivateSessions,
        _In_ bool includeReservations,
        _In_ bool includeInactiveSessions,
        _In_ uint32 maxItems,
        _In_ string_t actualJsonString,
        _In_ string_t pathAndQueryString
        )
    {
        const string_t defaultMultiplayerStatesResponse = testResponseJsonFromFile[L"defaultMultiplayerStatesResponse"].serialize();
        auto responseJson = web::json::value::parse(defaultMultiplayerStatesResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto getSessionsRequest = ref new MultiplayerGetSessionsRequest(
            serviceConfigurationId,
            maxItems
            );

#pragma warning(suppress: 6387)
        getSessionsRequest->SessionTemplateNameFilter = sessionTemplateNameFilter;
#pragma warning(suppress: 6387)
        getSessionsRequest->XboxUserIdsFilter = xboxUserIds;
#pragma warning(suppress: 6387)
        getSessionsRequest->KeywordFilter = keywordFilter;
        getSessionsRequest->VisibilityFilter = visibilityFilter;
        getSessionsRequest->ContractVersionFilter = contractVersionFilter;
        getSessionsRequest->IncludePrivateSessions = includePrivateSessions;
        getSessionsRequest->IncludeReservations = includeReservations;
        getSessionsRequest->IncludeInactiveSessions = includeInactiveSessions;

        auto results = create_task(
            xboxLiveContext->MultiplayerService->GetSessionsAsync(
                getSessionsRequest
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            pathAndQueryString,
            httpCall->PathQueryFragment.to_string()
            );

        auto actualJson = web::json::value::parse(actualJsonString);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));

        VERIFY_IS_EQUAL_JSON(requestJson, actualJson);
        uint32 counter = 0;
        for (auto result : results)
        {
            VerifyMultiplayerSessionStates(result, responseJson[L"results"][counter]);
            ++counter;
        }
    }

    MultiplayerSession^ GetCurrentSessionAsyncHelper() { return GetCurrentSessionAsyncHelper(defaultMultiplayerResponse); };

    MultiplayerSession^ GetCurrentSessionAsyncHelper(const string_t response)
    {
        auto responseJson = web::json::value::parse(response);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        MultiplayerSession^ result = create_task(
            xboxLiveContext->MultiplayerService->GetCurrentSessionAsync(
                ref new MultiplayerSessionReference(
                    L"8d050174-412b-4d51-a29b-d55a34edfdb7",
                    L"integration",
                    L"19de0095d8bb41048f19edbbb6bc6b04"
                    )
                )
            ).get();

        return result;
    }

    void WriteSessionAsyncHelper(MultiplayerSession^ currentSession, const string_t& expectedJson)
    {
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);
        
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        
        auto result = create_task(
        xboxLiveContext->MultiplayerService->WriteSessionAsync(
            currentSession,
            MultiplayerSessionWriteMode::UpdateExisting
            )  
        ).get();

        auto writeJson = web::json::value::parse(expectedJson);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        TEST_LOG(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()).c_str());
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);
    }

    void TryWriteSessionAsyncHelper(MultiplayerSession^ currentSession, bool shouldSucceed, int32_t httpStatus, bool returnEmpty = false)
    {
        UNREFERENCED_PARAMETER(returnEmpty);
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson, httpStatus);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(
            xboxLiveContext->MultiplayerService->TryWriteSessionAsync(
                currentSession,
                MultiplayerSessionWriteMode::UpdateExisting
                )
            ).get();


        WriteSessionStatus status;
        switch (httpStatus)
        {
        case 200:
            status = WriteSessionStatus::Updated;
            break;

        case 201:
            status = WriteSessionStatus::Created;
            break;

        case 204:
            status = WriteSessionStatus::SessionDeleted;
            break;

        case 401:
            status = WriteSessionStatus::AccessDenied;
            break;

        case 404:
            status = WriteSessionStatus::HandleNotFound;
            break;

        case 409:
            status = WriteSessionStatus::Conflict;
            break;

        case 412:
            status = WriteSessionStatus::OutOfSync;
            break;
        }
        
        VERIFY_ARE_EQUAL_INT(result->Status, status);
        VERIFY_ARE_EQUAL(result->Succeeded, shouldSucceed);
    }

    DEFINE_TEST_CASE(TestGetCurrentSessionAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetCurrentSessionAsync);
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        MultiplayerSession^ result = create_task(
            xboxLiveContext->MultiplayerService->GetCurrentSessionAsync(
                ref new MultiplayerSessionReference(
                    L"8d050174-412b-4d51-a29b-d55a34edfdb7",
                    L"integration",
                    L"19de0095d8bb41048f19edbbb6bc6b04"
                    )
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/serviceconfigs/8d050174-412b-4d51-a29b-d55a34edfdb7/sessionTemplates/integration/sessions/19de0095d8bb41048f19edbbb6bc6b04", 
            httpCall->PathQueryFragment.to_string()
            );
        
        VerifyMultiplayerSession(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetCurrentSessionWithHandleAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetCurrentSessionWithHandleAsync);
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        MultiplayerSession^ result = create_task(
            xboxLiveContext->MultiplayerService->GetCurrentSessionByHandleAsync(
                L"testhandle"
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles/testhandle/session", 
            httpCall->PathQueryFragment.to_string()
            );
        
        VerifyMultiplayerSession(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetSessionsAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSessionsAsync);
        // without keyword filter and without context version filter
        GetSessionsAsyncHelper(
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            "integration",
            "12345678",
            nullptr,
            MultiplayerSessionVisibility::Full,
            0,
            true,
            true,
            true,
            100,
            L"/serviceconfigs/8d050174-412b-4d51-a29b-d55a34edfdb7/sessiontemplates/integration/sessions?xuid=12345678&visibility=full&private=true&reservations=true&inactive=true&take=100"
            );

        // without xbox user id filter and with all the include settings off
        GetSessionsAsyncHelper(
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            "integration",
            nullptr,
            "hello",
            MultiplayerSessionVisibility::Any,
            5,
            false,
            false,
            false,
            100,
            L"/serviceconfigs/8d050174-412b-4d51-a29b-d55a34edfdb7/sessiontemplates/integration/sessions?keyword=hello&version=5&take=100"
            );
    }

    DEFINE_TEST_CASE(TestGetSessionWithFiltersAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSessionWithFiltersAsync);

        const string_t getSessionsWithUsersJson = testResponseJsonFromFile[L"getSessionsWithUsersJson"].serialize();
        Vector<Platform::String^>^ xuids = ref new Vector<Platform::String^>();
        xuids->Append("12345");
        xuids->Append("67890");
        // without keyword filter and without context version filter
        GetSessionsWithUsersFilterAsyncHelper(
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            "integration",
            xuids->GetView(),
            nullptr,
            MultiplayerSessionVisibility::Full,
            0,
            true,
            true,
            true,
            100,
            getSessionsWithUsersJson,
            L"/serviceconfigs/8d050174-412b-4d51-a29b-d55a34edfdb7/sessiontemplates/integration/batch?visibility=full&private=true&reservations=true&inactive=true&take=100"
            );
    }

    DEFINE_TEST_CASE(TestLargeMemberSessionAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLargeMemberSessionAsync);
        const string_t largeMultiplayerResponse = testResponseJsonFromFile[L"largeMultiplayerResponse"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper(largeMultiplayerResponse);
        VERIFY_IS_NOT_NULL(currentSession->Members->GetAt(0)); // the deserialize_me function was called correctly
    }

    DEFINE_TEST_CASE(TestCreateSessionAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCreateSessionAsync);
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        MultiplayerSession^ session = ref new MultiplayerSession(
            xboxLiveContext,
            ref new MultiplayerSessionReference(
                L"8d050174-412b-4d51-a29b-d55a34edfdb7",
                L"integration",
                L"12345678"
                )
            );

        session->Join("\"Hello\"", true, true);
        session->SetCurrentUserStatus(MultiplayerSessionMemberStatus::Active);
        session->SetVisibility(MultiplayerSessionVisibility::Private);
        session->SetMaxMembersInSession(10);
        Windows::Foundation::Collections::IVector<Platform::String^>^ sessionGroups = ref new Vector<Platform::String^>();
        sessionGroups->Append("team-buzz");
        sessionGroups->Append("posse.99");

        session->CurrentUser->Groups = sessionGroups;

        Windows::Foundation::Collections::IVector<Platform::String^>^ encounters = ref new Vector<Platform::String^>();
        encounters->Append("CoffeeShop-757093D8-E41F-49D0-BB13-17A49B20C6B9");
        session->CurrentUser->Encounters = encounters;

        auto rolesMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
        rolesMap->Insert(L"lfg", L"friend");
        rolesMap->Insert(L"admin", L"super");
        session->SetCurrentUserRoleInfo(rolesMap->GetView());

        auto result = create_task(
                xboxLiveContext->MultiplayerService->WriteSessionAsync(
                session,
                MultiplayerSessionWriteMode::CreateNew
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"PUT", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/serviceconfigs/8d050174-412b-4d51-a29b-d55a34edfdb7/sessionTemplates/integration/sessions/12345678",
            httpCall->PathQueryFragment.to_string()
            );

        const string_t defaultJsonWrite = testResponseJsonFromFile[L"defaultJsonWrite"].serialize();
        auto writeJson = web::json::value::parse(defaultJsonWrite);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);
    }

    DEFINE_TEST_CASE(TestSynchronizedUpdateAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSynchronizedUpdateAsync);
        auto currentSession = GetCurrentSessionAsyncHelper();
        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(
            xboxLiveContext->MultiplayerService->WriteSessionAsync(
            currentSession,
            MultiplayerSessionWriteMode::SynchronizedUpdate
            )
            ).get();
    }

    DEFINE_TEST_CASE(TestWriteSessionCapabilities)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionCapabilities);
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());

        MultiplayerSessionCapabilities^ capabilities = ref new MultiplayerSessionCapabilities();
        capabilities->Connectivity = true;
        capabilities->Gameplay = true;
        capabilities->Large = true;
        capabilities->SuppressPresenceActivityCheck = true;
        capabilities->ConnectionRequiredForActiveMembers = true;
        capabilities->Crossplay = true;
        capabilities->UserAuthorizationStyle = true;
        capabilities->HasOwners = true;
        capabilities->Searchable = true;

        currentSession->SetSessionCapabilities(
            capabilities
            );
        const string_t sessionCapabilitiesJson = testResponseJsonFromFile[L"sessionCapabilitiesJson"].serialize();
        WriteSessionAsyncHelper(currentSession, sessionCapabilitiesJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithServersJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithServersJson);
        const string_t serversJson = testResponseJsonFromFile[L"serversJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->ServersJson = L"{ \"server\" : 100 }";
        WriteSessionAsyncHelper(currentSession, serversJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetSessionCustomPropertyJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetSessionCustomPropertyJson);
        const string_t writeCustomPropertyJson = testResponseJsonFromFile[L"writeCustomPropertyJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetSessionCustomPropertyJson(L"PropA", L"ValueA");
        currentSession->SetSessionCustomPropertyJson(L"PropB", L"{\"ValueB\":5}");

        // Ensure that properties are not written to the local state
        string_t sessionCustomProperties = currentSession->SessionProperties->SessionCustomPropertiesJson->Data();
        VERIFY_ARE_EQUAL(sessionCustomProperties, L"{}");

        currentSession->SetSessionCustomPropertyJson(L"boolTrue", L"true");
        currentSession->SetSessionCustomPropertyJson(L"boolFalse", L"false");
        currentSession->SetSessionCustomPropertyJson(L"stringTrue", L"\"true\"");
        currentSession->SetSessionCustomPropertyJson(L"stringFalse", L"\"false\"");
        currentSession->SetSessionCustomPropertyJson(L"number42", L"42");
        currentSession->SetSessionCustomPropertyJson(L"objectA", L"{\"name\":\"A\"}");
        currentSession->SetSessionCustomPropertyJson(L"arrayA", L"[1,2,3,4]");

        WriteSessionAsyncHelper(currentSession, writeCustomPropertyJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingProperties)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetMatchmakingProperties);
        const string_t matchmakingPropertiesJson = testResponseJsonFromFile[L"matchmakingPropertiesJson"].serialize();
        MultiplayerSessionResponseTestData data = GetDefaultMultiplayerSessionResponseTestData(0, false);
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());

        currentSession->SetMatchmakingTargetSessionConstantsJson(
            L"{\"foo\":\"test1002\"}"
            );

        WriteSessionAsyncHelper(currentSession, matchmakingPropertiesJson);
    }

    DEFINE_TEST_CASE(WriteSessionAsyncWithSetMatchmakingConstantsJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(WriteSessionAsyncWithSetMatchmakingConstantsJson);
        const string_t matchmakingSessionConstantsJson = testResponseJsonFromFile[L"matchmakingSessionConstantsJson"].serialize();
        MultiplayerSessionResponseTestData data = GetDefaultMultiplayerSessionResponseTestData(0, false);
        auto currentSession = GetCurrentSessionAsyncHelper();

        currentSession->SetMatchmakingTargetSessionConstantsJson(
            TestDataToJson(data.properties_system_matchmaking_targetSessionConstants)
            );

        WriteSessionAsyncHelper(currentSession, matchmakingSessionConstantsJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithAddMemberReservation)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithAddMemberReservation);
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->AddMemberReservation(
            L"1234",
            L"{ \"skill\" : 100 }",
            true
            );

        currentSession->AddMemberReservation(
            L"4567",
            L"{ \"down\" : true }"
            );

        const string_t writeAddMemberJson = testResponseJsonFromFile[L"writeAddMemberJson"].serialize();
        WriteSessionAsyncHelper(currentSession, writeAddMemberJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithDeleteSessionCustomPropertyJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithDeleteSessionCustomPropertyJson);
        const string_t deleteCustomPropertyJson = testResponseJsonFromFile[L"deleteCustomPropertyJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->DeleteSessionCustomPropertyJson(L"PropA");
        currentSession->DeleteSessionCustomPropertyJson(L"PropB");

        currentSession->DeleteSessionCustomPropertyJson(L"boolTrue");
        currentSession->DeleteSessionCustomPropertyJson(L"boolFalse");
        currentSession->DeleteSessionCustomPropertyJson(L"stringTrue");
        currentSession->DeleteSessionCustomPropertyJson(L"stringFalse");
        currentSession->DeleteSessionCustomPropertyJson(L"number42");
        currentSession->DeleteSessionCustomPropertyJson(L"objectA");
        currentSession->DeleteSessionCustomPropertyJson(L"arrayA");

        WriteSessionAsyncHelper(currentSession, deleteCustomPropertyJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithJoin1);
        const std::wstring joinJson =
        LR"(
        {
            "members": {
                "me": {
                    "constants": {
                        "system": {
                            "xuid": "TestXboxUserId"
                        }
                    }
                }
            }
        }
        )";

        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join();
        WriteSessionAsyncHelper(currentSession, joinJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithJoin2);
        const std::wstring joinJson =
            LR"(
        {
            "members": {
                "me": {
                    "constants": {
                        "system": {
                            "xuid": "TestXboxUserId"
                        },
                        "custom": {
                            "testValue": 100
                        }
                    }
                }
            }
        }
        )";

        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join(L"{ \"testValue\" : 100 }");
        WriteSessionAsyncHelper(currentSession, joinJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin4)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithJoin4);
        const std::wstring joinJson =
            LR"(
        {
            "members": {
                "me": {
                    "constants": {
                        "system": {
                            "xuid": "TestXboxUserId",
                            "initialize": true
                        },
                        "custom": {
                            "testValue": 100
                        }
                    },
                    "properties": {
                        "system": {
                            "active": true
                        }
                    }
                }
            }
        }
        )";

        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join(L"{ \"testValue\" : 100 }", true, true);
        WriteSessionAsyncHelper(currentSession, joinJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithLeave)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithLeave);
        const string_t leaveJson = testResponseJsonFromFile[L"leaveJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Leave();
        WriteSessionAsyncHelper(currentSession, leaveJson);
    }


    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserInactive)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserInactive);
        const string_t memberStatusInactiveJson = testResponseJsonFromFile[L"memberStatusInactiveJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join();
        currentSession->SetCurrentUserStatus(MultiplayerSessionMemberStatus::Inactive);
        WriteSessionAsyncHelper(currentSession, memberStatusInactiveJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserActive)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserActive);
        const string_t memberStatusActiveJson = testResponseJsonFromFile[L"memberStatusActiveJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join();
        currentSession->SetCurrentUserStatus(MultiplayerSessionMemberStatus::Active);
        WriteSessionAsyncHelper(currentSession, memberStatusActiveJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserSecureDeviceAddressBase64)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserSecureDeviceAddressBase64);
        const string_t secureDeviceJson = testResponseJsonFromFile[L"secureDeviceJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join(nullptr, false, true);
        currentSession->SetCurrentUserSecureDeviceAddressBase64(L"1234");
        WriteSessionAsyncHelper(currentSession, secureDeviceJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserMemberCustomPropertyJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserMemberCustomPropertyJson);
        const string_t memberCustomJson = testResponseJsonFromFile[L"memberCustomJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join(nullptr, false, true);
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"health", L"4567");

        // Ensure that properties are not written to the local state
        string_t currentUserCustomProperties = currentSession->CurrentUser->MemberCustomPropertiesJson->Data();
        VERIFY_ARE_EQUAL(currentUserCustomProperties, L"{}");

        currentSession->SetCurrentUserMemberCustomPropertyJson(L"boolTrue", L"true");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"boolFalse", L"false");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"stringTrue", L"\"true\"");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"stringFalse", L"\"false\"");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"number42", L"42");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"objectA", L"{\"name\":\"A\"}");
        currentSession->SetCurrentUserMemberCustomPropertyJson(L"arrayA", L"[1,2,3,4]");
        WriteSessionAsyncHelper(currentSession, memberCustomJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithDeleteCurrentUserMemberCustomPropertyJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithDeleteCurrentUserMemberCustomPropertyJson);
        const string_t deleteMemberCustomJson = testResponseJsonFromFile[L"deleteMemberCustomJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join(nullptr, false, true);
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"health");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"boolTrue");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"boolFalse");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"stringTrue");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"stringFalse");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"number42");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"objectA");
        currentSession->DeleteCurrentUserMemberCustomPropertyJson(L"arrayA");
        WriteSessionAsyncHelper(currentSession, deleteMemberCustomJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithTurnCollection)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithTurnCollection);
        const string_t turnJson = testResponseJsonFromFile[L"turnJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        Vector<MultiplayerSessionMember^>^ newTurnCollection = ref new Vector<MultiplayerSessionMember^>();
        newTurnCollection->Append(currentSession->Members->GetAt(0));
        currentSession->SessionProperties->TurnCollection = newTurnCollection->GetView();
        WriteSessionAsyncHelper(currentSession, turnJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithKeywords)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithKeywords);
        const string_t keywordsJson = testResponseJsonFromFile[L"keywordsJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        Vector<Platform::String^>^ newKeywords = ref new Vector<Platform::String^>();
        newKeywords->Append(L"apple");

        currentSession->SessionProperties->Keywords = newKeywords->GetView();
        WriteSessionAsyncHelper(currentSession, keywordsJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoinRestriction)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithJoinRestriction);
        const string_t joinRestrictionJson = testResponseJsonFromFile[L"joinRestrictionJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SessionProperties->JoinRestriction = MultiplayerSessionRestriction::Followed;
        WriteSessionAsyncHelper(currentSession, joinRestrictionJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithReadRestriction)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithReadRestriction);
        const string_t readRestrictionJson = testResponseJsonFromFile[L"readRestrictionJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SessionProperties->ReadRestriction = MultiplayerSessionRestriction::Followed;
        WriteSessionAsyncHelper(currentSession, readRestrictionJson);
    }

    DEFINE_TEST_CASE(TestTryWriteSessionAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestTryWriteSessionAsync);
        auto currentSession = GetCurrentSessionAsyncHelper();
        TryWriteSessionAsyncHelper(currentSession, true, 204, true);
        TryWriteSessionAsyncHelper(currentSession, true, 200);
        TryWriteSessionAsyncHelper(currentSession, true, 201);
        TryWriteSessionAsyncHelper(currentSession, false, 401, true);
        TryWriteSessionAsyncHelper(currentSession, false, 409, true);
        TryWriteSessionAsyncHelper(currentSession, false, 412, false);
        TryWriteSessionAsyncHelper(currentSession, false, 404, true);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetTimeouts)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetTimeouts);
        const string_t timeoutsJson = testResponseJsonFromFile[L"timeoutsJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());

        currentSession->SetTimeouts(
            input.MemberReservedTimeout,
            input.MemberInactiveTimeout,
            input.MemberReadyTimeout,
            input.sessionEmpty
            );
        WriteSessionAsyncHelper(currentSession, timeoutsJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetArbitrationTimeouts)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetArbitrationTimeouts);
        const string_t arbitrationTimeoutsJson = testResponseJsonFromFile[L"arbitrationTimeoutsJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());

        currentSession->SetArbitrationTimeouts(
            input.ArbitrationTimeout,
            input.ForfeitTimeout
            );
        WriteSessionAsyncHelper(currentSession, arbitrationTimeoutsJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetQualityOfServiceConnectivityMetrics)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetQualityOfServiceConnectivityMetrics);
        const string_t qosMetricsJson = testResponseJsonFromFile[L"qosMetricsJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetQualityOfServiceConnectivityMetrics(
            true,
            true,
            true,
            true
            );
        WriteSessionAsyncHelper(currentSession, qosMetricsJson);
    }

    DEFINE_TEST_CASE(TestGetMeasurementServerAddresses)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetMeasurementServerAddresses);
        const string_t measurementServerAddressJson = testResponseJsonFromFile[L"measurementServerAddressJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetMeasurementServerAddresses(
            input.MeasurementServerAddresses
            );
        WriteSessionAsyncHelper(currentSession, measurementServerAddressJson);
    }

    DEFINE_TEST_CASE(TestGetCloudComputePackageJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetCloudComputePackageJson);
        const string_t cloudComputePackageJson = testResponseJsonFromFile[L"cloudComputePackageJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetCloudComputePackageJson(
            L"{\"crossSandbox\":true, \"titleId\":\"4567\", \"gsiSet\":\"128ce92a-45d0-4319-8a7e-bd8e940114ec\"}"
            );
        WriteSessionAsyncHelper(currentSession, cloudComputePackageJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetManagedInitialization)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetManagedInitialization);
        const string_t managedInitializationJson = testResponseJsonFromFile[L"managedInitializationJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
#pragma warning(suppress: 4973)
        currentSession->SetManagedInitialization(
            input.JoinTimeout,
            input.MeasurementTimeout,
            input.EvaluationTimeout,
            input.AutoEvalute,
            input.MembersNeededToStart
            );

        WriteSessionAsyncHelper(currentSession, managedInitializationJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMemberInitialization)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetManagedInitialization);
        const string_t managedInitializationJson = testResponseJsonFromFile[L"memberInitializationJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetMemberInitialization(
            input.JoinTimeout,
            input.MeasurementTimeout,
            input.EvaluationTimeout,
            input.ExternalEvaluation,
            input.MembersNeededToStart
            );

        WriteSessionAsyncHelper(currentSession, managedInitializationJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetPeerToPeerRequirements)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetPeerToPeerRequirements);
        const string_t peerToPeerJson = testResponseJsonFromFile[L"peerToPeerJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetPeerToPeerRequirements(
            input.PeerToPeerRequirementsLatencyMaximum,
            input.PeerToPeerRequirementsBandwidthMinimumInKilobitsPerSecond
            );
        WriteSessionAsyncHelper(currentSession, peerToPeerJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetPeerToHostRequirements)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetPeerToHostRequirements);
        const string_t peerToHostJson = testResponseJsonFromFile[L"peerToHostJson"].serialize();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->SetPeerToHostRequirements(
            input.PeerToHostRequirementsLatencyMaximum,
            input.PeerToHostRequirementsBandwidthDownMinimumInKilobitsPerSecond,
            input.PeerToHostRequirementsBandwidthUpMinimumInKilobitsPerSecond,
            input.PeerToHostRequirementsHostSelectionMetric
            );
        WriteSessionAsyncHelper(currentSession, peerToHostJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetInitializationStatus)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetInitializationStatus);
        const string_t initializationStatusJson = testResponseJsonFromFile[L"initializationStatusJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetInitializationStatus(true);
        WriteSessionAsyncHelper(currentSession, initializationStatusJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetHostDeviceToken)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetHostDeviceToken);
        const string_t hostDeviceTokenJson = testResponseJsonFromFile[L"hostDeviceTokenJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetHostDeviceToken(L"1234");
        WriteSessionAsyncHelper(currentSession, hostDeviceTokenJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingServerConnectionPath)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetMatchmakingServerConnectionPath);
        const string_t serverConnectionPathJson = testResponseJsonFromFile[L"serverConnectionPathJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetMatchmakingServerConnectionPath(
            L"8001"
            );
        WriteSessionAsyncHelper(currentSession, serverConnectionPathJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingResubmit)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetMatchmakingResubmit);
        const string_t matchmakingResubmitJson = testResponseJsonFromFile[L"matchmakingResubmitJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetMatchmakingResubmit(true);
        WriteSessionAsyncHelper(currentSession, matchmakingResubmitJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetServerConnectionStringCandidates)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetServerConnectionStringCandidates);
        const string_t serverConnectionStringCandidatesJson = testResponseJsonFromFile[L"serverConnectionStringCandidatesJson"].serialize();

        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetServerConnectionStringCandidates(input.ServerConnectionStringCandidates);
        WriteSessionAsyncHelper(currentSession, serverConnectionStringCandidatesJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserMembersInGroup)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserMembersInGroup);
        const string_t membersInGroupJson = testResponseJsonFromFile[L"membersInGroupJson"].serialize();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->Join(nullptr, false, true);

        Vector<MultiplayerSessionMember^>^ membersInGroup = ref new Vector<MultiplayerSessionMember^>();
        membersInGroup->Append(currentSession->Members->GetAt(0));

        currentSession->SetCurrentUserMembersInGroup(
            membersInGroup->GetView()
            );

        WriteSessionAsyncHelper(currentSession, membersInGroupJson);
        auto membersList = currentSession->Members;
        for (auto member : membersList)
        {
            VERIFY_IS_TRUE(member->MembersInGroup->Size == 1);
        }
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithClosed)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithClosed);
        const string_t closedJsonTrue = testResponseJsonFromFile[L"closedJsonTrue"].serialize();
        const string_t closedJsonFalse = testResponseJsonFromFile[L"closedJsonFalse"].serialize();

        MultiplayerSession^ currentSession = GetCurrentSessionAsyncHelper();

        // can be set to true of false -- test both
        currentSession->SetClosed(true);
        WriteSessionAsyncHelper(currentSession, closedJsonTrue);

        currentSession->SetClosed(false);
        WriteSessionAsyncHelper(currentSession, closedJsonFalse);

    }

    TEST_METHOD(TestWriteSessionAsyncWithLocked)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithLocked);
        const string_t lockedJsonTrue = testResponseJsonFromFile[L"lockedJsonTrue"].serialize();
        const string_t lockedJsonFalse = testResponseJsonFromFile[L"lockedJsonFalse"].serialize();

        MultiplayerSession^ currentSession = GetCurrentSessionAsyncHelper();
        currentSession->SetLocked(true);
        WriteSessionAsyncHelper(currentSession, lockedJsonTrue);

        currentSession->SetLocked(false);
        WriteSessionAsyncHelper(currentSession, lockedJsonFalse);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceMeasurements)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceMeasurements);
        const string_t qosMeasurementsJson = testResponseJsonFromFile[L"qosMeasurementsJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        currentSession->Join();
        MultiplayerQualityOfServiceMeasurements^ measurement = ref new MultiplayerQualityOfServiceMeasurements(
            "test1_deviceToken",
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::milliseconds(1001)),
            1002,
            1003,
            L"{\"foo\":1004}"
            );

        Vector<MultiplayerQualityOfServiceMeasurements^>^ measurements = ref new Vector<MultiplayerQualityOfServiceMeasurements^>();
        measurements->Append(measurement);

        currentSession->SetCurrentUserQualityOfServiceMeasurements(
            measurements->GetView()
            );
        WriteSessionAsyncHelper(currentSession, qosMeasurementsJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceServerMeasurementsJson)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceServerMeasurementsJson);
        const string_t qosMeasurementsFromJsonJson = testResponseJsonFromFile[L"qosMeasurementsFromJsonJson"].serialize();
        auto currentSession = ref new MultiplayerSession(GetMockXboxLiveContext_WinRT());
        currentSession->Join(nullptr, false, true);
        currentSession->SetCurrentUserQualityOfServiceServerMeasurementsJson(
            L"{\"measurement\":1004}"
            );
        WriteSessionAsyncHelper(currentSession, qosMeasurementsFromJsonJson);
    }

    DEFINE_TEST_CASE(TestDataIntegrity)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestDataIntegrity);
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto currentSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        currentSession->Join(nullptr, false, true);
        currentSession->SetQualityOfServiceConnectivityMetrics(
            true,
            true,
            true,
            true
            );

        VERIFY_ARE_EQUAL(currentSession->SessionConstants->EnableMetricsBandwidthDown, true);
        VERIFY_ARE_EQUAL(currentSession->SessionConstants->EnableMetricsBandwidthUp, true);
        VERIFY_ARE_EQUAL(currentSession->SessionConstants->EnableMetricsCustom, true);
        VERIFY_ARE_EQUAL(currentSession->SessionConstants->EnableMetricsLatency, true);

        currentSession->SetTimeouts(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3003)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3004))
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberReservationTimeout).count(), 3001);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberInactiveTimeout).count(), 3002);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberReadyTimeout).count(), 3003);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->SessionEmptyTimeout).count(), 3004);
        
        currentSession->SetArbitrationTimeouts(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002))
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->ArbitrationTimeout).count(), 3001);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->ForfeitTimeout).count(), 3002);

        Platform::String^ matchmakingTargetSessionConstantsJson = ref new Platform::String(L"{\"measurement\":1004}");
        currentSession->SetMatchmakingTargetSessionConstantsJson(
            matchmakingTargetSessionConstantsJson
            );

        VERIFY_ARE_EQUAL(matchmakingTargetSessionConstantsJson, currentSession->SessionProperties->MatchmakingTargetSessionConstantsJson);
    
        currentSession->SetCurrentUserStatus(MultiplayerSessionMemberStatus::Inactive);

        VERIFY_ARE_EQUAL_INT(currentSession->CurrentUser->Status, MultiplayerSessionMemberStatus::Inactive);

        currentSession->SetPeerToPeerRequirements(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            100
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->PeerToPeerRequirements->LatencyMaximum).count(), 3001);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->PeerToPeerRequirements->BandwidthMinimumInKilobitsPerSecond, 100);
        
        currentSession->SetPeerToHostRequirements(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            100,
            150,
            MultiplayMetrics::Latency
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->PeerToHostRequirements->LatencyMaximum).count(), 3001);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->PeerToHostRequirements->BandwidthDownMinimumInKilobitsPerSecond, 100);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->PeerToHostRequirements->BandwidthUpMinimumInKilobitsPerSecond, 150);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->PeerToHostRequirements->HostSelectionMetric, MultiplayMetrics::Latency);

#pragma warning(suppress: 4973)
        currentSession->SetManagedInitialization(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3003)),
            true,
            1000
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->ManagedInitialization->JoinTimeout).count(), 3001);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->ManagedInitialization->MeasurementTimeout).count(), 3002);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->ManagedInitialization->EvaluationTimeout).count(), 3003);
        VERIFY_ARE_EQUAL(currentSession->SessionConstants->ManagedInitialization->AutoEvaluate, true);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->ManagedInitialization->MembersNeededToStart, 1000);

        currentSession->SetMemberInitialization(
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3002)),
            UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3003)),
            false,
            1000
            );

        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberInitialization->JoinTimeout).count(), 3001);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberInitialization->MeasurementTimeout).count(), 3002);
        VERIFY_ARE_EQUAL_INT(UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(currentSession->SessionConstants->MemberInitialization->EvaluationTimeout).count(), 3003);
        VERIFY_ARE_EQUAL(currentSession->SessionConstants->MemberInitialization->ExternalEvaluation, false);
        VERIFY_ARE_EQUAL_INT(currentSession->SessionConstants->MemberInitialization->MembersNeededToStart, 1000);

        Vector<MultiplayerQualityOfServiceMeasurements^>^ qosVector = ref new Vector<MultiplayerQualityOfServiceMeasurements^>();
        Platform::String^ testString = ref new Platform::String(L"test");
        Platform::String^ measurementJsonString = ref new Platform::String(L"{\"measurement\":1004}");
        qosVector->Append(
            ref new MultiplayerQualityOfServiceMeasurements(
                testString,
                UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(3001)),
                1000,
                1001,
                measurementJsonString
                )
            );

        currentSession->SetCurrentUserQualityOfServiceMeasurements(
            qosVector->GetView()
            );

        VERIFY_ARE_EQUAL_STR(currentSession->CurrentUser->MemberMeasurements->GetAt(0)->MemberDeviceToken, testString);
        VERIFY_ARE_EQUAL_INT(currentSession->CurrentUser->MemberMeasurements->GetAt(0)->BandwidthDownInKilobitsPerSecond, 1000);
        VERIFY_ARE_EQUAL_INT(currentSession->CurrentUser->MemberMeasurements->GetAt(0)->BandwidthUpInKilobitsPerSecond, 1001);
        VERIFY_ARE_EQUAL_STR(currentSession->CurrentUser->MemberMeasurements->GetAt(0)->CustomJson, measurementJsonString);

        MultiplayerSessionCapabilities^ sessionCapabilities = ref new MultiplayerSessionCapabilities();
        sessionCapabilities->Gameplay = true;

        currentSession->SetSessionCapabilities(sessionCapabilities);

        VERIFY_ARE_EQUAL(currentSession->SessionConstants->CapabilitiesGameplay, true);
    }

    DEFINE_TEST_CASE(TestSetActivityAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetActivityAsync);
        const string_t activityJson = testResponseJsonFromFile[L"activityJson"].serialize();
        
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(
            xboxLiveContext->MultiplayerService->SetActivityAsync(
                ref new MultiplayerSessionReference(
                        "MockScid", // serviceConfigurationId
                        "MockSessionTemplateName", // sessionTemplateName
                        "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce" // sessionName
                    )
                )
            );

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles",
            httpCall->PathQueryFragment.to_string()
            );

        auto writeJson = web::json::value::parse(activityJson);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);
    }

    DEFINE_TEST_CASE(TestClearActivityAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestClearActivityAsync);
        const string_t activitiesForUserResponseJson = testResponseJsonFromFile[L"activitiesForUserResponseJson"].serialize();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(activitiesForUserResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(
            xboxLiveContext->MultiplayerService->ClearActivityAsync(
                "MockScid" // serviceConfigurationId
                )
            ).wait();

        VERIFY_ARE_EQUAL_STR(L"DELETE", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles/7a4d0a99-4e23-4eba-9894-5173cf123fb4",
            httpCall->PathQueryFragment.to_string()
            );
    }

    DEFINE_TEST_CASE(TestSetTransferHandleAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetTransferHandleAsync);
        const string_t transferHandleJson = testResponseJsonFromFile[L"transferHandleJson"].serialize();
        const string_t transferHandleResponseJson = testResponseJsonFromFile[L"transferHandleResponseJson"].serialize();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(transferHandleResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Platform::String^ handleId = create_task(
            xboxLiveContext->MultiplayerService->SetTransferHandleAsync(
                ref new MultiplayerSessionReference(
                        "MockScid", // serviceConfigurationId
                        "MockSessionTemplateName", // sessionTemplateName
                        "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce" // sessionName
                    ),
                ref new MultiplayerSessionReference(
                        "MockScid", // serviceConfigurationId
                        "samplelobbytemplate107", // sessionTemplateName
                        "bd6c41c3-01c3-468a-a3b5-3e0fe8133862" // sessionName
                    )
                )
            ).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles",
            httpCall->PathQueryFragment.to_string()
            );

        auto writeJson = web::json::value::parse(transferHandleJson);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);

        web::json::value id = web::json::value::parse(transferHandleResponseJson)[_T("id")];
        Platform::String^ testString = ref new Platform::String(id.as_string().c_str());
        VERIFY_ARE_EQUAL_STR(handleId, testString);
    }

    DEFINE_TEST_CASE(TestGetSearchHandlesAsync_1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSearchHandlesAsync_1);
        const string_t searchHandlesRequestJson = testResponseJsonFromFile[L"searchHandlesRequestJson"].serialize();
        const string_t searchHandlesResponseJson = testResponseJsonFromFile[L"searchHandlesResponseJson"].serialize();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(searchHandlesResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto searchHandles = create_task(
            xboxLiveContext->MultiplayerService->GetSearchHandlesAsync(
                ref new Platform::String(L"MockScid"),
                ref new Platform::String(L"GlobalLFGTemplate"),
                ref new Platform::String(L"OrderBy"),
                true,
                ref new Platform::String(L"SearchQuery")
            )
        ).get();

        auto expectedJson = web::json::value::parse(searchHandlesRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_ARE_EQUAL(expectedJson.serialize(), actualJson.serialize());

        auto resultJson = web::json::value::parse(searchHandlesResponseJson);
        web::json::array searchHandlesJson = resultJson[L"results"].as_array();
        uint32 counter = 0;
        for (auto handleDetails : searchHandles)
        {
            VerifySearchHandleDetails(handleDetails, searchHandlesJson[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetSearchHandlesAsync_2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSearchHandlesAsync_2);
        const string_t searchHandlesRequestJson = testResponseJsonFromFile[L"searchHandlesRequestJson"].serialize();
        const string_t searchHandlesResponseJson = testResponseJsonFromFile[L"searchHandlesResponseJson"].serialize();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(searchHandlesResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto searchHandleRequest = ref new MultiplayerQuerySearchHandleRequest(
            ref new Platform::String(L"MockScid"),
            ref new Platform::String(L"GlobalLFGTemplate")
            );

        searchHandleRequest->OrderBy = ref new Platform::String(L"OrderBy");
        searchHandleRequest->OrderAscending = true;
        searchHandleRequest->SearchFilter = ref new Platform::String(L"SearchQuery");

        auto searchHandles = create_task(
            xboxLiveContext->MultiplayerService->GetSearchHandlesAsync(searchHandleRequest)
            ).get();

        auto expectedJson = web::json::value::parse(searchHandlesRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_ARE_EQUAL(expectedJson.serialize(), actualJson.serialize());

        auto resultJson = web::json::value::parse(searchHandlesResponseJson);
        web::json::array searchHandlesJson = resultJson[L"results"].as_array();
        uint32 counter = 0;
        for (auto handleDetails : searchHandles)
        {
            VerifySearchHandleDetails(handleDetails, searchHandlesJson[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetSearchHandlesAsync_3)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSearchHandlesAsync_3);
        const string_t searchHandlesWithSocialGroupRequestJson = testResponseJsonFromFile[L"searchHandlesWithSocialGroupRequestJson"].serialize();
        const string_t searchHandlesResponseJson = testResponseJsonFromFile[L"searchHandlesResponseJson"].serialize();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(searchHandlesResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto searchHandleRequest = ref new MultiplayerQuerySearchHandleRequest(
            ref new Platform::String(L"MockScid"),
            ref new Platform::String(L"GlobalLFGTemplate")
        );

        searchHandleRequest->OrderBy = ref new Platform::String(L"OrderBy");
        searchHandleRequest->OrderAscending = true;
        searchHandleRequest->SearchFilter = ref new Platform::String(L"SearchQuery");
        searchHandleRequest->SocialGroup = ref new Platform::String(L"favorites");

        auto searchHandles = create_task(
            xboxLiveContext->MultiplayerService->GetSearchHandlesAsync(searchHandleRequest)
        ).get();

        auto expectedJson = web::json::value::parse(searchHandlesWithSocialGroupRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_ARE_EQUAL(expectedJson.serialize(), actualJson.serialize());

        auto resultJson = web::json::value::parse(searchHandlesResponseJson);
        web::json::array searchHandlesJson = resultJson[L"results"].as_array();
        uint32 counter = 0;
        for (auto handleDetails : searchHandles)
        {
            VerifySearchHandleDetails(handleDetails, searchHandlesJson[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestSetSearchHandleAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSetSearchHandleAsync);
        const string_t searchHandleJson = testResponseJsonFromFile[L"searchHandleJson"].serialize();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        auto mpSearchHandleRequest = ref new MultiplayerSearchHandleRequest(
            ref new MultiplayerSessionReference(
                "MockScid",
                "GlobalLFGSessionTemplateName",
                "LFGSession"
            )
        );

        auto tags = ref new Vector<Platform::String^>();
        tags->Append(L"micsrequired");
        tags->Append(L"girlsonly");
        mpSearchHandleRequest->Tags = tags->GetView();

        auto numbersMetaMap = ref new Platform::Collections::Map<Platform::String^, double>();
        numbersMetaMap->Insert(L"Skill_D", 10.145);
        numbersMetaMap->Insert(L"Skill_I", 14);
        mpSearchHandleRequest->NumbersMetadata = numbersMetaMap->GetView();

        auto stringsMetaMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
        stringsMetaMap->Insert(L"Class", L"A");
        mpSearchHandleRequest->StringsMetadata = stringsMetaMap->GetView();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(xboxLiveContext->MultiplayerService->SetSearchHandleAsync(mpSearchHandleRequest));

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles",
            httpCall->PathQueryFragment.to_string()
        );

        auto writeJson = web::json::value::parse(searchHandleJson);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()).data());
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);
    }

    DEFINE_TEST_CASE(TestClearSearchHandleAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestClearActivityAsync);
        const string_t searchHandleJson = testResponseJsonFromFile[L"searchHandleJson"].serialize();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(searchHandleJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(xboxLiveContext->MultiplayerService->ClearSearchHandleAsync("TestHandleId")).wait();

        VERIFY_ARE_EQUAL_STR(L"DELETE", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://sessiondirectory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/handles/TestHandleId",
            httpCall->PathQueryFragment.to_string()
        );
    }

    DEFINE_TEST_CASE(TestSendInvitesAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSendInvitesAsync);
        const string_t inviteRequestJson = testResponseJsonFromFile[L"inviteRequestJson"].serialize();
        const string_t inviteResponseJson = testResponseJsonFromFile[L"inviteResponseJson"].serialize();

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(inviteResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Vector<Platform::String^>^ xuids = ref new Vector<Platform::String^>();
        xuids->Append(L"1234");
        xuids->Append(L"5678");

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        IVectorView<Platform::String^>^ invites = create_task(
            xboxLiveContext->MultiplayerService->SendInvitesAsync(
                ref new MultiplayerSessionReference(
                    "MockScid", // serviceConfigurationId
                    "MockSessionTemplateName", // sessionTemplateName
                    "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce" // sessionName
                    ),
                xuids->GetView(),
                1018096776
                )
            ).get();

        auto expectedJson = web::json::value::parse(inviteRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));

        VERIFY_IS_EQUAL_JSON(expectedJson, actualJson);

        web::json::value id = web::json::value::parse(inviteResponseJson)[_T("id")];
        Platform::String^ testString = ref new Platform::String(id.as_string().c_str());
        VERIFY_ARE_EQUAL_STR(invites->GetAt(1), testString);
    }

    DEFINE_TEST_CASE(TestGetActivitiesForUsersAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetActivitiesForUsersAsync);
        const string_t activitiesForUserRequestJson = testResponseJsonFromFile[L"activitiesForUserRequestJson"].serialize();
        const string_t activitiesForUserResponseJson = testResponseJsonFromFile[L"activitiesForUserResponseJson"].serialize();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(activitiesForUserResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Vector<Platform::String^>^ xuids = ref new Vector<Platform::String^>();
        xuids->Append(L"TestXboxUserId");

        auto activities = create_task(
            xboxLiveContext->MultiplayerService->GetActivitiesForUsersAsync(
                ref new Platform::String(L"MockScid"),
                xuids->GetView()
                )
            ).get();
        
        auto expectedJson = web::json::value::parse(activitiesForUserRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_ARE_EQUAL(expectedJson.serialize(), actualJson.serialize());

        auto resultJson = web::json::value::parse(activitiesForUserResponseJson);
        web::json::array keywordsJson = resultJson[L"results"].as_array();
        uint32 counter = 0;
        for (auto activity : activities)
        {
            VerifyActivityDetails(activity, keywordsJson[counter]);
            ++counter;
        }
    }

    DEFINE_TEST_CASE(TestGetActivitiesForSocialGroupAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetActivitiesForSocialGroupAsync);
        const string_t activitiesForSocialGroupRequestJson = testResponseJsonFromFile[L"activitiesForSocialGroupRequestJson"].serialize();
        const string_t activitiesForSocialGroupResponseJson = testResponseJsonFromFile[L"activitiesForSocialGroupResponseJson"].serialize();

        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto responseJson = web::json::value::parse(activitiesForSocialGroupResponseJson);
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        
        auto activities = create_task(
            xboxLiveContext->MultiplayerService->GetActivitiesForSocialGroupAsync(
                ref new Platform::String(L"MockScid"),
                L"TestXboxUserId",
                L"friends"
                )
            ).get();

        auto expectedJson = web::json::value::parse(activitiesForSocialGroupRequestJson);
        auto actualJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        VERIFY_ARE_EQUAL(expectedJson.serialize(), actualJson.serialize());
    }

    DEFINE_TEST_CASE(TestCompareMultiplayerSessions)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCompareMultiplayerSessions);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto oldSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        oldSession->SetClosed(true);
        oldSession->SetLocked(true);
        oldSession->SetSessionCustomPropertyJson(L"hello", L"goodbye");
        oldSession->Join();

        input.HostDeviceToken = L"12345";
        auto sessionResult = multiplayer_session::_Deserialize(
            web::json::value::parse(defaultMultiplayerResponse)
            );
        VERIFY_IS_TRUE(!sessionResult.err());
        auto currentSession = ref new MultiplayerSession(
            std::make_shared<multiplayer_session>(
                sessionResult.payload()
                )
            );

        uint32 changeType = static_cast<uint32>(MultiplayerSession::CompareMultiplayerSessions(oldSession, currentSession));

        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberListChange) & changeType) == MultiplayerSessionChangeTypes::MemberListChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::HostDeviceTokenChange) & changeType) == MultiplayerSessionChangeTypes::HostDeviceTokenChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::InitializationStateChange) & changeType) == MultiplayerSessionChangeTypes::InitializationStateChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MatchmakingStatusChange) & changeType) == MultiplayerSessionChangeTypes::MatchmakingStatusChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::SessionJoinabilityChange) & changeType) == MultiplayerSessionChangeTypes::SessionJoinabilityChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::CustomPropertyChange) & changeType) != MultiplayerSessionChangeTypes::CustomPropertyChange);
        
        currentSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        currentSession->Join();
        oldSession->SetCurrentUserMemberCustomPropertyJson(L"hello", L"goodbye");
        oldSession->SetCurrentUserStatus(MultiplayerSessionMemberStatus::Active);

        changeType = static_cast<uint32>(MultiplayerSession::CompareMultiplayerSessions(oldSession, currentSession));
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberStatusChange) & changeType) == MultiplayerSessionChangeTypes::MemberStatusChange);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberCustomPropertyChange) & changeType) != MultiplayerSessionChangeTypes::MemberCustomPropertyChange);
    
        oldSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        oldSession->SetClosed(true);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::SessionJoinabilityChange) & changeType) != MultiplayerSessionChangeTypes::CustomPropertyChange);

        oldSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        oldSession->SetLocked(true);
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::SessionJoinabilityChange) & changeType) != MultiplayerSessionChangeTypes::CustomPropertyChange);

        oldSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        currentSession = TestCreateSessionHelper(input, GetMockXboxLiveContext_WinRT());
        changeType = static_cast<uint32>(MultiplayerSession::CompareMultiplayerSessions(oldSession, currentSession));

        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberListChange) & changeType) == MultiplayerSessionChangeTypes::MemberListChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::HostDeviceTokenChange) & changeType) == MultiplayerSessionChangeTypes::HostDeviceTokenChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::InitializationStateChange) & changeType) == MultiplayerSessionChangeTypes::InitializationStateChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MatchmakingStatusChange) & changeType) == MultiplayerSessionChangeTypes::MatchmakingStatusChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::SessionJoinabilityChange) & changeType) == MultiplayerSessionChangeTypes::SessionJoinabilityChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::CustomPropertyChange) & changeType) == MultiplayerSessionChangeTypes::CustomPropertyChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberStatusChange) & changeType) == MultiplayerSessionChangeTypes::MemberStatusChange);
        VERIFY_IS_FALSE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MemberCustomPropertyChange) & changeType) == MultiplayerSessionChangeTypes::MemberCustomPropertyChange);

        // Test MatchmakingStatusChange for different target session refs.
        currentSession = ref new MultiplayerSession(
            std::make_shared<multiplayer_session>(
                sessionResult.payload()
                )
            );

        const string_t responseForComparingSessions = testResponseJsonFromFile[L"MultiplayerResponseForComparingSessions"].serialize();
        auto compareSessionResult = multiplayer_session::_Deserialize(
            web::json::value::parse(responseForComparingSessions)
            );
        VERIFY_IS_TRUE(!compareSessionResult.err());
        auto compareSession = ref new MultiplayerSession(
            std::make_shared<multiplayer_session>(
                compareSessionResult.payload()
                )
            );

        changeType = static_cast<uint32>(MultiplayerSession::CompareMultiplayerSessions(currentSession, compareSession));
        VERIFY_IS_TRUE(static_cast<MultiplayerSessionChangeTypes>(static_cast<uint32>(MultiplayerSessionChangeTypes::MatchmakingStatusChange) & changeType) == MultiplayerSessionChangeTypes::MatchmakingStatusChange);
    }

    DEFINE_TEST_CASE(TestRTAMultiplayer)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTAMultiplayer);

        const int subId = 666;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TimeSpan ts;
        ts.Duration = 1000;
        xboxLiveContext->Settings->WebsocketTimeoutWindow = ts;
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, rtaConnectionIdJson, subId);

        xboxLiveContext->MultiplayerService->EnableMultiplayerSubscriptions();
        VERIFY_IS_TRUE(xboxLiveContext->MultiplayerService->MultiplayerSubscriptionsEnabled);

        auto session = ref new MultiplayerSession(xboxLiveContext);
        session->Join();

        // Verify it should fail before RTA activated
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(session, MultiplayerSessionWriteMode::CreateNew)).get(),
            E_XBL_RUNTIME_ERROR
            );

        // Connect RTA
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();
        TEST_LOG(L"Activate() connected event triggered");

        bool didFire = false;

        //Make websocket auto reconnect
        // Write session on connection, make sure it can finish once connected
        mockSocket->m_waitForSignal = true;
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        helper->connectingEvent.wait();
        TEST_LOG(L"Auto-reconnecting event triggered");

        concurrency::event writeSessionEvent;
        create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(session, MultiplayerSessionWriteMode::CreateNew))
        .then([&writeSessionEvent](MultiplayerSession^)
        {
            writeSessionEvent.set();
        });
        mockSocket->m_connectEvent.set();
        helper->connectedEvent.wait();
        TEST_LOG(L"Auto-reconnected event triggered");
        writeSessionEvent.wait();
        TEST_LOG(L"writeSessionEvent triggered");

        // Normal case when connected
        create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(session,MultiplayerSessionWriteMode::CreateNew)).get();
        // fire event, make sure received
        didFire = false;
        bool didLostFire = false;
        const string_t rtaSessionUpdateJson = testResponseJsonFromFile[L"rtaSessionUpdateJson"].serialize();
        auto parsedSessionChangedJson = web::json::value::parse(rtaSessionUpdateJson);
        auto sessionChangeEvent = xboxLiveContext->MultiplayerService->MultiplayerSessionChanged +=
            ref new Windows::Foundation::EventHandler<MultiplayerSessionChangeEventArgs^>([this, &didFire, &parsedSessionChangedJson](Platform::Object^, MultiplayerSessionChangeEventArgs^ args)
        {
            didFire = true;
            auto splitString = utils::string_split(parsedSessionChangedJson[_T("shoulderTaps")].as_array()[0][_T("resource")].as_string(), '~');
            
            VERIFY_ARE_EQUAL(args->SessionReference->ServiceConfigurationId->Data(), splitString[0]);
            VERIFY_ARE_EQUAL(args->SessionReference->SessionTemplateName->Data(), splitString[1]);
            VERIFY_ARE_EQUAL(args->SessionReference->SessionName->Data(), splitString[2]);
            VERIFY_ARE_EQUAL_INT(args->ChangeNumber, parsedSessionChangedJson[_T("shoulderTaps")].as_array()[0][_T("changeNumber")].as_integer());
            VERIFY_ARE_EQUAL(args->Branch->Data(), parsedSessionChangedJson[_T("shoulderTaps")].as_array()[0][_T("branch")].as_string());
        });

        auto subscriptionLostEvent = xboxLiveContext->MultiplayerService->MultiplayerSubscriptionLost +=
            ref new Windows::Foundation::EventHandler<MultiplayerSubscriptionLostEventArgs^>([&didLostFire](Platform::Object^, MultiplayerSubscriptionLostEventArgs^ args)
        {
            didLostFire = true;
        });

        mockSocket->receive_rta_event(subId, rtaSessionUpdateJson);
        VERIFY_IS_TRUE(didFire);

        //Make websocket auto reconnect
        //force disconnect socket, should auto reconnect
        didFire = false;

        auto mockSubscription = m_mockXboxSystemFactory->GetMocckMultiplayerSubscription();
        concurrency::event subscribedEvent;
        mockSubscription->OnSetState([&subscribedEvent](real_time_activity_subscription_state state)
        {
            if (real_time_activity_subscription_state::subscribed == state)
            {
                subscribedEvent.set();
            }
        });

        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        helper->connectingEvent.wait();
        helper->connectedEvent.wait();
        create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(session,MultiplayerSessionWriteMode::CreateNew)).get();

        // wait for subscription to be resubscribed.
        subscribedEvent.wait();
        mockSocket->receive_rta_event(subId, rtaSessionUpdateJson);
        VERIFY_IS_TRUE(didFire);

        //force disconnect socket
        didFire = false;
        mockSocket->m_waitForSignal = false;
        mockSocket->m_connectToFail = true;
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        helper->disconnectedEvent.wait();

        // should not be able to receive 
        mockSocket->receive_rta_event(subId, rtaSessionUpdateJson);
        VERIFY_IS_FALSE(didFire);

        // make it reconnect
        helper->reset_events();
        mockSocket->m_connectToFail = false;
        helper->connectedEvent.wait();

        // subscription should be dead, still not receiving 
        mockSocket->receive_rta_event(subId, rtaSessionUpdateJson);
        VERIFY_IS_FALSE(didFire);

        mockSocket->close();
        VERIFY_IS_TRUE(didLostFire);
    }

    DEFINE_TEST_CASE(TestRTADisableMultiplayerSubscriptions)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTADisableMultiplayerSubscriptions);
        const int subId = 666;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TimeSpan ts;
        ts.Duration = 1000;
        xboxLiveContext->Settings->WebsocketTimeoutWindow = ts;
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, rtaConnectionIdJson, subId);

        auto session = ref new MultiplayerSession(xboxLiveContext);
        session->Join();

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();

        xboxLiveContext->MultiplayerService->EnableMultiplayerSubscriptions();
        VERIFY_IS_TRUE(xboxLiveContext->MultiplayerService->MultiplayerSubscriptionsEnabled);

        // fire event, make sure received
        bool didLostFire = false;
        concurrency::event fireEvent;
        auto subscriptionLostEvent = xboxLiveContext->MultiplayerService->MultiplayerSubscriptionLost +=
            ref new Windows::Foundation::EventHandler<MultiplayerSubscriptionLostEventArgs^>([&didLostFire, &fireEvent](Platform::Object^, MultiplayerSubscriptionLostEventArgs^ args)
        {
            didLostFire = true;
            fireEvent.set();
        });

        xboxLiveContext->MultiplayerService->DisableMultiplayerSubscriptions();

        fireEvent.wait();
        VERIFY_IS_TRUE(didLostFire);
    }

    DEFINE_TEST_CASE(TestMultiplayerSubscribeChangeTypes)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultiplayerSubscribeChangeTypes);
        const string_t setSessionChangeTypesJson = testResponseJsonFromFile[L"setSessionChangeTypesJson"].serialize();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto session = ref new MultiplayerSession(xboxLiveContext);
        VERIFY_ARE_EQUAL_INT(session->SubscribedChangeTypes, MultiplayerSessionChangeTypes::None);

        session->Join();

        VERIFY_ARE_EQUAL_INT(session->SubscribedChangeTypes, MultiplayerSessionChangeTypes::None);

        auto changeSubscription = MultiplayerSessionChangeTypes::CustomPropertyChange |
            MultiplayerSessionChangeTypes::HostDeviceTokenChange |
            MultiplayerSessionChangeTypes::InitializationStateChange |
            MultiplayerSessionChangeTypes::MatchmakingStatusChange |
            MultiplayerSessionChangeTypes::MemberCustomPropertyChange |
            MultiplayerSessionChangeTypes::MemberListChange |
            MultiplayerSessionChangeTypes::MemberStatusChange |
            MultiplayerSessionChangeTypes::SessionJoinabilityChange;

        session->SetSessionChangeSubscription(
            changeSubscription
            );

        VERIFY_ARE_EQUAL_INT(session->SubscribedChangeTypes, changeSubscription);

        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto result = create_task(
            xboxLiveContext->MultiplayerService->WriteSessionAsync(
            session,
            MultiplayerSessionWriteMode::UpdateExisting
            )
            ).get();

        auto writeJson = web::json::value::parse(setSessionChangeTypesJson);
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        requestJson[_T("members")][_T("me")][_T("properties")][_T("system")][_T("subscription")][_T("id")] = web::json::value::null();    // Can't test the id is a GUID
        VERIFY_IS_EQUAL_JSON(writeJson, requestJson);
    }

    DEFINE_TEST_CASE(TestHttpCalls)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpCalls);
        const string_t emptyJson = testResponseJsonFromFile[L"emptyJson"].serialize();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto session = ref new MultiplayerSession(xboxLiveContext);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(emptyJson), 204);

        session->Join();

        VERIFY_IS_NULL(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(
                session,
                MultiplayerSessionWriteMode::UpdateOrCreateNew
                )).get()
            );

        VERIFY_THROWS_HR_CX(
            create_task(
                xboxLiveContext->MultiplayerService->GetCurrentSessionAsync(
                    ref new MultiplayerSessionReference(
                        L"8d050174-412b-4d51-a29b-d55a34edfdb7",
                        L"integration",
                        L"19de0095d8bb41048f19edbbb6bc6b04"
                        )
                    )
                ).get(),
            __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
        );
    }

    DEFINE_TEST_CASE(TestMultiplayerInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultiplayerInvalidArgs);
        MultiplayerSessionTestCreateInput input = GetDefaultMultiplayerSessionTestCreateInput();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TEST_LOG(L"TestMultiplayerInvalidArgs: MultiplayerSession: Test invalid xboxLiveContext");
        /*VERIFY_THROWS_HR_CX(
            ref new MultiplayerSession(
                nullptr,
                input.multiplayerSessionReference,
                input.maxMembersInSession,
                input.clientMatchmakingCapable,
                input.multiplayerSessionVisibility,
                input.initiatorXboxUserIds,
                TestDataToJson(input.sessionCustomConstantsJson)
                ),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: MultiplayerSession: Test invalid sessionRef");
        VERIFY_THROWS_HR_CX(
            ref new MultiplayerSession(
                xboxLiveContext,
                nullptr,
                input.maxMembersInSession,
                input.clientMatchmakingCapable,
                input.multiplayerSessionVisibility,
                input.initiatorXboxUserIds,
                TestDataToJson(input.sessionCustomConstantsJson)
                ),
            E_INVALIDARG
            );*/

        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionAsync: Test invalid session");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(nullptr, MultiplayerSessionWriteMode::CreateNew)).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionAsync: Test invalid write mode");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(
                nullptr,
                static_cast<MultiplayerSessionWriteMode>((int)MultiplayerSessionWriteMode::UpdateOrCreateNew + 1)   // invalid arg
                )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: GetCurrentSessionAsync: Test invalid sessionRef");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetCurrentSessionAsync(nullptr)).get(),
            E_INVALIDARG
            );

        auto getSessionsRequest = ref new MultiplayerGetSessionsRequest(
            "foo",
            100
            );

        getSessionsRequest->SessionTemplateNameFilter = "bar";
        getSessionsRequest->XboxUserIdFilter = "123456";
        getSessionsRequest->VisibilityFilter = MultiplayerSessionVisibility::Unknown;   // visibility (should never be passed in as Unknown)
        getSessionsRequest->IncludeInactiveSessions = true;
        getSessionsRequest->IncludeReservations = true;
        getSessionsRequest->IncludePrivateSessions = true;

        TEST_LOG(L"TestMultiplayerInvalidArgs: GetSessionsAsync: Test invalid MultiplayerSessionVisibility");

#pragma warning(suppress: 6387)
        getSessionsRequest->XboxUserIdFilter = nullptr;// xuid (either xuid or keywords needed)
#pragma warning(suppress: 6387)
        getSessionsRequest->KeywordFilter = nullptr;
        getSessionsRequest->VisibilityFilter = MultiplayerSessionVisibility::Any;   // visibility (should never be passed in as Unknown)

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetSessionsAsync(
                getSessionsRequest
                )).get(),
            E_INVALIDARG
            );

        getSessionsRequest->KeywordFilter = "123456";
        getSessionsRequest->IncludeInactiveSessions = true; // includeInactiveSessions (must have xuid)
        getSessionsRequest->IncludeReservations = false;
        getSessionsRequest->IncludePrivateSessions = false;
        TEST_LOG(L"TestMultiplayerInvalidArgs: GetSessionsAsync: Test invalid includeInactiveSessions");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetSessionsAsync(
                getSessionsRequest
                )).get(),
            E_INVALIDARG
            );

        getSessionsRequest->IncludeInactiveSessions = false;
        getSessionsRequest->IncludeReservations = true; // includeReservedSessions (must have xuid)
        getSessionsRequest->IncludePrivateSessions = false;
        TEST_LOG(L"TestMultiplayerInvalidArgs: GetSessionsAsync: Test invalid includeReservedSessions");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetSessionsAsync(
                getSessionsRequest
                )).get(),
            E_INVALIDARG
            );

        // GetSessionByHandle and WriteSessionByHandle
        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionByHandleAsync: Test null session");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionByHandleAsync(
                nullptr, 
                MultiplayerSessionWriteMode::CreateNew, 
                L"handle"
                )).get(),
                E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionByHandleAsync: Test invalid writemode");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionByHandleAsync(
                ref new MultiplayerSession(xboxLiveContext), 
                (MultiplayerSessionWriteMode)99, 
                L"handle"
                )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionByHandleAsync: Test null handle");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionByHandleAsync(
                ref new MultiplayerSession(xboxLiveContext),
                (MultiplayerSessionWriteMode)99,
                nullptr
                )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: WriteSessionByHandleAsync: Test empty handle");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionByHandleAsync(
                ref new MultiplayerSession(xboxLiveContext),
                (MultiplayerSessionWriteMode)99, 
                L""
                )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: GetCurrentSessionByHandleAsync: Test null handleId");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetCurrentSessionByHandleAsync(
                nullptr
                )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestMultiplayerInvalidArgs: GetCurrentSessionByHandleAsync: Test empty handleId");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetCurrentSessionByHandleAsync(
                ""
                )).get(),
            E_INVALIDARG
            );

        Platform::String^ defaultScid = ref new Platform::String(L"1110");

        auto xuidVector = ref new Vector<Platform::String^>();
        xuidVector->Append("1234");

        TEST_LOG(L"TestGetActivitiesForUsersInvalidArgs: null scid");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetActivitiesForUsersAsync(nullptr, xuidVector->GetView())).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetActivitiesForUsersInvalidArgs: empty scid");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetActivitiesForUsersAsync("", xuidVector->GetView())).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetActivitiesForUsersInvalidArgs: null xuids");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetActivitiesForUsersAsync(defaultScid, nullptr)).get(),
            E_INVALIDARG
            );

        xuidVector = ref new Vector<Platform::String^>();
        TEST_LOG(L"TestGetActivitiesForUsersInvalidArgs: empty xuids");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->GetActivitiesForUsersAsync(defaultScid, xuidVector->GetView())).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetActivitiesForSocialGroupInvalidArgs: null scid");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SetActivityAsync(nullptr)).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetActivitiesForSocialGroupInvalidArgs: empty scid");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->ClearActivityAsync("")).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestClearActivityInvalidArgs: null scid");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SetActivityAsync(nullptr)).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestClearActivityInvalidArgs: empty scid");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->ClearActivityAsync("")).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestSetActivityInvalidArgs: null sessionreference");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SetActivityAsync(nullptr)).get(),
            E_INVALIDARG
            );

        MultiplayerSessionReference^ testReference = ref new MultiplayerSessionReference(
            "0001", 
            "testTemplate", 
            "testSessionName"
            );
        Vector<Platform::String^>^ xuidVectorEmpty = ref new Vector<Platform::String^>();

        TEST_LOG(L"TestSendInvitesInvalidArgs: null xuids vector");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SendInvitesAsync(testReference, nullptr, 1234)).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestSendInvitesInvalidArgs: empty xuids vector");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SendInvitesAsync(testReference, xuidVectorEmpty->GetView(), 1234)).get(),
            E_INVALIDARG
            );

        xuidVector->Append("4567");
        TEST_LOG(L"TestSendInvitesInvalidArgs: null sessionref");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->SendInvitesAsync(nullptr, xuidVectorEmpty->GetView(), 1234)).get(),
            E_INVALIDARG
            );

        auto session = ref new MultiplayerSession(xboxLiveContext);
        session->Join();
        auto timeSpan = Windows::Foundation::TimeSpan();
        timeSpan.Duration = 10;

        TimeSpan ts;
        ts.Duration = 1000;

        TEST_LOG(L"TestRTAConnectionFailed: uninitialized RTA");

        const int subId = 666;
        xboxLiveContext->Settings->WebsocketTimeoutWindow = ts;
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, rtaConnectionIdJson, subId);

        xboxLiveContext->MultiplayerService->EnableMultiplayerSubscriptions();

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MultiplayerService->WriteSessionAsync(
                session,
                MultiplayerSessionWriteMode::UpdateOrCreateNew
                )).get(),
            E_XBL_RUNTIME_ERROR
            );

        TEST_LOG(L"TestRTAConnectionFailed: Already enabled");
        VERIFY_THROWS_HR_CX(
            xboxLiveContext->MultiplayerService->EnableMultiplayerSubscriptions(),
            E_UNEXPECTED
            );

        TEST_LOG(L"Testing Setting Session Constants Post Write");
        session = GetCurrentSessionAsyncHelper();

        VERIFY_THROWS_HR_CX(
            session->SetTimeouts(
                timeSpan,
                timeSpan,
                timeSpan,
                timeSpan
                ),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            session->SetQualityOfServiceConnectivityMetrics(
                true,
                true,
                true,
                true
                ),
            E_UNEXPECTED
            );

        ts.Duration = 1;
#pragma warning(suppress: 4973)
        VERIFY_THROWS_HR_CX(
            session->SetManagedInitialization(
                ts,
                ts,
                ts,
                false,
                1
                ),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            session->SetMemberInitialization(
                ts,
                ts,
                ts,
                false,
                1
                ),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            session->SetPeerToPeerRequirements(
                ts,
                1000
                ),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            session->SetPeerToHostRequirements(
                ts,
                1000,
                1000,
                MultiplayMetrics::Bandwidth
                ),
            E_UNEXPECTED
            );

        auto qosList = ref new Vector<Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^>();
        qosList->Append(ref new Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer(
            xbox::services::game_server_platform::quality_of_service_server(
            _T("asdfasdf"),
            _T("1234345346"),
            _T("123421345")
            )));

        VERIFY_THROWS_HR_CX(
            session->SetMeasurementServerAddresses(
                qosList->GetView()
                ),
            E_UNEXPECTED
            );

        auto capabilites = ref new MultiplayerSessionCapabilities(xbox::services::multiplayer::multiplayer_session_capabilities());
        VERIFY_THROWS_HR_CX(
            session->SetSessionCapabilities(
                capabilites
                ),
            E_UNEXPECTED
            );

        session->Join();

        VERIFY_THROWS_HR_CX(
            session->Leave(),
            E_UNEXPECTED
            );

        VERIFY_THROWS_HR_CX(
            session->Join(),
            E_UNEXPECTED
            );
    }

    DEFINE_TEST_CASE(TestTournamentSession)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestTournamentSession);
        const string_t defaultTournamentSessionResponse = testResponseJsonFromFile[L"defaultTournamentSessionResponse"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper(defaultTournamentSessionResponse);
        VERIFY_IS_TRUE(currentSession->ArbitrationStatus == TournamentArbitrationStatus::InProgress);

        auto tournamentServer = currentSession->TournamentsServer;
        VERIFY_IS_NOT_NULL(tournamentServer);
        VERIFY_IS_TRUE(tournamentServer->RegistrationState == TournamentRegistrationState::Registered);
        VERIFY_IS_TRUE(tournamentServer->RegistrationReason == TournamentRegistrationReason::TournamentCompleted);
        VERIFY_IS_TRUE(tournamentServer->LastGameResultSource == TournamentGameResultSource::Arbitration);

        VERIFY_IS_TRUE(tournamentServer->LastGameEndTime.UniversalTime != 0);
        VERIFY_IS_TRUE(tournamentServer->NextGameStartTime.UniversalTime != 0);

        auto arbitrationServer = currentSession->ArbitrationServer;
        VERIFY_IS_NOT_NULL(arbitrationServer);

        VERIFY_IS_TRUE(arbitrationServer->ArbitrationStartTime.UniversalTime != 0);
        VERIFY_IS_TRUE(arbitrationServer->ResultState == TournamentArbitrationState::PartialResults);
        VERIFY_IS_TRUE(arbitrationServer->ResultSource == TournamentGameResultSource::Adjusted);
        VERIFY_IS_TRUE(arbitrationServer->ResultConfidenceLevel == 95);
        VERIFY_IS_TRUE(arbitrationServer->Results->Size == 2);
        VERIFY_IS_TRUE(arbitrationServer->Results->HasKey("team1"));
        TournamentTeamResult^ result = arbitrationServer->Results->Lookup("team1");
        VERIFY_IS_TRUE(result->State == TournamentGameResultState::Rank);
        VERIFY_IS_TRUE(result->Ranking == 3);
        VERIFY_IS_TRUE(arbitrationServer->Results->HasKey("team2"));
        result = arbitrationServer->Results->Lookup("team2");
        VERIFY_IS_TRUE(result->State == TournamentGameResultState::Rank);
        VERIFY_IS_TRUE(result->Ranking == 2);

        auto nextGameSessionRef = tournamentServer->NextGameSessionRef;
        VERIFY_IS_NOT_NULL(nextGameSessionRef);
        VERIFY_ARE_EQUAL_STR(nextGameSessionRef->ServiceConfigurationId, "foo");
        VERIFY_ARE_EQUAL_STR(nextGameSessionRef->SessionTemplateName, "bar");
        VERIFY_ARE_EQUAL_STR(nextGameSessionRef->SessionName, "session-seven");

        for (const auto& member : currentSession->Members)
        {
            VERIFY_IS_NOT_NULL(member->Results);
            VERIFY_IS_TRUE(member->Results->Size == 2);
            for (auto result2 : member->Results)
            {
                Platform::String^ key = result2->Key;
                TournamentTeamResult^ teamResult = result2->Value;
                VERIFY_IS_TRUE(key->Equals("team1") || key->Equals("team2"));
                VERIFY_IS_TRUE(teamResult->State == TournamentGameResultState::Rank);
                VERIFY_IS_TRUE(teamResult->Ranking == 3 || teamResult->Ranking == 2);
            }

            VERIFY_IS_TRUE(member->ArbitrationStatus == TournamentArbitrationStatus::Joining);

            auto teamSessionRef = member->TournamentTeamSessionRef;
            VERIFY_IS_NOT_NULL(teamSessionRef);
            VERIFY_ARE_EQUAL_STR(teamSessionRef->ServiceConfigurationId, "TestScid");
            VERIFY_ARE_EQUAL_STR(teamSessionRef->SessionTemplateName, "TournamentGameSessionTest");
            VERIFY_ARE_EQUAL_STR(teamSessionRef->SessionName, "TestName");
        }
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserArbitrationResult)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetCurrentUserArbitrationResult);
        const string_t arbitrationResultRankJson = testResponseJsonFromFile[L"arbitrationResultRankJson"].serialize();
        const string_t arbitrationResultJson = testResponseJsonFromFile[L"arbitrationResultJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        TournamentTeamResult^ resultRank = ref new TournamentTeamResult();
        resultRank->Ranking = 3;
        auto resultMap = ref new Platform::Collections::Map<Platform::String^, TournamentTeamResult^>();
        resultMap->Insert(L"team1", resultRank);
        currentSession->SetCurrentUserArbitrationResults(
            resultMap->GetView()
            );
        WriteSessionAsyncHelper(currentSession, arbitrationResultRankJson);

        TournamentTeamResult^ result = ref new TournamentTeamResult();
        result->State = TournamentGameResultState::Loss;
        resultMap->Insert(L"team1", result);
        currentSession->SetCurrentUserArbitrationResults(
            resultMap->GetView()
            );
        WriteSessionAsyncHelper(currentSession, arbitrationResultJson);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMutableRoleSettings)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestWriteSessionAsyncWithSetMutableRoleSettings);
        const string_t roleTypesRequestJson = testResponseJsonFromFile[L"roleTypesRequestJson"].serialize();
        auto currentSession = GetCurrentSessionAsyncHelper();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        MultiplayerRoleType^ lfgRoleType = ref new MultiplayerRoleType();
        MultiplayerRoleType^ squadRoleType = ref new MultiplayerRoleType();

        MultiplayerRoleInfo^ friendRole = ref new MultiplayerRoleInfo();
        friendRole->MaxMembersCount = 5;
        friendRole->TargetCount = 3;
        MultiplayerRoleInfo^ otherRole = ref new MultiplayerRoleInfo();
        otherRole->MaxMembersCount = 5;

        auto rolesMap = ref new Platform::Collections::Map<Platform::String^, MultiplayerRoleInfo^>();
        rolesMap->Insert(L"friend", friendRole);
        rolesMap->Insert(L"other", otherRole);
        lfgRoleType->Roles = rolesMap->GetView();
        squadRoleType->Roles = rolesMap->GetView();

        auto roleTypesMap = ref new Platform::Collections::Map<Platform::String^, MultiplayerRoleType^>();
        roleTypesMap->Insert(L"lfg", lfgRoleType);
        roleTypesMap->Insert(L"squad", squadRoleType);

        currentSession->SetMutableRoleSettings(roleTypesMap->GetView());
        WriteSessionAsyncHelper(currentSession, roleTypesRequestJson);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
