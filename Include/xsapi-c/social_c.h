// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

#include "real_time_activity_c.h"
#include "multiplayer_c.h"

extern "C"
{

/// <summary>
/// Defines the relationship filters available for social groups.
/// </summary>
/// <argof><see cref="XblSocialGetSocialRelationshipsAsync"/></argof>
enum class XblSocialRelationshipFilter : uint32_t
{
    /// <summary>
    /// All the people on the user's people list.
    /// </summary>
    All,

    /// <summary>
    /// Filters to only the people on the user's people list that have the attribute "Favorite" associated with them.
    /// </summary>
    Favorite,

    /// <summary>
    /// Filters to only the people on the user's people list that are also legacy Xbox Live friends.
    /// </summary>
    LegacyXboxLiveFriends
};

/// <summary>
/// Defines values used to identify the type of reputation feedback.
/// </summary>
/// <argof><see cref="XblSocialSubmitReputationFeedbackAsync"/></argof>
/// <memof><see cref="XblReputationFeedbackItem"/></memof>
enum class XblReputationFeedbackType : uint32_t
{
    /// <summary>
    /// Titles that are able to automatically determine that a user kills a teammate 
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayKillsTeammates,

    /// <summary>
    /// Titles that are able to automatically determine that a user is cheating 
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayCheater,

    /// <summary>
    /// Titles that are able to automatically determine that a user has tampered 
    /// with on-disk content may send this feedback without user intervention.
    /// </summary>
    FairPlayTampering,

    /// <summary>
    /// Titles that are able to automatically determine that a user quit a game early 
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayQuitter,

    /// <summary>
    /// When a user is voted out of a game (kicked), titles 
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayKicked,

    /// <summary>
    /// Titles that allow users to report inappropriate video communications 
    /// may send this feedback.
    /// </summary>
    CommunicationsInappropriateVideo,

    /// <summary>
    /// Titles that allow users to report inappropriate voice communications 
    /// may send this feedback.
    /// </summary>
    CommunicationsAbusiveVoice,

    /// <summary>
    /// Titles that allow users to report inappropriate user generated content 
    /// may send this feedback.
    /// </summary>
    InappropriateUserGeneratedContent,

    /// <summary>
    /// Titles that allow users to vote on a most valuable player at the end of a multiplayer session 
    /// may send this feedback.
    /// </summary>
    PositiveSkilledPlayer,

    /// <summary>
    /// Titles that allow users to submit positive feedback on helpful fellow players 
    /// may send this feedback.
    /// </summary>
    PositiveHelpfulPlayer,

    /// <summary>
    /// Titles that allow users to submit positive feedback on shared user generated content 
    /// may send this feedback.
    /// </summary>
    PositiveHighQualityUserGeneratedContent,

    /// <summary>
    /// Titles that allow users to report phishing message may send this feedback.
    /// </summary>
    CommsPhishing,

    /// <summary>
    /// Titles that allow users to report communication based on a picture 
    /// may send this feedback.
    /// </summary>
    CommsPictureMessage,

    /// <summary>
    /// Titles that allow users to report spam messages may send this feedback.
    /// </summary>
    CommsSpam,

    /// <summary>
    /// Titles that allow users to report text messages may send this feedback.
    /// </summary>
    CommsTextMessage,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    CommsVoiceMessage,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    FairPlayConsoleBanRequest,

    /// <summary>
    /// Titles that allow users to report if a user stands idle on purpose in a game, 
    /// usually round after round, may send this feedback.
    /// </summary>
    FairPlayIdler,

    /// <summary>
    /// Titles that report a recommendation to ban a user from Xbox Live may send this feedback.
    /// </summary>
    FairPlayUserBanRequest,

    /// <summary>
    /// Titles that allow users to report inappropriate gamer picture may send this feedback.
    /// </summary>
    UserContentGamerpic,

    /// <summary>
    /// Titles that allow users to report inappropriate biography and other personal information 
    /// may send this feedback.
    /// </summary>
    UserContentPersonalInfo,

    /// <summary>
    /// Titles that allow users to report unsporting behavior may send this feedback.
    /// </summary>
    FairPlayUnsporting,

    /// <summary>
    /// Titles that allow users to report leaderboard cheating may send this feedback.
    /// </summary>
    FairPlayLeaderboardCheater
};

/// <summary>
/// Defines values used to identify the type of social notification.
/// </summary>
/// <memof><see cref="XblSocialRelationshipChangeEventArgs"/></memof>
enum class XblSocialNotificationType : uint32_t
{
    /// <summary>
    /// Unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// User(s) were added.
    /// </summary>
    Added,

    /// <summary>
    /// User(s) data changed.
    /// </summary>
    Changed,

    /// <summary>
    /// User(s) were removed.
    /// </summary>
    Removed
};

/// <summary>
/// Represents the relationship between the user and another Xbox user.
/// </summary>
/// <argof><see cref="XblSocialRelationshipResultGetRelationships"/></argof>
typedef struct XblSocialRelationship
{
    /// <summary>
    /// The person's Xbox user identifier.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// Indicates whether the person is one that the user cares about more.  
    /// Since users can have a very large number of people in their people list, favorite people 
    /// should be prioritized first in experiences and shown before others that are not favorites.
    /// </summary>
    bool isFavorite;

    /// <summary>
    /// Indicates whether the person is following the person that requested the information.
    /// </summary>
    bool isFollowingCaller;

    /// <summary>
    /// A UTF-8 encoded collection of strings indicating which social networks 
    /// this person has a relationship with.
    /// </summary>
    _Field_z_ const char** socialNetworks;

    /// <summary>
    /// The count of social networks strings in the socialNetworks array.
    /// </summary>
    size_t socialNetworksCount;
} XblSocialRelationship;

/// <summary>
/// Event arguments for a social relationship change.
/// </summary>
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
    /// The Xbox user ids who the event is for.
    /// </summary>
    uint64_t* xboxUserIds;

    /// <summary>
    /// The number of strings in the xboxUserIds array.
    /// </summary>
    size_t xboxUserIdsCount;
} XblSocialRelationshipChangeEventArgs;

/// <summary>
/// A handle to a social relationship result.
/// </summary>
/// <remarks>
/// This handle is used by other APIs to get the social relationship objects and to get 
/// the next page of results from the service if there is one.  
/// The handle must be closed using <see cref="XblSocialRelationshipResultCloseHandle"/> when the result is no longer needed.
/// </remarks>
typedef struct XblSocialRelationshipResult* XblSocialRelationshipResultHandle;

/// <summary>
/// Gets a list of people that the caller is socially connected to.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User Id to get the social relationships for.</param>
/// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
/// <param name="startIndex">Controls the starting index of the results list.</param>
/// <param name="maxItems">The maximum number of items that the results list can contain.  
/// Pass 0 to attempt to retrieve all items.</param>
/// <param name="async">Caller allocated <see cref="XAsyncBlock"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblSocialGetSocialRelationshipsResult"/> to get the result.
/// </remarks>
/// <rest>V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</rest>
STDAPI XblSocialGetSocialRelationshipsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblSocialRelationshipFilter socialRelationshipFilter,
    _In_ size_t startIndex,
    _In_ size_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result handle from an XblSocialGetSocialRelationshipsAsync API call.
/// </summary>
/// <param name="async">The same AsyncBlock that was passed to <see cref="XblSocialGetSocialRelationshipsAsync"/>.</param>
/// <param name="handle">Passes back an XblSocialRelationshipResultHandle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When no longer using the XblSocialRelationshipResultHandle, make sure to call <see cref="XblSocialRelationshipResultCloseHandle"/>.  
/// When all outstanding handles have been closed, the memory associated with the social relationship's results list will be freed.  
/// After calling this API, make sure to call <see cref="XblSocialRelationshipResultGetRelationships"/> to get the results list to iterate over.  
/// Then call <see cref="XblSocialRelationshipResultHasNext"/> to check if there are additional pages of results.
/// </remarks>
STDAPI XblSocialGetSocialRelationshipsResult(
    _In_ XAsyncBlock* async,
    _Out_ XblSocialRelationshipResultHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Get a list of the XblSocialRelationship objects from an XblSocialRelationshipResultHandle.
/// </summary>
/// <param name="resultHandle">Social relationship result handle.</param>
/// <param name="relationships">Passes back a pointer to an array of XblSocialRelationship objects.  
/// The memory for the returned pointer remains valid for the life of the XblSocialRelationshipResultHandle object until it is closed.</param>
/// <param name="relationshipsCount">Passes back the number of items in the relationships array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When no longer using the XblSocialRelationshipResultHandle, make sure to call <see cref="XblSocialRelationshipResultCloseHandle"/>.  
/// When all outstanding handles have been closed, the memory associated with the social relationship's results list will be freed.
/// </remarks>
STDAPI XblSocialRelationshipResultGetRelationships(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ const XblSocialRelationship** relationships,
    _Out_ size_t* relationshipsCount
) XBL_NOEXCEPT;

/// <summary>
/// Checks if there are more pages of social relationships to retrieve from the service.
/// </summary>
/// <param name="resultHandle">The XblSocialRelationshipResultHandle from XblSocialGetSocialRelationshipsResult.</param>
/// <param name="hasNext">Passes back true if there are more results to retrieve, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the result has a next page, then call <see cref="XblSocialRelationshipResultGetNextAsync"/> to retrieve the next page of items.
/// </remarks>
STDAPI XblSocialRelationshipResultHasNext(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ bool* hasNext
) XBL_NOEXCEPT;

/// <summary>
/// Gets the total number of results for an XblSocialRelationshipResultHandle.
/// </summary>
/// <param name="resultHandle">The XblSocialRelationshipResultHandle from <see cref="XblSocialGetSocialRelationshipsResult"/>.</param>
/// <param name="totalCount">Passes back the total number of results for the query.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that this is only the total number of results requested by <see cref="XblSocialGetSocialRelationshipsAsync"/> 
/// and may be different from the maximum number of result items.
/// </remarks>
STDAPI XblSocialRelationshipResultGetTotalCount(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ size_t* totalCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the next page of a list of people that the caller is socially connected to.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="resultHandle">Social relationship result handle from a previous call to XblSocialGetSocialRelationshipsAsync.</param>
/// <param name="maxItems">Controls the number of XblSocialRelationship objects to get. 0 will return as many as possible.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call this API after finding more results from <see cref="XblSocialRelationshipResultHasNext"/>.  
/// After calling this API, make sure to call <see cref="XblSocialRelationshipResultGetNextResult"/> to get the result.
/// </remarks>
STDAPI XblSocialRelationshipResultGetNextAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _In_ size_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblSocialRelationshipResultGetNextAsync API call.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to <see cref="XblSocialRelationshipResultGetNextAsync"/>.</param>
/// <param name="handle">Passes back a XblSocialRelationshipResultHandle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When no longer using the XblSocialRelationshipResultHandle, make sure to call <see cref="XblSocialRelationshipResultCloseHandle"/>.  
/// When all outstanding handles have been closed, the memory associated with the social relationship's results list will be freed.  
/// After calling this API, make sure to call <see cref="XblSocialRelationshipResultGetRelationships"/> to get the results list to iterate over.  
/// Then call <see cref="XblSocialRelationshipResultHasNext"/> to see if there are more results.
/// </remarks>
STDAPI XblSocialRelationshipResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblSocialRelationshipResultHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates the XblSocialRelationshipResultHandle.
/// </summary>
/// <param name="handle">The XblSocialRelationshipResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">Passes back the duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When no longer using the XblSocialRelationshipResultHandle, make sure to call <see cref="XblSocialRelationshipResultCloseHandle"/>.
/// </remarks>
STDAPI XblSocialRelationshipResultDuplicateHandle(
    _In_ XblSocialRelationshipResultHandle handle,
    _Out_ XblSocialRelationshipResultHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes the XblSocialRelationshipResultHandle.
/// </summary>
/// <param name="handle">The XblSocialRelationshipResultHandle to close.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, the memory associated with the social relationship's results list will be freed.
/// </remarks>
STDAPI_(void) XblSocialRelationshipResultCloseHandle(
    _In_ XblSocialRelationshipResultHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Subscribes to the social service for people changed events.
/// DEPRECATED. Calling this API is no longer required and it will be removed in a future release. RTA subscription will be managed 
/// automatically by XSAPI as `XblSocialRelationshipChangedHandler` are added and removed.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
/// <param name="subscriptionHandle">Passes back a handle to the subscription which is used to un-subscribe.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblSocialUnsubscribeFromSocialRelationshipChange"/> to un-subscribe.
/// </remarks>
STDAPI_XBL_DEPRECATED XblSocialSubscribeToSocialRelationshipChange(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Un-subscribes a previously created social relationship change subscription.
/// DEPRECATED. Calling this API is no longer required and it will be removed in a future release. RTA subscription will be managed 
/// automatically by XSAPI as `XblSocialRelationshipChangedHandler` are added and removed.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="subscriptionHandle">The subscription handle to unsubscribe.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call this API only if <see cref="XblSocialSubscribeToSocialRelationshipChange"/> was used to subscribe to social relationship changes.
/// </remarks>
STDAPI_XBL_DEPRECATED XblSocialUnsubscribeFromSocialRelationshipChange(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// A callback invoked when a social relationship changes.
/// </summary>
/// <param name="eventArgs">The arguments associated with the relationship change.
/// The fields of the struct are only valid during the callback.</param>
/// <param name="context">Context provided by when the handler is added.</param>
/// <returns></returns>
/// <remarks>
/// For the callback to work properly, you must be subscribed to social relationship changes for at least one user.
/// </remarks>
typedef void
(STDAPIVCALLTYPE* XblSocialRelationshipChangedHandler)(
    _In_ const XblSocialRelationshipChangeEventArgs* eventArgs,
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for notifications of social relationship changes caused by the registering user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Client context pointer to be passed back to the handler.</param>
/// <returns>A XblFunctionContext used to remove the handler.</returns>
/// <remarks>
/// This handler triggers only if the user changes the relationship with another user.
/// This handler does not trigger if another user changes the relationship with the user.
///
/// Call <see cref="XblSocialRemoveSocialRelationshipChangedHandler"/> to un-register event handler.
/// </remarks>
STDAPI_(XblFunctionContext) XblSocialAddSocialRelationshipChangedHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblSocialRelationshipChangedHandler handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT;

/// <summary>
/// Removes a social relationship change handler.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handlerFunctionContext">Context for the handler to remove.</param>
/// <returns></returns>
/// <remarks>
/// <prereq/>
/// Call this API only if <see cref="XblSocialAddSocialRelationshipChangedHandler"/> was used to register an event handler.
/// </remarks>
STDAPI XblSocialRemoveSocialRelationshipChangedHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext handlerFunctionContext
) XBL_NOEXCEPT;

/// <summary>
/// Represents the parameters for submitting reputation feedback on a user.
/// </summary>
/// <argof><see cref="XblSocialSubmitBatchReputationFeedbackAsync"/></argof>
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
    /// Multiplayer session reference describing the MPSD session this feedback relates to. (Optional)
    /// </summary>
    XblMultiplayerSessionReference* sessionReference;

    /// <summary>
    /// UTF-8 encoded user supplied text added to explain the reason for the feedback.
    /// </summary>
    _Field_z_ const char* reasonMessage;

    /// <summary>
    /// The UTF-8 encoded id of a resource that can be used as evidence for the feedback.  
    /// Example: the Id of a video file.
    /// </summary>
    _Field_z_ const char* evidenceResourceId;
} XblReputationFeedbackItem;

/// <summary>
/// Submits reputation feedback on the specified user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
/// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
/// <param name="sessionReference">Multiplayer session reference describing the MPSD session this feedback relates to. (Optional)</param>
/// <param name="reasonMessage">User supplied text in UTF-8 encoded added to explain the reason for the feedback. (Optional)</param>
/// <param name="evidenceResourceId">The UTF-8 encoded id of a resource that can be used as evidence for the feedback.  
/// Example: the Id of a video file. (Optional)</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Make sure to call <see cref="XAsyncGetStatus"/> to get the result.
/// </remarks>
/// <rest>V100 POST /users/xuid({xuid})/feedback</rest>
STDAPI XblSocialSubmitReputationFeedbackAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblReputationFeedbackType reputationFeedbackType,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_z_ const char* reasonMessage,
    _In_opt_z_ const char* evidenceResourceId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Submits reputation feedback on the specified user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="feedbackItems">An array of XblReputationFeedbackItem objects to submit reputation feedback on.</param>
/// <param name="feedbackItemsCount">The count of items in the feedbackItems array.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Make sure to call <see cref="XAsyncGetStatus"/> to get the result.
/// </remarks>
/// <rest>V101 POST /users/batchfeedback</rest>
STDAPI XblSocialSubmitBatchReputationFeedbackAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const XblReputationFeedbackItem* feedbackItems,
    _In_ size_t feedbackItemsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

}