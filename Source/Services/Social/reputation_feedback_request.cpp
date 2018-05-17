// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "utils.h"
#include "social_internal.h"

using namespace xbox::services::social;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

reputation_feedback_request::reputation_feedback_request(
    _In_ reputation_feedback_type feedbackType,
    _In_ xsapi_internal_string sessionName,
    _In_ xsapi_internal_string reasonMessage,
    _In_ xsapi_internal_string evidenceResourceId
    ) :
    m_feedbackType(feedbackType),
    m_sessionName(std::move(sessionName)),
    m_reasonMessage(std::move(reasonMessage)),
    m_evidenceResourceId(std::move(evidenceResourceId))
{
}

web::json::value
reputation_feedback_request::serialize_batch_feedback_request(
    _In_ const xsapi_internal_vector<reputation_feedback_item_internal>& feedbackItems,
    _Out_ std::error_code& err
    )
{
    web::json::value itemArrayJson = web::json::value::array();
    uint32_t i = 0;
    err = xbox_live_error_code::no_error;

    for (auto& feedbackItem : feedbackItems)
    {
        // Example:
        //
        // "targetXuid": "33445566778899",
        // "titleId" : "6487",
        // "sessionRef" :
        // {
        //     "scid": "1234-1234-471F-B696-07B61F09EC20",
        //     "templateName" : "CaptureFlag5",
        //     "name" : "Example556932",
        // },
        // "feedbackType": "FairPlayKillsTeammates",
        // "textReason" : "Killed 19 team members in a single session",
        // "evidenceId" : null

        xbox_live_result<xsapi_internal_string> feedbackTypeToString = convert_reputation_feedback_type_to_string(feedbackItem.feedback_type());
        err = feedbackTypeToString.err();
        if (err)
        {
            break;
        }

        web::json::value itemJson;
        itemJson[_T("targetXuid")] = web::json::value::string(utils::string_t_from_internal_string(feedbackItem.xbox_user_id()));
        itemJson[_T("titleId")] = web::json::value::null();

        web::json::value sessionRefJson;
        if (!feedbackItem.session_reference().is_null())
        {
            sessionRefJson[_T("scid")] = web::json::value::string(feedbackItem.session_reference().service_configuration_id());
            sessionRefJson[_T("templateName")] = web::json::value::string(feedbackItem.session_reference().session_template_name());
            sessionRefJson[_T("name")] = web::json::value::string(feedbackItem.session_reference().session_name());
            itemJson[_T("sessionRef")] = sessionRefJson;
        }
        else
        {
            itemJson[_T("sessionRef")] = web::json::value::null();
        }

        itemJson[_T("feedbackType")] = web::json::value::string(utils::string_t_from_internal_string(feedbackTypeToString.payload()));
        itemJson[_T("textReason")] = web::json::value::string(utils::string_t_from_internal_string(feedbackItem.reason_message()));
        if (!feedbackItem.evidence_resource_id().empty())
        {
            itemJson[_T("evidenceId")] = web::json::value::string(utils::string_t_from_internal_string(feedbackItem.evidence_resource_id()));
        }
        else
        {
            itemJson[_T("evidenceId")] = web::json::value::null();
        }

        itemArrayJson[i++] = itemJson;
    }

    web::json::value request;
    request[_T("items")] = itemArrayJson;
    return request;
}

web::json::value 
reputation_feedback_request::serialize_feedback_request()
{
    web::json::value request;
    xbox_live_result<xsapi_internal_string> feedbackTypeToString = convert_reputation_feedback_type_to_string(m_feedbackType);
    if (!feedbackTypeToString.err())
    {
        request[_T("sessionName")] = web::json::value::string(utils::string_t_from_internal_string(m_sessionName));
        request[_T("feedbackType")] = web::json::value::string(utils::string_t_from_internal_string(feedbackTypeToString.payload()));
        request[_T("textReason")] = web::json::value::string(utils::string_t_from_internal_string(m_reasonMessage));
        if (!m_evidenceResourceId.empty())
        {
            request[_T("evidenceId")] = web::json::value::string(utils::string_t_from_internal_string(m_evidenceResourceId));
        }
        else
        {
            request[_T("evidenceId")] = web::json::value::null();
        }
    }
    
    return request;
}

const xbox_live_result<xsapi_internal_string>
reputation_feedback_request::convert_reputation_feedback_type_to_string(reputation_feedback_type feedbackType)
{
    switch (feedbackType)
    {
        case reputation_feedback_type::fair_play_kills_teammates: return xbox_live_result<xsapi_internal_string>("FairPlayKillsTemmates");
        case reputation_feedback_type::fair_play_cheater: return xbox_live_result<xsapi_internal_string>("FairPlayCheater");
        case reputation_feedback_type::fair_play_tampering: return xbox_live_result<xsapi_internal_string>("FairPlayTampering");
        case reputation_feedback_type::fair_play_quitter: return xbox_live_result<xsapi_internal_string>("FairPlayQuitter");
        case reputation_feedback_type::fair_play_kicked: return xbox_live_result<xsapi_internal_string>("FairPlayKicked");
        case reputation_feedback_type::communications_inappropiate_video: return xbox_live_result<xsapi_internal_string>("CommsInappropriateVideo");
        case reputation_feedback_type::communications_abusive_voice: return xbox_live_result<xsapi_internal_string>("CommsAbusiveVoice");
        case reputation_feedback_type::inappropiate_user_generated_content: return xbox_live_result<xsapi_internal_string>("UserContentInappropriateUGC");
        case reputation_feedback_type::positive_skilled_player: return xbox_live_result<xsapi_internal_string>("PositiveSkilledPlayer");
        case reputation_feedback_type::positive_helpful_player: return xbox_live_result<xsapi_internal_string>("PositiveHelpfulPlayer");
        case reputation_feedback_type::positive_high_quality_user_generated_content: return xbox_live_result<xsapi_internal_string>("PositiveHighQualityUGC");
        case reputation_feedback_type::comms_phishing: return xbox_live_result<xsapi_internal_string>("CommsPhishing");
        case reputation_feedback_type::comms_picture_message: return xbox_live_result<xsapi_internal_string>("CommsPictureMessage");
        case reputation_feedback_type::comms_spam: return xbox_live_result<xsapi_internal_string>("CommsSpam");
        case reputation_feedback_type::comms_text_message: return xbox_live_result<xsapi_internal_string>("CommsTextMessage");
        case reputation_feedback_type::comms_voice_message: return xbox_live_result<xsapi_internal_string>("CommsVoiceMessage");
        case reputation_feedback_type::fair_play_console_ban_request: return xbox_live_result<xsapi_internal_string>("FairPlayConsoleBanRequest");
        case reputation_feedback_type::fair_play_idler: return xbox_live_result<xsapi_internal_string>("FairPlayIdler");
        case reputation_feedback_type::fair_play_user_ban_request: return xbox_live_result<xsapi_internal_string>("FairPlayUserBanRequest");
        case reputation_feedback_type::user_content_gamerpic: return xbox_live_result<xsapi_internal_string>("UserContentGamertag");
        case reputation_feedback_type::user_content_personalinfo: return xbox_live_result<xsapi_internal_string>("UserContentPersonalInfo");
        case reputation_feedback_type::fair_play_unsporting: return xbox_live_result<xsapi_internal_string>("FairplayUnsporting");
        case reputation_feedback_type::fair_play_leaderboard_cheater: return xbox_live_result<xsapi_internal_string>("FairplayLeaderboardCheater");

        default: return xbox_live_result<xsapi_internal_string>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

reputation_feedback_item::reputation_feedback_item()
{
}

reputation_feedback_item::reputation_feedback_item(
    _In_ string_t xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
    _In_ string_t reasonMessage,
    _In_ string_t evidenceResourceId
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_reputationFeedbackType(reputationFeedbackType),
    m_sessionRef(std::move(sessionRef)),
    m_reasonMessage(std::move(reasonMessage)),
    m_evidenceResourceId(std::move(evidenceResourceId))
{
}

const string_t&
reputation_feedback_item::xbox_user_id() const
{
    return m_xboxUserId;
}

reputation_feedback_type
reputation_feedback_item::feedback_type() const
{
    return m_reputationFeedbackType;
}

const xbox::services::multiplayer::multiplayer_session_reference&
reputation_feedback_item::session_reference() const
{
    return m_sessionRef;
}

const string_t&
reputation_feedback_item::reason_message() const
{
    return m_reasonMessage;
}

const string_t&
reputation_feedback_item::evidence_resource_id() const
{
    return m_evidenceResourceId;
}


reputation_feedback_item_internal::reputation_feedback_item_internal(
    _In_ const reputation_feedback_item& legacyCppObj
    ) :
    m_xboxUserId(utils::internal_string_from_string_t(legacyCppObj.xbox_user_id())),
    m_reputationFeedbackType(legacyCppObj.feedback_type()),
    m_sessionRef(legacyCppObj.session_reference()),
    m_reasonMessage(utils::internal_string_from_string_t(legacyCppObj.reason_message())),
    m_evidenceResourceId(utils::internal_string_from_string_t(legacyCppObj.evidence_resource_id()))
{
}

reputation_feedback_item_internal::reputation_feedback_item_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
    _In_ xsapi_internal_string reasonMessage,
    _In_ xsapi_internal_string evidenceResourceId
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_reputationFeedbackType(reputationFeedbackType),
    m_sessionRef(std::move(sessionRef)),
    m_reasonMessage(std::move(reasonMessage)),
    m_evidenceResourceId(std::move(evidenceResourceId))
{
}

const xsapi_internal_string&
reputation_feedback_item_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

reputation_feedback_type
reputation_feedback_item_internal::feedback_type() const
{
    return m_reputationFeedbackType;
}

const xbox::services::multiplayer::multiplayer_session_reference& 
reputation_feedback_item_internal::session_reference() const
{
    return m_sessionRef;
}

const xsapi_internal_string&
reputation_feedback_item_internal::reason_message() const
{
    return m_reasonMessage;
}

const xsapi_internal_string&
reputation_feedback_item_internal::evidence_resource_id() const
{
    return m_evidenceResourceId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END