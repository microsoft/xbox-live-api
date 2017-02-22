// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "ReputationFeedbackType_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

/// <summary>
/// Represents the parameters for submitting reputation feedback on a user
/// </summary>
public ref class ReputationFeedbackItem sealed
{
public:
    ReputationFeedbackItem();

    /// <summary>
    /// Construct a ReputationFeedbackItem object
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
    /// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
    /// <param name="sessionReference">The session reference of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
    /// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
    /// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
    ReputationFeedbackItem(
        _In_ Platform::String^ xboxUserId,
        _In_ ReputationFeedbackType reputationFeedbackType,
        _In_ Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
        _In_ Platform::String^ reasonMessage,
        _In_ Platform::String^ evidenceResourceId
        );

    /// <summary>
    /// The Xbox User ID of the user that reputation feedback is being submitted on.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The reputation feedback type being submitted.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(FeedbackType, feedback_type, ReputationFeedbackType);

    /// <summary>
    /// The reference to the multiplayer session directory session the user is sending feedback from.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ SessionReference { Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ get(); }

    /// <summary>
    /// User supplied text added to explain the reason for the feedback.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ReasonMessage, reason_message);

    /// <summary>
    /// The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(EvidenceResourceId, evidence_resource_id);

internal:
    xbox::services::social::reputation_feedback_item GetCppObj() const;

private:
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ m_sessionRef;
    xbox::services::social::reputation_feedback_item m_cppObj;
};


/// <summary>
/// Provides access methods for the reputation service.
/// </summary>
public ref class ReputationService sealed
{
public:
    /// <summary>
    /// Submits reputation feedback on the specified user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
    /// <param name="reputationFeedbackType">The reputation feeback type being submitted.</param>
    /// <param name="sessionName">The name of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
    /// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
    /// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V100 POST /users/xuid({xuid})/feedback</remarks>
    Windows::Foundation::IAsyncAction^ SubmitReputationFeedbackAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ ReputationFeedbackType reputationFeedbackType,
        _In_opt_ Platform::String^ sessionName,
        _In_opt_ Platform::String^ reasonMessage,
        _In_opt_ Platform::String^ evidenceResourceId
        );

    /// <summary>
    /// Submits batch reputation feedback on the specified users.
    /// </summary>
    /// <param name="feedbackItems">A vector of reputation_feedback_item objects to submit reputation feedback on.</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V101 POST /users/batchfeedback</remarks>
    Windows::Foundation::IAsyncAction^ SubmitBatchReputationFeedbackAsync(
        _In_ Windows::Foundation::Collections::IVectorView<ReputationFeedbackItem^>^ feedbackItems
        );

internal:
    ReputationService(
        _In_ xbox::services::social::reputation_service cppObj
        );
    
private:
    xbox::services::social::reputation_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END