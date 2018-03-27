// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER
{
    /// <summary>All the people on the user's people list.</summary>
    XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL,

    /// <summary>Filters to only the people on the user's people list that have the attribute "Favorite" associated with them.</summary>
    XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_FAVORITE,

    /// <summary>Filters to only the people on the user's people list that are also legacy Xbox Live friends.</summary>
    XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_LEGACY_XBOX_LIVE_FRIENDS
} XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER;

/// <summary>
/// Defines values used to identify the type of reputation feedback.
/// </summary>
typedef enum XBL_REPUTATION_FEEDBACK_TYPE
{
    /// <summary>
    /// Titles that are able to automatically determine that a user kills a teammate
    /// may send this feedback without user intervention.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_KILLS_TEAMMATES,

    /// <summary>
    /// Titles that are able to automatically determine that a user is cheating
    /// may send this feedback without user intervention.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_CHEATER,

    /// <summary>
    /// Titles that are able to automatically determine that a user has tampered with on-disk content
    /// may send this feedback without user intervention.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_TAMPERING,

    /// <summary>
    /// Titles that are able to automatically determine that a user quit a game early
    /// may send this feedback without user intervention.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_QUITTER,

    /// <summary>
    /// When a user is voted out of a game (kicked), titles
    /// may send this feedback without user intervention.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_KICKED,

    /// <summary>
    /// Titles that allow users to report inappropriate video communications
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMUNICATIONS_INAPPROPIATE_VIDEO,

    /// <summary>
    /// Titles that allow users to report inappropriate voice communications
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMUNICATIONS_ABUSIVE_VOICE,

    /// <summary>
    /// Titles that allow users to report inappropriate user generated content
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_INAPPROPIATE_USER_GENERATED_CONTENT,

    /// <summary>
    /// Titles that allow users to vote on a most valuable player at the end of a multiplayer session
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_POSITIVE_SKILLED_PLAYER,

    /// <summary>
    /// Titles that allow users to submit positive feedback on helpful fellow players
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_POSITIVE_HELPFUL_PLAYER,

    /// <summary>
    /// Titles that allow users to submit positive feedback on shared user generated content
    /// may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_POSITIVE_HIGH_QUALITY_USER_GENERATED_CONTENT,

    /// <summary>
    /// Titles that allow users to report phishing message may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMS_PHISHING,

    /// <summary>
    /// Titles that allow users to report communication based on a picture may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMS_PICTURE_MESSAGE,

    /// <summary>
    /// Titles that allow users to report spam messages may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMS_SPAM,

    /// <summary>
    /// Titles that allow users to report text messages may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMS_TEXT_MESSAGE,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_COMMS_VOICE_MESSAGE,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_CONSOLE_BAN_REQUEST,

    /// <summary>
    /// Titles that allow users to report if determine if a user stands idle on purpose in a game, usually round after round, may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_IDLER,

    /// <summary>
    /// Titles that report a recommendation to ban a user from Xbox Live may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_USER_BAN_REQUEST,

    /// <summary>
    /// Titles that allow users to report inappropriate gamer picture may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_USER_CONTENT_GAMERPIC,

    /// <summary>
    /// Titles that allow users to report inappropriate biography and other personal information may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_USER_CONTENT_PERSONALINFO,

    /// <summary>
    /// Titles that allow users to report unsporting behavior may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_UNSPORTING,

    /// <summary>
    /// Titles that allow users to report leaderboard cheating may send this feedback.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE_FAIR_PLAY_LEADERBOARD_CHEATER
} XBL_REPUTATION_FEEDBACK_TYPE;

typedef enum XBL_SOCIAL_NOTIFICATION_TYPE
{
    /// <summary>
    /// unknown
    /// </summary>
    XBL_SOCIAL_NOTIFICATION_TYPE_UNKNOWN,

    /// <summary>
    /// User(s) were added.
    /// </summary>
    XBL_SOCIAL_NOTIFICATION_TYPE_ADDED,

    /// <summary>
    /// User(s) data changed.
    /// </summary>
    XBL_SOCIAL_NOTIFICATION_TYPE_CHANGED,

    /// <summary>
    /// User(s) were removed.
    /// </summary>
    XBL_SOCIAL_NOTIFICATION_TYPE_REMOVED
} XBL_SOCIAL_NOTIFICATION_TYPE;

typedef struct XBL_XBOX_SOCIAL_RELATIONSHIP
{
    /// <summary>
    /// The person's Xbox user identifier.
    /// </summary>
    PCSTR xboxUserId;

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
    PCSTR *socialNetworks;

    /// <summary>
    /// The count of string in socialNetworks array
    /// </summary>
    uint32_t socialNetworksCount;
} XBL_XBOX_SOCIAL_RELATIONSHIP;

typedef struct XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT
{
    /// <summary>
    /// Collection of XboxSocialRelationship objects returned by a request.
    /// </summary>
    XBL_XBOX_SOCIAL_RELATIONSHIP *items;

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
    XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER filter;

    /// <summary>
    /// Internal
    /// </summary>
    uint32_t continuationSkip;
} XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT;

typedef struct XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS
{
    /// <summary>
    /// The Xbox user ID for the user who's social graph changes are being listed for.
    /// </summary>
    PCSTR callerXboxUserId;

    /// <summary>
    /// The type of notification change.
    /// </summary>
    XBL_SOCIAL_NOTIFICATION_TYPE socialNotification;

    /// <summary>
    /// The Xbox user ids who the event is for
    /// </summary>
    PCSTR *xboxUserIds;

    /// <summary>
    /// The number of strings in the xboxUserIds array
    /// </summary>
    uint32_t xboxUserIdsCount;
} XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS;

/// <summary>
/// Callback function signature for XblGetSocialRelationships*. result and profile are valid only until the callback returns.
/// </summary>
typedef void(*XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ CONST XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT *socialRelationShipResult,
    _In_opt_ void* context
    );

/// <summary>
/// Gets a XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT containing a the list of people that the caller is connected to.
/// Defaults to filtering to PersonView.All.
/// Defaults to startIndex and maxItems of 0 to return entire list if possible.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>
/// Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationships(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets a XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT containing a the list of people that the caller is connected to.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>
/// Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsWithFilter(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets a XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT containing a the list of people that the specified user is connected to.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User Id to get the social relationships for.</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>
/// Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsForUser(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets a XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT containing a the list of people that the caller is connected to.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
/// <param name="startIndex">Controls the starting index to return.</param>
/// <param name="maxItems">Controls the number of XBL_XBOX_SOCIAL_RELATIONSHIP objects to get.  0 will return as many as possible</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>
/// Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsEx(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets an XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT object containing the next page.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialRelationshipResult">Result returned from a previous call to XblGetSocialRelations*.</param>
/// <param name="maxItems">Controls the number of XBL_XBOX_SOCIAL_RELATIONSHIP objects to get.  0 will return as many as possible</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>
/// <returns>
/// Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V1 GET /users/{ownerId}/people</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialRelationshipResultGetNext(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ CONST XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT *socialRelationshipResult,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Creates a deep copy of an XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT in a user allocated buffer. If the provided buffer
/// is not large enough, nullptr will be returned and cbBuffer will be set to the required buffer size. If the buffer is
/// large enough, the source object will be copied into the provided buffer. The returned pointer will be the same as the
/// provided buffer pointer (except that it is typed). The caller is responsible for freeing the buffer when they no longer
/// need the copied object. This method will not allocate any additional memory.
/// </summary>
/// <param name="source">An XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT returned from another API.</param>
/// <param name="buffer">User allocated buffer to copy the source object into.</param>
/// <param name="cbBuffer">Size in bytes of the input buffer. If the buffer is not large enough, this will be set to the required size.</param>
/// <returns>
/// A pointer to the copied object. Equivalent to the provided buffer pointer.
/// </returns>
XBL_API XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT* XBL_CALLING_CONV
XblCopySocialRelationshipResult(
    _In_ CONST XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT *source,
    _In_ void *buffer,
    _Inout_ uint64_t *cbBuffer
    ) XBL_NOEXCEPT;

typedef void* XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION;

/// <summary>
/// Subscribes to the social service for people changed events
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
/// <param name="subscriptionHandle">A handle to the subscription which is used to unsubscribe.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubscribeToSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _Out_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION *subscriptionHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Unsubscribes a previously created social relationship change subscription.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="subscription">The subscription handle to unsubscribe</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblUnsubscribeFromSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION subscriptionHandle
    ) XBL_NOEXCEPT;

typedef void(*XBL_SOCIAL_RELATIONSHIP_CHANGED_HANDLER)(
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS eventArgs,
    _In_opt_ void* context
    );

/// <summary>
/// Registers an event handler for social relationship change notifications.
/// Event handlers receive social_relationship_change_event_args.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Client context pointer to be passed back to the handler.</param>
/// <returns>A function_context used to remove the handler</returns>
XBL_API function_context XBL_CALLING_CONV
XblAddSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGED_HANDLER handler,
    _In_ void *handlerContext
    ) XBL_NOEXCEPT;

/// <summary>
/// Removes a social relationship change handler
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handlerContext">Context for the handler to remove.</param>
XBL_API void XBL_CALLING_CONV
XblRemoveSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ function_context handlerFunctionContext
    ) XBL_NOEXCEPT;

typedef struct XBL_REPUTATION_FEEDBACK_ITEM
{
    /// <summary>
    /// The Xbox User ID of the user that reputation feedback is being submitted on.
    /// </summary>
    PCSTR xboxUserId;

    /// <summary>
    /// The reputation feedback type being submitted.
    /// </summary>
    XBL_REPUTATION_FEEDBACK_TYPE feedbackType;

    /// <summary>
    /// The reference to the multiplayer session directory session the user is sending feedback from.
    /// </summary>
    // TODO multiplayer session reference

    /// <summary>
    /// User supplied text added to explain the reason for the feedback.
    /// </summary>
    PCSTR reasonMessage;

    /// <summary>
    /// The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file.
    /// </summary>
    PCSTR evidenceResourceId;
} XBL_REPUTATION_FEEDBACK_ITEM;

typedef void(*XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_opt_ void* context
    );

/// <summary>
/// Submits reputation feedback on the specified user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
/// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
/// <param name="sessionName">The name of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
/// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
/// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.</returns>
/// <remarks>Calls V100 POST /users/xuid({xuid})/feedback</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitReputationFeedback(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_REPUTATION_FEEDBACK_TYPE reputationFeedbackType,
    _In_opt_ PCSTR sessionName,
    _In_opt_ PCSTR reasonMessage,
    _In_opt_ PCSTR evidenceResourceId,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void *callbackContext,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

/// <summary>
/// Submits reputation feedback on the specified user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="feedbackItems">An array of XBL_REPUTATION_FEEDBACK_ITEM objects to submit reputation feedback on.</param>
/// <param name="feedbackItemsCount">The count of items in the feedbackItems array.</param>
/// <param name="queue">The async queue to associate this call with.</param>
/// <param name="callbackContext">Context passed back to callback.</param>
/// <param name="callback">A client callback function that will be called when the async operation is complete.</param>
/// <returns>Result code for this API operation. The result of the asynchronous operation is returned via the callback parameters.</returns>
/// <remarks>Calls V101 POST /users/batchfeedback</remarks>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitBatchReputationFeedback(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XBL_REPUTATION_FEEDBACK_ITEM *feedbackItems,
    _In_ uint32_t feedbackItemsCount,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void *callbackContext,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)