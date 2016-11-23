//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once 
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Represents managed initialization for a session.
/// </summary>
/// <remarks>
/// If using managed initialization, the session expects the system or the title to perform initialization
/// following the session creation and/or as new members join the session. The session automatically tracks
/// the timeouts and initialization stages, including QoS measurements if any metrics are set.
/// These timeouts override the session's reservation and ready timeouts for members that have the
/// MultiplayerSessionMember.InitializationEpisode Property set. 
///
/// In managed initialization, the initialization stage goes from "joining" to "measuring" to "evaluating". 
/// If the MultiplayerManagedInitialization.AutoEvaluate Property is set to true, the evaluating stage is skipped.
/// If metrics are not set for the session, the measuring stage is skipped. 
/// If the stage is set to "failed", the session cannot be initialized. 
/// </remarks>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "managedInitialization": {
///     "joinTimeout": 4000,
///     "measurementTimeout" : 5000,
///     "evaluationTimeout" : 5000,
///     "autoEvaluate" : true,
///     "membersNeededToStart" : 2
/// },
/// </code>
/// </example>

public ref class MultiplayerManagedInitialization sealed
{
public:
    /// <summary>
    /// Indicates if the ManagedInitializion object is set, which means that the session is using
    /// managed initialization.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ManagedInitializationSet, managed_initialization_set, bool);
    
    /// <summary>
    /// The period of time that the system waits for a member to join a session during the joining stage 
    /// of managed initialization. This stage is the first stage of the QoS process.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(JoinTimeout, join_timeout)

    /// <summary>
    /// The period of time that the system waits for a measuring operation to occur during managed initialization.
    /// This stage is the measurement stage of the QoS process. 
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(MeasurementTimeout, measurement_timeout)

    /// <summary>
    /// The period of time that the Xbox system waits for evaluation to occur during managed initialization. 
    /// Returns the timeout for the evaluation stage of the QoS process. 
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(EvaluationTimeout, evaluation_timeout)

    /// <summary>
    /// A value that indicates if the system should auto-evaluate during managed initialization. 
    /// This is an optional evaluate stage for a title. The title can do evaluation when set to false.
    /// </summary>
    DEFINE_PROP_GET_OBJ(AutoEvaluate, auto_evaluate, bool);

    /// <summary>
    /// The number of session members needed to start a session, during managed initialization episode 0 only. 
    /// Defaults to 2. This number must be between 1 and maxMemberCount.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MembersNeededToStart, members_need_to_start, uint32);

internal:
    MultiplayerManagedInitialization(
        _In_ xbox::services::multiplayer::multiplayer_managed_initialization cppObj
        );
private:
    xbox::services::multiplayer::multiplayer_managed_initialization m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END