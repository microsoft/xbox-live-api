// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

ReputationFeedbackRequest::ReputationFeedbackRequest(
    _In_ uint64_t xuid,
    _In_ XblReputationFeedbackType feedbackType,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_z_ const char* reasonMessage,
    _In_opt_z_ const char* evidenceResourceId
)
{
    xsapi_internal_stringstream pathAndQuery;
    pathAndQuery << "/users/xuid(" << xuid << ")/feedback";
    m_pathAndQuery = pathAndQuery.str();

    m_requestBody.SetObject();
    JsonDocument::AllocatorType& allocator = m_requestBody.GetAllocator();

    if (sessionReference)
    {
        JsonValue sessionRefJson;
        multiplayer::Serializers::SerializeSessionReference(*sessionReference, sessionRefJson, allocator);
        m_requestBody.AddMember("sessionRef", sessionRefJson, allocator);
    }
    else
    {
        m_requestBody.AddMember("sessionRef", JsonValue(rapidjson::kNullType), allocator);
    }
    xsapi_internal_string feedbackTypeString = ReputationFeedbackTypeToString(feedbackType);
    m_requestBody.AddMember("feedbackType", JsonValue(feedbackTypeString.c_str(), allocator).Move(), allocator);
    m_requestBody.AddMember("textReason", JsonValue(reasonMessage, allocator).Move(), allocator);
    
    if (evidenceResourceId)
    {
        m_requestBody.AddMember("evidenceId", JsonValue(evidenceResourceId, allocator).Move(), allocator);
    }
    else
    {
        m_requestBody.AddMember("evidenceId", JsonValue(rapidjson::kNullType), allocator);
    }
}

ReputationFeedbackRequest::ReputationFeedbackRequest(
    _In_ const XblReputationFeedbackItem* items,
    _In_ size_t itemsCount
) :
    m_pathAndQuery{ "/users/batchtitlefeedback" }
{
    m_requestBody.SetObject();
    JsonDocument::AllocatorType& allocator = m_requestBody.GetAllocator();

    JsonValue itemArrayJson(rapidjson::kArrayType);
    for (size_t i = 0; i < itemsCount; ++i)
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

        JsonValue itemJson(rapidjson::kObjectType);
        itemJson.AddMember("targetXuid", JsonValue(utils::uint64_to_internal_string(items[i].xboxUserId).c_str(), allocator).Move(), allocator);
        itemJson.AddMember("titleId", JsonValue(rapidjson::kNullType), allocator);

        if (items[i].sessionReference)
        {
            JsonValue sessionRefJson;
            multiplayer::Serializers::SerializeSessionReference(*items[i].sessionReference, sessionRefJson, allocator);
            itemJson.AddMember("sessionRef", sessionRefJson, allocator);
        }
        else
        {
            itemJson.AddMember("sessionRef", JsonValue(rapidjson::kNullType), allocator);
        }
        xsapi_internal_string feedbackTypeString = ReputationFeedbackTypeToString(items[i].feedbackType);
        itemJson.AddMember("feedbackType", JsonValue(feedbackTypeString.c_str(), allocator).Move(), allocator);
        itemJson.AddMember("textReason", JsonValue(items[i].reasonMessage, allocator).Move(), allocator);
        if (items[i].evidenceResourceId)
        {
            itemJson.AddMember("evidenceId", JsonValue(items[i].evidenceResourceId, allocator).Move(), allocator);
        }
        else
        {
            itemJson.AddMember("evidenceId", JsonValue(rapidjson::kNullType), allocator);
        }

        itemArrayJson.PushBack(itemJson, allocator);
    }

    m_requestBody.AddMember("items", itemArrayJson, allocator);
}

ReputationFeedbackRequest::ReputationFeedbackRequest(const ReputationFeedbackRequest& other)
{
    m_pathAndQuery = other.m_pathAndQuery;
    JsonUtils::CopyFrom(m_requestBody, other.m_requestBody);
}

const xsapi_internal_string& ReputationFeedbackRequest::PathAndQuery() const noexcept
{
    return m_pathAndQuery;
}

const JsonValue& ReputationFeedbackRequest::Body() const noexcept
{
    return m_requestBody;
}

xsapi_internal_string ReputationFeedbackRequest::ReputationFeedbackTypeToString(
    XblReputationFeedbackType feedbackType
)
{
    switch (feedbackType)
    {
        case XblReputationFeedbackType::FairPlayKillsTeammates: return "FairPlayKillsTeammates";
        case XblReputationFeedbackType::FairPlayCheater: return "FairPlayCheater";
        case XblReputationFeedbackType::FairPlayTampering: return "FairPlayTampering";
        case XblReputationFeedbackType::FairPlayQuitter: return "FairPlayQuitter";
        case XblReputationFeedbackType::FairPlayKicked : return "FairPlayKicked";
        case XblReputationFeedbackType::CommunicationsInappropriateVideo: return "CommsInappropriateVideo";
        case XblReputationFeedbackType::CommunicationsAbusiveVoice: return "CommsAbusiveVoice";
        case XblReputationFeedbackType::InappropriateUserGeneratedContent: return "UserContentInappropriateUGC";
        case XblReputationFeedbackType::PositiveSkilledPlayer: return "PositiveSkilledPlayer";
        case XblReputationFeedbackType::PositiveHelpfulPlayer: return "PositiveHelpfulPlayer";
        case XblReputationFeedbackType::PositiveHighQualityUserGeneratedContent: return "PositiveHighQualityUGC";
        case XblReputationFeedbackType::CommsPhishing: return "CommsPhishing";
        case XblReputationFeedbackType::CommsPictureMessage: return "CommsPictureMessage";
        case XblReputationFeedbackType::CommsSpam: return "CommsSpam";
        case XblReputationFeedbackType::CommsTextMessage: return "CommsTextMessage";
        case XblReputationFeedbackType::CommsVoiceMessage: return "CommsVoiceMessage";
        case XblReputationFeedbackType::FairPlayConsoleBanRequest: return "FairPlayConsoleBanRequest";
        case XblReputationFeedbackType::FairPlayIdler: return "FairPlayIdler";
        case XblReputationFeedbackType::FairPlayUserBanRequest: return "FairPlayUserBanRequest";
        case XblReputationFeedbackType::UserContentGamerpic: return "UserContentGamertag";
        case XblReputationFeedbackType::UserContentPersonalInfo: return "UserContentPersonalInfo";
        case XblReputationFeedbackType::FairPlayUnsporting: return "FairplayUnsporting";
        case XblReputationFeedbackType::FairPlayLeaderboardCheater: return "FairplayLeaderboardCheater";

        default:
        {
            assert(false);
            return xsapi_internal_string{};
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END