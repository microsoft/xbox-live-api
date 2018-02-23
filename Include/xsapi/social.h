// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/real_time_activity.h"
#include "xsapi/multiplayer.h"
#include "xsapi/system.h"

namespace xbox { namespace services {
    class xbox_live_context;

    /// <summary>
    /// Contains classes and enumerations that let you retrieve
    /// information about player reputation and relationship with
    /// other players from Xbox Live.
    /// </summary>
    namespace social {

class social_service;
class social_service_impl;
class reputation_service_impl;
class xbox_social_relationship_internal;
class xbox_social_relationship_result_internal;
class social_relationship_change_event_args_internal;
class social_relationship_change_subscription_internal;

enum class xbox_social_relationship_filter
{
    /// <summary>All the people on the user's people list.</summary>
    all,

    /// <summary>Filters to only the people on the user's people list that have the attribute "Favorite" associated with them.</summary>
    favorite,

    /// <summary>Filters to only the people on the user's people list that are also legacy Xbox Live friends.</summary>
    legacy_xbox_live_friends
};
/// <summary>
/// Defines values used to identify the type of reputation feedback.
/// </summary>
enum class reputation_feedback_type
{
    /// <summary>
    /// Titles that are able to automatically determine that a user kills a teammate
    /// may send this feedback without user intervention.
    /// </summary>
    fair_play_kills_teammates,

    /// <summary>
    /// Titles that are able to automatically determine that a user is cheating
    /// may send this feedback without user intervention.
    /// </summary>
    fair_play_cheater,

    /// <summary>
    /// Titles that are able to automatically determine that a user has tampered with on-disk content
    /// may send this feedback without user intervention.
    /// </summary>
    fair_play_tampering,

    /// <summary>
    /// Titles that are able to automatically determine that a user quit a game early
    /// may send this feedback without user intervention.
    /// </summary>
    fair_play_quitter,

    /// <summary>
    /// When a user is voted out of a game (kicked), titles
    /// may send this feedback without user intervention.
    /// </summary>
    fair_play_kicked,

    /// <summary>
    /// Titles that allow users to report inappropriate video communications
    /// may send this feedback.
    /// </summary>
    communications_inappropiate_video,

    /// <summary>
    /// Titles that allow users to report inappropriate voice communications
    /// may send this feedback.
    /// </summary>
    communications_abusive_voice,

    /// <summary>
    /// Titles that allow users to report inappropriate user generated content
    /// may send this feedback.
    /// </summary>
    inappropiate_user_generated_content,

    /// <summary>
    /// Titles that allow users to vote on a most valuable player at the end of a multiplayer session
    /// may send this feedback.
    /// </summary>
    positive_skilled_player,

    /// <summary>
    /// Titles that allow users to submit positive feedback on helpful fellow players
    /// may send this feedback.
    /// </summary>
    positive_helpful_player,

    /// <summary>
    /// Titles that allow users to submit positive feedback on shared user generated content
    /// may send this feedback.
    /// </summary>
    positive_high_quality_user_generated_content,

    /// <summary>
    /// Titles that allow users to report phishing message may send this feedback.
    /// </summary>
    comms_phishing,

    /// <summary>
    /// Titles that allow users to report communication based on a picture may send this feedback.
    /// </summary>
    comms_picture_message,

    /// <summary>
    /// Titles that allow users to report spam messages may send this feedback.
    /// </summary>
    comms_spam,

    /// <summary>
    /// Titles that allow users to report text messages may send this feedback.
    /// </summary>
    comms_text_message,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    comms_voice_message,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    fair_play_console_ban_request,

    /// <summary>
    /// Titles that allow users to report if determine if a user stands idle on purpose in a game, usually round after round, may send this feedback.
    /// </summary>
    fair_play_idler,

    /// <summary>
    /// Titles that report a recommendation to ban a user from Xbox Live may send this feedback.
    /// </summary>
    fair_play_user_ban_request,

    /// <summary>
    /// Titles that allow users to report inappropriate gamer picture may send this feedback.
    /// </summary>
    user_content_gamerpic,

    /// <summary>
    /// Titles that allow users to report inappropriate biography and other personal information may send this feedback.
    /// </summary>
    user_content_personalinfo,

    /// <summary>
    /// Titles that allow users to report unsporting behavior may send this feedback.
    /// </summary>
    fair_play_unsporting,

    /// <summary>
    /// Titles that allow users to report leaderboard cheating may send this feedback.
    /// </summary>
    fair_play_leaderboard_cheater
};

enum class social_notification_type
{
    /// <summary>
    /// unknown
    /// </summary>
    unknown,

    /// <summary>
    /// User(s) were added.
    /// </summary>
    added,

    /// <summary>
    /// User(s) data changed.
    /// </summary>
    changed,

    /// <summary>
    /// User(s) were removed.
    /// </summary>
    removed
};

class social_group_constants
{
public:
    /// <summary>
    /// Returns Favorites constant string
    /// </summary>
    static const string_t favorite() { return _T("Favorites"); }

    /// <summary>
    /// Returns People constant string
    /// </summary>
    static const string_t people() { return _T("People"); }
};

/// <summary>
/// Represents the relationship between the user and another Xbox user.
/// </summary>
class xbox_social_relationship
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    xbox_social_relationship(
        _In_ std::shared_ptr<xbox_social_relationship_internal> internalObj
        );

    /// <summary>
    /// The person's Xbox user identifier.
    /// </summary>
    _XSAPIIMP string_t xbox_user_id() const;

    /// <summary>
    /// Indicates whether the person is one that the user cares about more. 
    /// Users can have a very large number of people in their people list, 
    /// favorite people should be prioritized first in experiences and shown before others that are not favorites. 
    /// </summary>
    _XSAPIIMP bool is_favorite() const;

    /// <summary>
    /// Indicates whether the person is following the person that requested the information.
    /// </summary>
    _XSAPIIMP bool is_following_caller() const;

    /// <summary>
    /// A collection of strings indicating which social networks this person has a relationship with. 
    /// </summary>
    _XSAPIIMP std::vector< string_t > social_networks() const;

private:
    std::shared_ptr<xbox_social_relationship_internal> m_internalObj;
};

/// <summary>
/// Services that manage user relationship.
/// </summary>
class xbox_social_relationship_result
{
    // Example:
    // {
    //     "people": [
    //         {
    //             "xuid": "2603643534573573",
    //             "isFavorite": true,
    //             "isFollowingCaller": true,
    //             "socialNetworks": ["MyNetwork1", "MyNetwork2"]
    //         },
    //         {
    //             "xuid": "2603643534573572",
    //             "isFavorite": true,
    //             "isFollowingCaller": false,
    //             "socialNetworks": ["MyNetwork1"]
    //         },
    //         {
    //             "xuid": "2603643534573577",
    //             "isFavorite": false
    //             "isFollowingCaller": false
    //         },
    //     ],
    //     "totalCount": 3
    // }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    xbox_social_relationship_result();

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_social_relationship_result(
        _In_ std::shared_ptr<xbox_social_relationship_result_internal> internalObj
        );

    /// <summary>
    /// Collection of XboxSocialRelationship objects returned by a request.
    /// </summary>
    _XSAPIIMP std::vector<xbox_social_relationship> items() const;

    /// <summary>
    /// The total number of XboxSocialRelationship objects that can be requested.
    /// </summary>
    _XSAPIIMP uint32_t total_count() const;

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of social relationships to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    _XSAPIIMP bool has_next() const;

    /// <summary>
    /// Returns an XboxSocialRelationshipResult object containing the next page.
    /// </summary>
    /// <param name="maxItems">The maximum number of items the response can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>Returns an XboxSocialRelationshipResult object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<xbox_social_relationship_result>> get_next(
        _In_ uint32_t maxItems
        );

private:
    std::shared_ptr<xbox_social_relationship_result_internal> m_internalObj;
};

class social_relationship_change_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    social_relationship_change_event_args(
        _In_ std::shared_ptr<social_relationship_change_event_args_internal> internalObj
        );

    /// <summary>
    /// The Xbox user ID for the user who's social graph changes are being listed for.
    /// </summary>
    _XSAPIIMP string_t caller_xbox_user_id() const;

    /// <summary>
    /// The type of notification change.
    /// </summary>
    _XSAPIIMP social_notification_type social_notification() const;

    /// <summary>
    /// The Xbox user ids who the event is for
    /// </summary>
    _XSAPIIMP std::vector<string_t> xbox_user_ids() const;

private:
    std::shared_ptr<social_relationship_change_event_args_internal> m_internalObj;
};

class social_relationship_change_subscription : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    social_relationship_change_subscription(
        _In_ std::shared_ptr<social_relationship_change_subscription_internal> internalObj
        );

    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    _XSAPIIMP string_t xbox_user_id() const;

    // TODO remove these after migrating real time activity service
    /// <summary>The state of the subscription request.</summary>
    _XSAPIIMP virtual xbox::services::real_time_activity::real_time_activity_subscription_state state() const override;

    /// <summary>The resource uri for the request.</summary>
    _XSAPIIMP virtual const string_t& resource_uri() const override;

    /// <summary>The unique subscription id for the request.</summary>
    _XSAPIIMP virtual uint32_t subscription_id() const override;

private:
    std::shared_ptr<social_relationship_change_subscription_internal> m_internalObj;

    friend social_service;
};

/// <summary>
/// Services that manage user relationship.
/// </summary>
class social_service
{
public:
    /// <summary>
    /// Returns a XboxSocialRelationshipResult containing a the list of people that the user is connected to.
    /// Defaults to filtering to PersonView.All.
    /// Defaults to startIndex and maxItems of 0 to return entire list if possible.
    /// </summary>
    /// <returns>An XboxSocialRelationshipResult object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships();

    /// <summary>
    /// Returns a xbox_social_relationship_result containing a the list of people that the user is connected to.
    /// </summary>
    /// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
    /// <returns>An xbox_social_relationship_result object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ xbox_social_relationship_filter socialRelationshipFilter
        );

    /// <summary>
    /// Returns a xbox_social_relationship_result containing a the list of people that the user is connected to.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User Id to get the social relationships for.</param>
    /// <returns>An xbox_social_relationship_result object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Returns a xbox_social_relationship_result containing a the list of people that the user is connected to.
    /// </summary>
    /// <param name="socialRelationshipFilter">Controls how the list is filtered.</param>
    /// <param name="startIndex">Controls the starting index to return.</param>
    /// <param name="maxItems">Controls the number of xbox_social_relationship_result objects to get.  0 will return as many as possible</param>
    /// <returns>An xbox_social_relationship_result object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ xbox_social_relationship_filter socialRelationshipFilter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Subscribes to the social service for people changed events
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
    /// <returns>
    /// You can register an event handler for social relationship changes by calling set_social_changed_handler().
    /// </returns>
    _XSAPIIMP xbox_live_result<std::shared_ptr<social_relationship_change_subscription>> subscribe_to_social_relationship_change(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Unsubscribes a previously created social relationship change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    _XSAPIIMP xbox_live_result<void> unsubscribe_from_social_relationship_change(
        _In_ std::shared_ptr<social_relationship_change_subscription> subscription
        );

    /// <summary>
    /// Registers an event handler for social relationship change notifications.
    /// Event handlers receive social_relationship_change_event_args.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    _XSAPIIMP function_context add_social_relationship_changed_handler(
        _In_ std::function<void(social_relationship_change_event_args)> handler
        );

    /// <summary>
    /// Removes a social relationship change handler
    /// </summary>
    /// <param name="context">The handler to remove.</param>
    _XSAPIIMP void remove_social_relationship_changed_handler(
        _In_ function_context context
        );

    std::shared_ptr<xbox_live_context_settings> _Xbox_live_context_settings() { return m_xboxLiveContextSettings; }

private:
    social_service() {};

    social_service(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> contextSettings,
        _In_ std::shared_ptr<social_service_impl> serviceImpl
        );

    pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ const string_t& xboxUserId,
        _In_ xbox_social_relationship_filter socialRelationshipFilter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems
        );

    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;

    std::shared_ptr<social_service_impl> m_socialServiceImpl;

    friend xbox_live_context;
};

/// <summary>
/// Represents the parameters for submitting reputation feedback on a user
/// </summary>
class reputation_feedback_item
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    reputation_feedback_item();

    /// <summary>
    /// Construct a reputation_feedback_item object
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
    /// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
    /// <param name="sessionRef">The session reference of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
    /// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
    /// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
    reputation_feedback_item(
        _In_ string_t xboxUserId,
        _In_ reputation_feedback_type reputationFeedbackType,
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef = xbox::services::multiplayer::multiplayer_session_reference(),
        _In_ string_t reasonMessage = string_t(),
        _In_ string_t evidenceResourceId = string_t()
        );

    /// <summary>
    /// The Xbox User ID of the user that reputation feedback is being submitted on.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// The reputation feedback type being submitted.
    /// </summary>
    _XSAPIIMP reputation_feedback_type feedback_type() const;

    /// <summary>
    /// The reference to the multiplayer session directory session the user is sending feedback from.
    /// </summary>
    _XSAPIIMP const xbox::services::multiplayer::multiplayer_session_reference& session_reference() const;

    /// <summary>
    /// User supplied text added to explain the reason for the feedback.
    /// </summary>
    _XSAPIIMP const string_t& reason_message() const;

    /// <summary>
    /// The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file.
    /// </summary>
    _XSAPIIMP const string_t& evidence_resource_id() const;

private:
    string_t m_xboxUserId;
    reputation_feedback_type m_reputationFeedbackType;
    xbox::services::multiplayer::multiplayer_session_reference m_sessionRef;
    string_t m_reasonMessage;
    string_t m_evidenceResourceId;
};


/// <summary>
/// Manages the reputation service.
/// </summary>
class reputation_service
{
public:
    /// <summary>
    /// Submits reputation feedback on the specified user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user that reputation feedback is being submitted on.</param>
    /// <param name="reputationFeedbackType">The reputation feedback type being submitted.</param>
    /// <param name="sessionName">The name of the multiplayer session directory session the user is sending feedback from. (Optional)</param>
    /// <param name="reasonMessage">User supplied text added to explain the reason for the feedback. (Optional)</param>
    /// <param name="evidenceResourceId">The Id of a resource that can be used as evidence for the feedback. Example: the Id of a video file. (Optional)</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V100 POST /users/xuid({xuid})/feedback</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<void>> submit_reputation_feedback(
        _In_ const string_t& xboxUserId,
        _In_ reputation_feedback_type reputationFeedbackType,
        _In_ const string_t& sessionName = string_t(),
        _In_ const string_t& reasonMessage = string_t(),
        _In_ const string_t& evidenceResourceId = string_t()
        );

    /// <summary>
    /// Submits batch reputation feedback on the specified users.
    /// </summary>
    /// <param name="feedbackItems">A vector of reputation_feedback_item objects to submit reputation feedback on.</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V101 POST /users/batchfeedback</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<void>> submit_batch_reputation_feedback(
        _In_ const std::vector< reputation_feedback_item >& feedbackItems
        );

private:
    reputation_service() {};

    reputation_service(_In_ std::shared_ptr<reputation_service_impl> serviceImpl);

    std::shared_ptr<reputation_service_impl> m_serviceImpl;

    friend xbox_live_context;
};

}}}
