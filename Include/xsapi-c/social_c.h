// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

typedef enum XblSocialRelationshipFilter
{
    /// <summary>All the people on the user's people list.</summary>
    XblSocialRelationshipFilter_All,

    /// <summary>Filters to only the people on the user's people list that have the attribute "Favorite" associated with them.</summary>
    XblSocialRelationshipFilter_Favorite,

    /// <summary>Filters to only the people on the user's people list that are also legacy Xbox Live friends.</summary>
    XblSocialRelationshipFilter_LegacyXboxLiveFriends
} XblSocialRelationshipFilter;

/// <summary>
/// Defines values used to identify the type of reputation feedback.
/// </summary>
typedef enum XblReputationFeedbackType
{
    /// <summary>
    /// Titles that are able to automatically determine that a user kills a teammate
    /// may send this feedback without user intervention.
    /// </summary>
    XblReputationFeedbackType_FairPlayKillsTeammates,

    /// <summary>
    /// Titles that are able to automatically determine that a user is cheating
    /// may send this feedback without user intervention.
    /// </summary>
    XblReputationFeedbackType_FairPlayCheater,

    /// <summary>
    /// Titles that are able to automatically determine that a user has tampered with on-disk content
    /// may send this feedback without user intervention.
    /// </summary>
    XblReputationFeedbackType_FairPlayTampering,

    /// <summary>
    /// Titles that are able to automatically determine that a user quit a game early
    /// may send this feedback without user intervention.
    /// </summary>
    XblReputationFeedbackType_FairPlayQuitter,

    /// <summary>
    /// When a user is voted out of a game (kicked), titles
    /// may send this feedback without user intervention.
    /// </summary>
    XblReputationFeedbackType_FairPlayKicked,

    /// <summary>
    /// Titles that allow users to report inappropriate video communications
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommunicationsInappropriateVideo,

    /// <summary>
    /// Titles that allow users to report inappropriate voice communications
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommunicationsAbusiveVoice,

    /// <summary>
    /// Titles that allow users to report inappropriate user generated content
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_InappropriateUserGeneratedContent,

    /// <summary>
    /// Titles that allow users to vote on a most valuable player at the end of a multiplayer session
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_PositiveSkilledPlayer,

    /// <summary>
    /// Titles that allow users to submit positive feedback on helpful fellow players
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_PositiveHelpfulPlayer,

    /// <summary>
    /// Titles that allow users to submit positive feedback on shared user generated content
    /// may send this feedback.
    /// </summary>
    XblReputationFeedbackType_PositiveHighQualityUserGeneratedContent,

    /// <summary>
    /// Titles that allow users to report phishing message may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommsPhishing,

    /// <summary>
    /// Titles that allow users to report communication based on a picture may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommsPictureMessage,

    /// <summary>
    /// Titles that allow users to report spam messages may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommsSpam,

    /// <summary>
    /// Titles that allow users to report text messages may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommsTextMessage,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    XblReputationFeedbackType_CommsVoiceMessage,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    XblReputationFeedbackType_FairPlayConsoleBanRequest,

    /// <summary>
    /// Titles that allow users to report if determine if a user stands idle on purpose in a game, usually round after round, may send this feedback.
    /// </summary>
    XblReputationFeedbackType_FairPlayIdler,

    /// <summary>
    /// Titles that report a recommendation to ban a user from Xbox Live may send this feedback.
    /// </summary>
    XblReputationFeedbackType_FairPlayUserBanRequest,

    /// <summary>
    /// Titles that allow users to report inappropriate gamer picture may send this feedback.
    /// </summary>
    XblReputationFeedbackType_UserContentGamerpic,

    /// <summary>
    /// Titles that allow users to report inappropriate biography and other personal information may send this feedback.
    /// </summary>
    XblReputationFeedbackType_UserContentPersonalInfo,

    /// <summary>
    /// Titles that allow users to report unsporting behavior may send this feedback.
    /// </summary>
    XblReputationFeedbackType_FairPlayUnsporting,

    /// <summary>
    /// Titles that allow users to report leaderboard cheating may send this feedback.
    /// </summary>
    XblReputationFeedbackType_FairPlayLeaderboardCheater
} XblReputationFeedbackType;

typedef enum XblSocialNotificationType
{
    /// <summary>
    /// unknown
    /// </summary>
    XblSocialNotificationType_Unknown,

    /// <summary>
    /// User(s) were added.
    /// </summary>
    XblSocialNotificationType_Added,

    /// <summary>
    /// User(s) data changed.
    /// </summary>
    XblSocialNotificationType_Changed,

    /// <summary>
    /// User(s) were removed.
    /// </summary>
    XblSocialNotificationType_Removed
} XblSocialNotificationType;

typedef struct XblSocialRelationship
{
    /// <summary>
    /// The person's Xbox user identifier.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// Indicates whether the person is one that the user cares about more.
    /// Users can have a very large number of people in their people list,
    /// favorite people should be prioritized first in experiences and shown before others that are not favorites.
    /// </summary>
    bool isFavorite;

    /// <summary>
    /// Indicates whether the person is following the person that requested the information.
    /// </summary>
    bool isFollowingCaller;

    /// <summary>
    /// A collection of strings indicating which social networks this person has a relationship with. 
    /// </summary>
    UTF8CSTR* socialNetworks;

    /// <summary>
    /// The count of string in socialNetworks array
    /// </summary>
    uint32_t socialNetworksCount;
} XblSocialRelationship;

typedef struct XblSocialRelationshipResult
{
    /// <summary>
    /// Collection of XboxSocialRelationship objects returned by a request.
    /// </summary>
    XblSocialRelationship* items;

    /// <summary>
    /// The count of XboxSocialRelationship objects in items array.
    /// </summary>
    uint32_t itemsCount;

    /// <summary>
    /// The total number of XboxSocialRelationship objects that can be requested.
    /// </summary>
    uint32_t totalCount;

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of social relationships to retrieve.
    /// </summary>
    bool hasNext;

    /// <summary>
    /// Internal
    /// </summary>
    XblSocialRelationshipFilter filter;

    /// <summary>
    /// Internal
    /// </summary>
    uint32_t continuationSkip;
} XblSocialRelationshipResult;

typedef struct XblSocialRelationshipChangeEventArgs
{
    /// <summary>
    /// The Xbox user ID for the user who's social graph changes are being listed for.
    /// </summary>
    uint64_t callerXboxUserId;

    /// <summary>
    /// The type of notification change.
    /// </summary>
    XblSocialNotificationType socialNotification;

    /// <summary>
    /// The number of strings in the xboxUserIds array
    /// </summary>
    uint32_t xboxUserIdsCount;

    /// <summary>
    /// The Xbox user ids who the event is for
    /// </summary>
    uint64_t xboxUserIds[ANYSIZE_ARRAY];
} XblSocialRelationshipChangeEventArgs;

/// <summary>
/// Gets a XblSocialRelationshipResult containing a the list of people that the caller is connected to.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User Id to get the social relationships for.</param>
/// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
/// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
STDAPI XblSocialGetSocialRelationships(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblSocialRelationshipFilter socialRelationshipFilter
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblSocialGetSocialRelationships API call.
/// The required buffer size should first be obtained with GetAsyncResultSize.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The size of the provided buffer.</param>
/// <param name="result">The buffer to be written to.</param>
/// <param name="bufferUser">The actual number of bytes written to the buffer.</param>
STDAPI XblSocialGetSocialRelationshipsResult(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblSocialRelationshipResult* buffer,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets an XblSocialRelationshipResult object containing the next page.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialRelationshipResult">Result returned from a previous call to XblGetSocialRelations*.</param>
/// <param name="maxItems">Controls the number of XblSocialRelationship objects to get. 0 will return as many as possible</param>
/// <remarks>Calls V1 GET /users/{ownerId}/people</remarks>
STDAPI XblSocialRelationshipResultGetNext(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ const XblSocialRelationshipResult* socialRelationshipResult,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblSocialRelationshipResultGetNextResult API call.
/// The required buffer size should first be obtained with GetAsyncResultSize.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The size of the provided buffer.</param>
/// <param name="result">The buffer to be written to.</param>
/// <param name="bufferUser">The actual number of bytes written to the buffer.</param>
STDAPI XblSocialRelationshipResultGetNextResult(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblSocialRelationshipResult* buffer,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT;

typedef void* xbl_social_relationship_change_subscription_handle;

/// <summary>
/// Subscribes to the social service for people changed events
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
/// <param name="subscriptionHandle">A handle to the subscription which is used to unsubscribe.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XblSocialSubscribeToSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _Out_ xbl_social_relationship_change_subscription_handle* subscriptionHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Unsubscribes a previously created social relationship change subscription.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="subscription">The subscription handle to unsubscribe</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XblSocialUnsubscribeFromSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_social_relationship_change_subscription_handle subscriptionHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// A callback invoked when a social relationship changes. For the callback to be called, you must
/// first subscribe to social relationship changes for at least one user by calling XblSocialSubscribeToSocialRelationshipChange.
/// </summary>
/// <param name="eventArgs">
/// The arguments associated with the relationship change. The fields of the struct are only valid during the callback.
/// </param>
/// <param name="context">Context provided by when the handler is added.</param>
typedef void
(STDAPIVCALLTYPE* XblSocialRelationshipChangedHandler)(
    _In_ XblSocialRelationshipChangeEventArgs* eventArgs,
    _In_opt_ void* context
    );

/// <summary>
/// Registers an event handler for social relationship change notifications.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Client context pointer to be passed back to the handler.</param>
/// <returns>A function_context used to remove the handler</returns>
STDAPI_(function_context) XblSocialAddSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblSocialRelationshipChangedHandler handler,
    _In_ void* handlerContext
    ) XBL_NOEXCEPT;

/// <summary>
/// Removes a social relationship change handler.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handlerContext">Context for the handler to remove.</param>
STDAPI_(void) XblSocialRemoveSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ function_context handlerFunctionContext
    ) XBL_NOEXCEPT;

typedef struct XblReputationFeedbackItem
{
    /// <summary>
    /// The Xbox User ID of the user that reputation feedback is being submitted on.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The reputation feedback type being submitted.
    /// </summary>
    XblReputationFeedbackType feedbackType;

    /// <summary>
    /// User supplied text added to explain the reason for the feedback.
    /// </summary>
    UTF8CSTR reasonMessage;

    /// <summary>
    /// The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file.
    /// </summary>
    UTF8CSTR evidenceResourceId;
} XblReputationFeedbackItem;

/// <summary>
/// Submits reputation feedback on the specified user. The status of the async operation can be obtained with
/// GetAsyncStatus.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
/// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
/// <param name="sessionName">The name of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
/// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
/// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
/// <remarks>Calls V100 POST /users/xuid({xuid})/feedback</remarks>
STDAPI XblSocialSubmitReputationFeedback(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblReputationFeedbackType reputationFeedbackType,
    _In_opt_ UTF8CSTR sessionName,
    _In_opt_ UTF8CSTR reasonMessage,
    _In_opt_ UTF8CSTR evidenceResourceId
    ) XBL_NOEXCEPT;

/// <summary>
/// Submits reputation feedback on the specified user. The status of the async operation can be obtained with
/// GetAsyncStatus.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="feedbackItems">An array of XblReputationFeedbackItem objects to submit reputation feedback on.</param>
/// <param name="feedbackItemsCount">The count of items in the feedbackItems array.</param>
/// <returns>Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.</returns>
/// <remarks>Calls V101 POST /users/batchfeedback</remarks>
STDAPI XblSocialSubmitBatchReputationFeedback(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblReputationFeedbackItem* feedbackItems,
    _In_ uint32_t feedbackItemsCount
    ) XBL_NOEXCEPT;