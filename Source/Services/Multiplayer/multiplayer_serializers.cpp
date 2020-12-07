// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

Result<XblMultiplayerActivityDetails> Serializers::DeserializeMultiplayerActivityDetails(
    _In_ const JsonValue& json
)
{
    HRESULT errc = S_OK;

    XblMultiplayerActivityDetails returnResult{};
    XSAPI_ASSERT(!json.IsNull());

    xsapi_internal_string type;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", type));
    if (type.compare("activity") != 0)
    {
        errc = WEB_E_INVALID_JSON_STRING;
        return returnResult;
    }

    xsapi_internal_string id;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", id));
    utils::strcpy(returnResult.HandleId, sizeof(returnResult.HandleId), id.data());
    if (json.IsObject() && json.HasMember("sessionRef"))
    {
        returnResult.SessionReference = DeserializeSessionReference(json["sessionRef"]).Payload();
    }
    else
    {
        returnResult.SessionReference = XblMultiplayerSessionReference();
    }
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "ownerXuid", returnResult.OwnerXuid));
    xsapi_internal_string titleId;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "titleId", titleId));
    returnResult.TitleId = utils::internal_string_to_uint32(titleId);

    xsapi_internal_string customProperties = JsonUtils::SerializeJson(json["customProperties"]);
    returnResult.CustomSessionPropertiesJson = Make(customProperties.c_str());
    
    if (json.IsObject() && json.HasMember("relatedInfo"))
    {
        const JsonValue& relatedInfoObject = json["relatedInfo"];
        if (!relatedInfoObject.IsNull())
        {
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(relatedInfoObject, "membersCount", returnResult.MembersCount));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(relatedInfoObject, "maxMembersCount", returnResult.MaxMembersCount));
            xsapi_internal_string joinRestriction;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(relatedInfoObject, "joinRestriction", joinRestriction));
            returnResult.JoinRestriction = MultiplayerSessionRestrictionFromString(joinRestriction);
            xsapi_internal_string visibility;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(relatedInfoObject, "visibility", visibility));
            returnResult.Visibility = MultiplayerSessionVisibilityFromString(visibility);
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(relatedInfoObject, "closed", returnResult.Closed));
        }
    }

    return returnResult;
}

Result<XblMultiplayerSessionQueryResult> Serializers::DeserializeMultiplayerSessionQueryResult(
    _In_ const JsonValue& json
)
{
    if (json.IsNull())
    {
        return E_INVALIDARG;
    }
    XblMultiplayerSessionQueryResult returnResult{};

    if (json.IsObject() && json.HasMember("sessionRef"))
    {
        const JsonValue& sessionRefJson = json["sessionRef"];
        returnResult.SessionReference = DeserializeSessionReference(sessionRefJson).Payload();
    }
    else
    {
        returnResult.SessionReference = XblMultiplayerSessionReference();
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "myTurn", returnResult.IsMyTurn));
    xsapi_internal_string sessionStatus;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "status", sessionStatus));
    if (!sessionStatus.empty())
    {
        returnResult.Status = MultiplayerSessionStatusFromString(std::move(sessionStatus));
    }

    xsapi_internal_string sessionVisibility;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "visibility", sessionVisibility));
    if (!sessionVisibility.empty())
    {
        returnResult.Visibility = MultiplayerSessionVisibilityFromString(sessionVisibility);
    }

    xsapi_internal_string joinRestriction;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "joinRestriction", joinRestriction));
    if (!joinRestriction.empty())
    {
        returnResult.JoinRestriction = MultiplayerSessionRestrictionFromString(joinRestriction);
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "startTime", returnResult.StartTime));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", returnResult.Xuid));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "accepted", returnResult.AcceptedMemberCount));

    return returnResult;
}

Result<XblMultiplayerSessionReference> Serializers::DeserializeSessionReference(
    _In_ const JsonValue& json
)
{
    XblMultiplayerSessionReference result{};
    if (json.IsNull())
    {
        return result;
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "scid", result.Scid, sizeof(result.Scid)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "templateName", result.SessionTemplateName, sizeof(result.SessionTemplateName)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "name", result.SessionName, sizeof(result.SessionName)));

    return result;
}

void Serializers::SerializeSessionReference(const XblMultiplayerSessionReference& sessionReference, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    json.SetObject();
    json.AddMember("scid", JsonValue(sessionReference.Scid, allocator).Move(), allocator);
    json.AddMember("templateName", JsonValue(sessionReference.SessionTemplateName, allocator).Move(), allocator);
    json.AddMember("name", JsonValue(sessionReference.SessionName, allocator).Move(), allocator);
}

Result<XblMultiplayerInviteHandle> Serializers::DeserializeMultiplayerInvite(
    _In_ const JsonValue& json
)
{
    XblMultiplayerInviteHandle returnResult{};
    if (json.IsNull())
    {
        return returnResult;
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "id", returnResult.Data, sizeof(returnResult.Data)));
    return returnResult;
}

XblMultiplayerSessionRestriction Serializers::MultiplayerSessionRestrictionFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "none") == 0)
    {
        return XblMultiplayerSessionRestriction::None;
    }
    else if (utils::str_icmp_internal(value, "local") == 0)
    {
        return XblMultiplayerSessionRestriction::Local;
    }
    else if (utils::str_icmp_internal(value, "followed") == 0)
    {
        return XblMultiplayerSessionRestriction::Followed;
    }

    return XblMultiplayerSessionRestriction::Unknown;
}

xsapi_internal_string Serializers::StringFromMultiplayerSessionRestriction(
    _In_ XblMultiplayerSessionRestriction joinRestriction
)
{
    switch (joinRestriction)
    {
    case XblMultiplayerSessionRestriction::Unknown: return "unknown";
    case XblMultiplayerSessionRestriction::None: return "none";
    case XblMultiplayerSessionRestriction::Local: return "local";
    case XblMultiplayerSessionRestriction::Followed: return "followed";
    default:
    {
        XSAPI_ASSERT(false);
        return "unknown";
    }
    }
}

XblMultiplayerSessionStatus Serializers::MultiplayerSessionStatusFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "active") == 0)
    {
        return XblMultiplayerSessionStatus::Active;
    }
    else if (utils::str_icmp_internal(value, "inactive") == 0)
    {
        return XblMultiplayerSessionStatus::Inactive;
    }
    else if (utils::str_icmp_internal(value, "reserved") == 0)
    {
        return XblMultiplayerSessionStatus::Reserved;
    }

    return XblMultiplayerSessionStatus::Unknown;
}

XblMultiplayerSessionVisibility Serializers::MultiplayerSessionVisibilityFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "private") == 0)
    {
        return XblMultiplayerSessionVisibility::PrivateSession;
    }
    else if (utils::str_icmp_internal(value, "visible") == 0)
    {
        return XblMultiplayerSessionVisibility::Visible;
    }
    else if (utils::str_icmp_internal(value, "full") == 0)
    {
        return XblMultiplayerSessionVisibility::Full;
    }
    else if (utils::str_icmp_internal(value, "open") == 0)
    {
        return XblMultiplayerSessionVisibility::Open;
    }

    return XblMultiplayerSessionVisibility::Unknown;
}

xsapi_internal_string Serializers::StringFromMultiplayerSessionVisibility(_In_ XblMultiplayerSessionVisibility sessionVisibility)
{
    switch (sessionVisibility)
    {
    case XblMultiplayerSessionVisibility::Unknown: return "unknown";
    case XblMultiplayerSessionVisibility::Any: return "any";
    case XblMultiplayerSessionVisibility::PrivateSession: return "private";
    case XblMultiplayerSessionVisibility::Visible: return "visible";
    case XblMultiplayerSessionVisibility::Full: return "full";
    case XblMultiplayerSessionVisibility::Open: return "open";
    default:
    {
        XSAPI_ASSERT(false);
        return "unknown";
    }
    }
}

XblNetworkAddressTranslationSetting Serializers::MultiplayerNatSettingFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (value.empty())
    {
        return XblNetworkAddressTranslationSetting::Unknown;
    }
    else if (utils::str_icmp_internal(value, "strict") == 0)
    {
        return XblNetworkAddressTranslationSetting::Strict;
    }
    else if (utils::str_icmp_internal(value, "moderate") == 0)
    {
        return XblNetworkAddressTranslationSetting::Moderate;
    }
    else if (utils::str_icmp_internal(value, "open") == 0)
    {
        return XblNetworkAddressTranslationSetting::Open;
    }

    return XblNetworkAddressTranslationSetting::Unknown;
}

XblMultiplayerMeasurementFailure Serializers::MultiplayerMeasurementFailureFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (value.empty())
    {
        return XblMultiplayerMeasurementFailure::None;
    }
    else if (utils::str_icmp_internal(value, "bandwidthUp") == 0)
    {
        return XblMultiplayerMeasurementFailure::BandwidthUp;
    }
    else if (utils::str_icmp_internal(value, "bandwidthDown") == 0)
    {
        return XblMultiplayerMeasurementFailure::BandwidthDown;
    }
    else if (utils::str_icmp_internal(value, "latency") == 0)
    {
        return XblMultiplayerMeasurementFailure::Latency;
    }
    else if (utils::str_icmp_internal(value, "timeout") == 0)
    {
        return XblMultiplayerMeasurementFailure::Timeout;
    }
    else if (utils::str_icmp_internal(value, "group") == 0)
    {
        return XblMultiplayerMeasurementFailure::Group;
    }
    else if (utils::str_icmp_internal(value, "network") == 0)
    {
        return XblMultiplayerMeasurementFailure::Network;
    }
    else if (utils::str_icmp_internal(value, "episode") == 0)
    {
        return XblMultiplayerMeasurementFailure::Episode;
    }

    return XblMultiplayerMeasurementFailure::Unknown;
}

XblMultiplayerSessionChangeTypes Serializers::MultiplayerSessionChangeTypesFromStringVector(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& changeTypeList
)
{
    XblMultiplayerSessionChangeTypes resultingChangeTypes = XblMultiplayerSessionChangeTypes::None;
    for (auto& current : changeTypeList)
    {
        if (utils::str_icmp_internal(current, "everything") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::Everything;
        }
        else if (utils::str_icmp_internal(current, "host") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::HostDeviceTokenChange;
        }
        else if (utils::str_icmp_internal(current, "initialization") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::InitializationStateChange;
        }
        else if (utils::str_icmp_internal(current, "matchmakingStatus") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::MatchmakingStatusChange;
        }
        else if (utils::str_icmp_internal(current, "membersList") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::MemberListChange;
        }
        else if (utils::str_icmp_internal(current, "membersStatus") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::MemberStatusChange;
        }
        else if (utils::str_icmp_internal(current, "XblMultiplayerJoinability") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::SessionJoinabilityChange;
        }
        else if (utils::str_icmp_internal(current, "customProperty") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::CustomPropertyChange;
        }
        else if (utils::str_icmp_internal(current, "membersCustomProperty") == 0)
        {
            resultingChangeTypes |= XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange;
        }
    }
    return static_cast<XblMultiplayerSessionChangeTypes>(resultingChangeTypes);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END