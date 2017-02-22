// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi\social.h"
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

/// <summary> Defines values for reputation feedback types.</summary>
public enum class ReputationFeedbackType
{
    /// <summary>
    /// Titles that are able to automatically determine that a user kills a teammate
    /// may send this feedback without user intervention.
    ///</summary>
    FairPlayKillsTeammates = xbox::services::social::reputation_feedback_type::fair_play_kills_teammates,

    /// <summary>
    /// Titles that are able to automatically determine that a user is cheating
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayCheater = xbox::services::social::reputation_feedback_type::fair_play_cheater,

    /// <summary>
    /// Titles that are able to automatically determine that a user has tampered with on-disk content
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayTampering = xbox::services::social::reputation_feedback_type::fair_play_tampering,

    /// <summary>
    /// Titles that are able to automatically determine that a user quit a game early
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayQuitter = xbox::services::social::reputation_feedback_type::fair_play_quitter,

    /// <summary>
    /// When a user is voted out of a game (kicked), titles
    /// may send this feedback without user intervention.
    /// </summary>
    FairPlayKicked = xbox::services::social::reputation_feedback_type::fair_play_kicked,

    /// <summary>
    /// Titles that allow users to report innappropriate video communications
    /// may send this feedback.
    /// </summary>
    CommunicationsInappropriateVideo = xbox::services::social::reputation_feedback_type::communications_inappropiate_video,

    /// <summary>
    /// Titles that allow users to report innappropriate voice communications
    /// may send this feedback.
    /// </summary>
    CommunicationsAbusiveVoice = xbox::services::social::reputation_feedback_type::communications_abusive_voice,

    /// <summary>
    /// Titles that allow users to report innappropriate user generated content
    /// may send this feedback.
    /// </summary>
    InappropriateUserGeneratedContent = xbox::services::social::reputation_feedback_type::inappropiate_user_generated_content,

    /// <summary>
    /// Titles that allow users to vote on a most valuable player at the end of a multiplayer session
    /// may send this feedback.
    /// </summary>
    PositiveSkilledPlayer = xbox::services::social::reputation_feedback_type::positive_skilled_player,

    /// <summary>
    /// Titles that allow users to submit positive feedback on helpful fellow players
    /// may send this feedback.
    /// </summary>
    PositiveHelpfulPlayer = xbox::services::social::reputation_feedback_type::positive_helpful_player,

    /// <summary>
    /// Titles that allow users to submit positive feedback on shared user generated content
    /// may send this feedback.
    /// </summary>
    PositiveHighQualityUserGeneratedContent = xbox::services::social::reputation_feedback_type::positive_high_quality_user_generated_content,

    /// <summary>
    /// Titles that allow users to report phishing message may send this feedback.
    /// </summary>
    CommsPhishing = xbox::services::social::reputation_feedback_type::comms_phishing,

    /// <summary>
    /// Titles that allow users to report communication based on a picture may send this feedback.
    /// </summary>
    CommsPictureMessage = xbox::services::social::reputation_feedback_type::comms_picture_message,

    /// <summary>
    /// Titles that allow users to report spam messages may send this feedback.
    /// </summary>
    CommsSpam = xbox::services::social::reputation_feedback_type::comms_spam,

    /// <summary>
    /// Titles that allow users to report text messages may send this feedback.
    /// </summary>
    CommsTextMessage = xbox::services::social::reputation_feedback_type::comms_text_message,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    CommsVoiceMessage = xbox::services::social::reputation_feedback_type::comms_voice_message,

    /// <summary>
    /// Titles that allow users to report voice messages may send this feedback.
    /// </summary>
    FairPlayConsoleBanRequest = xbox::services::social::reputation_feedback_type::fair_play_console_ban_request,

    /// <summary>
    /// Titles that allow users to report if determine if a user stands idle on purpose in a game, usually round after round, may send this feedback.
    /// </summary>
    FairPlayIdler = xbox::services::social::reputation_feedback_type::fair_play_idler,

    /// <summary>
    /// Titles that report a recommendation to ban a user from Xbox Live may send this feedback.
    /// </summary>
    FairPlayUserBanRequest = xbox::services::social::reputation_feedback_type::fair_play_user_ban_request,

    /// <summary>
    /// Titles that allow users to report inappropriate gamer picture may send this feedback.
    /// </summary>
    UserContentGamerpic = xbox::services::social::reputation_feedback_type::user_content_gamerpic,

    /// <summary>
    /// Titles that allow users to report inappropriate biography and other personal information may send this feedback.
    /// </summary>
    UserContentPersonalinfo = xbox::services::social::reputation_feedback_type::user_content_personalinfo,

    /// <summary>
    /// Titles that allow users to report unsporting behavior may send this feedback.
    /// </summary>
    FairPlayUnsporting = xbox::services::social::reputation_feedback_type::fair_play_unsporting,

    /// <summary>
    /// Titles that allow users to report leaderboard cheating may send this feedback.
    /// </summary>
    FairPlayLeaderboardCheater = xbox::services::social::reputation_feedback_type::fair_play_leaderboard_cheater
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END
