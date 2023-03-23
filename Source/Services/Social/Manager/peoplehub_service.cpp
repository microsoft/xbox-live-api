// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "peoplehub_service.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

PeoplehubService::PeoplehubService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> httpCallSettings,
    _In_ uint32_t titleId
) noexcept :
    m_user{ std::move(user) },
    m_httpSettings{ std::move(httpCallSettings) },
    m_titleId{ titleId }
{
}

HRESULT PeoplehubService::GetSocialGraph(
    _In_ uint64_t xuid,
    _In_ XblSocialManagerExtraDetailLevel decorations,
    _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
) const noexcept
{
    return MakeServiceCall(xuid, decorations, RelationshipType::Social, Vector<uint64_t>{}, async);
}

HRESULT PeoplehubService::GetSocialUsers(
    _In_ uint64_t xuid,
    _In_ XblSocialManagerExtraDetailLevel decorations,
    _In_ const Vector<uint64_t>& xuids,
    _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
) const noexcept
{
    return MakeServiceCall(xuid, decorations, RelationshipType::Batch, xuids, async);
}

HRESULT PeoplehubService::MakeServiceCall(
    _In_ uint64_t xuid,
    _In_ XblSocialManagerExtraDetailLevel decorations,
    _In_ RelationshipType relationshipType,
    _In_opt_ const Vector<uint64_t>& batchUsers,
    _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
) const noexcept
{
    Stringstream subpath;
    JsonDocument bodyJson;

    subpath << "/users/xuid(" << xuid << ")/people/";

    switch (relationshipType)
    {
    case RelationshipType::Social:
    {
        subpath << "social";
        break;
    }
    case RelationshipType::Batch:
    {
        subpath << "batch";

        JsonValue xuidJson = JsonValue(rapidjson::kArrayType);
        for (size_t i = 0; i < batchUsers.size(); ++i)
        {
            xuidJson.PushBack(JsonValue(utils::uint64_to_internal_string(batchUsers[i]).c_str(), bodyJson.GetAllocator()).Move(), bodyJson.GetAllocator());
        }

        bodyJson.SetObject();
        bodyJson.AddMember("xuids", xuidJson, bodyJson.GetAllocator());
        break;
    }
    }

    // Always include the presenceDetail decoration
    subpath << "/decoration/presenceDetail";

    if (decorations != XblSocialManagerExtraDetailLevel::NoExtraDetail)
    {
        if ((decorations & XblSocialManagerExtraDetailLevel::TitleHistoryLevel) == XblSocialManagerExtraDetailLevel::TitleHistoryLevel)
        {
            subpath << ",titlehistory(" << m_titleId << ")";
        }
        if ((decorations & XblSocialManagerExtraDetailLevel::PreferredColorLevel) == XblSocialManagerExtraDetailLevel::PreferredColorLevel)
        {
            subpath << ",preferredcolor";
        }
    }

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_httpSettings,
        relationshipType == RelationshipType::Batch ? "POST" : "GET",
        XblHttpCall::BuildUrl("peoplehub", subpath.str()),
        xbox_live_api::get_social_graph
    ));

    httpCall->SetXblServiceContractVersion(5);

    if (!bodyJson.IsNull())
    {
        httpCall->SetRequestBody(bodyJson);
    }

    return httpCall->Perform({
        async.Queue(),
        [
            async
        ]
    (HttpResult httpResult)
    {
        HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        auto httpCall{ httpResult.ExtractPayload() };

        auto header = httpCall->GetResponseHeader("x-xbl-servicedefault");
        if (!header.empty())
        {
            LOGS_ERROR << "Peoplehub dependency failed to load: " << header;
            return async.Complete({ utils::convert_xbox_live_error_code_to_hresult(xbl_error_code::http_status_424_failed_dependency) });
        }

        Vector<XblSocialManagerUser> users;

        JsonDocument responseBodyJson = httpCall->GetResponseBodyJson();
        auto peopleJsonArray = JsonUtils::ExtractJsonArray(
            responseBodyJson,
            "people",
            false
        );

        for (auto& user : peopleJsonArray)
        {
            auto result{ DeserializeUser(user) };
            if (Succeeded(result))
            {
                users.push_back(result.ExtractPayload());
            }
            else
            {
                return async.Complete({ result.Hresult() });
            }
        }

        return async.Complete(users);
    } });
}

Result<XblSocialManagerUser> PeoplehubService::DeserializeUser(
    const JsonValue& json
)
{
    XblSocialManagerUser user{};

    if (!json.IsObject())
    {
        return user;
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", user.xboxUserId, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isFavorite", user.isFavorite));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isFollowedByCaller", user.isFollowedByCaller));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isFollowingCaller", user.isFollowingUser));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "displayName", user.displayName, sizeof(user.displayName)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "realName", user.realName, sizeof(user.realName)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "displayPicRaw", user.displayPicUrlRaw, sizeof(user.displayPicUrlRaw)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "useAvatar", user.useAvatar));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "gamertag", user.gamertag, sizeof(user.gamertag)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "modernGamertag", user.modernGamertag, sizeof(user.modernGamertag)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "modernGamertagSuffix", user.modernGamertagSuffix, sizeof(user.modernGamertagSuffix)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "uniqueModernGamertag", user.uniqueModernGamertag, sizeof(user.uniqueModernGamertag)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "gamerScore", user.gamerscore, sizeof(user.gamerscore)));

    auto presenceRecordResult = DeserializePresenceRecord(json);
    if (Succeeded(presenceRecordResult))
    {
        user.presenceRecord = presenceRecordResult.ExtractPayload();
    }
    else
    {
        return { presenceRecordResult.Hresult() };
    }

    if (json.IsObject() && json.HasMember("titleHistory"))
    {
        auto titleHistoryResult = DeserializeTitleHistory(json["titleHistory"]);
        if (Succeeded(titleHistoryResult))
        {
            user.titleHistory = titleHistoryResult.ExtractPayload();
        }
        else
        {
            return { titleHistoryResult.Hresult() };
        }
    }

    if (json.IsObject() && json.HasMember("preferredColor"))
    {
        auto preferredColorResult = DeserializePreferredColor(json["preferredColor"]);
        if (Succeeded(preferredColorResult))
        {
            user.preferredColor = preferredColorResult.ExtractPayload();
        }
        else
        {
            return { preferredColorResult.Hresult() };
        }
    }

    return Result<XblSocialManagerUser>{ user };
}

Result<XblSocialManagerPresenceRecord> PeoplehubService::DeserializePresenceRecord(
    const JsonValue& json
)
{
    XblSocialManagerPresenceRecord record{};

    if (!json.IsObject())
    {
        return record;
    }

    String presenceState;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "presenceState", presenceState));
    record.userState = XblPresenceRecord::UserStateFromString(presenceState);

    auto presenceDetailsJson = JsonUtils::ExtractJsonArray(
        json,
        "presenceDetails",
        false
    );

    for (const auto& titleRecordJson : presenceDetailsJson)
    {
        auto titleRecordResult = DeserializePresenceTitleRecord(titleRecordJson);
        if (Succeeded(titleRecordResult))
        {
            record.presenceTitleRecords[record.presenceTitleRecordCount++] = titleRecordResult.ExtractPayload();
        }

        if (record.presenceTitleRecordCount >= XBL_NUM_PRESENCE_RECORDS)
        {
            break;
        }
    }

    return Result<XblSocialManagerPresenceRecord>{ record };
}

Result<XblSocialManagerPresenceTitleRecord> PeoplehubService::DeserializePresenceTitleRecord(
    const JsonValue& json
)
{
    XblSocialManagerPresenceTitleRecord record{};

    if (!json.IsObject())
    {
        return { record };
    }

    String device, state, titleId;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "Device", device));
    record.deviceType = presence::DeviceRecord::DeviceTypeFromString(device);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "PresenceText", record.presenceText, sizeof(record.presenceText)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "State", state));
    record.isTitleActive = utils::str_icmp_internal(state, "active") == 0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "TitleId", titleId));
    record.titleId = utils::internal_string_to_uint32(titleId);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "IsPrimary", record.isPrimary));

    //get titleName from Presence string: format should be "Title - Rich Presence Text"
    for (int i = 0; i < XBL_TITLE_NAME_CHAR_SIZE; i++)
    {
        char c = record.presenceText[i];
        if (c == '-' || c == '\0')
        {
            record.titleName[i] = '\0';
            break;
        }
        record.titleName[i] = c;
    }

    return Result<XblSocialManagerPresenceTitleRecord>{ record };
}

Result<XblPreferredColor> PeoplehubService::DeserializePreferredColor(
    const JsonValue& json
)
{
    XblPreferredColor color{};

    if (!json.IsObject())
    {
        return { color };
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "primaryColor", color.primaryColor, sizeof(color.primaryColor)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "secondaryColor", color.secondaryColor, sizeof(color.secondaryColor)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "tertiaryColor", color.tertiaryColor, sizeof(color.tertiaryColor)));

    return Result<XblPreferredColor>{ color };
}

Result<XblTitleHistory> PeoplehubService::DeserializeTitleHistory(
    const JsonValue& json
)
{
    XblTitleHistory titleHistory{};

    if (!json.IsObject())
    {
        return titleHistory;
    }

    // If PeopleHub service fails to query TitleHistory, the "lastTimePlayed" field may be null.
    // We don't want to fail deserialization in this case, so just return that the user has not played
    constexpr const char* lastTimePlayedKey{ "lastTimePlayed" };
    if (json.HasMember(lastTimePlayedKey) && json[lastTimePlayedKey].IsString())
    {
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(
            json,
            lastTimePlayedKey,
            titleHistory.lastTimeUserPlayed,
            true
        ));
    }

    constexpr const char* lastTimePlayedTextKey{ "lastTimePlayedText" };
    if (json.HasMember(lastTimePlayedTextKey) && json[lastTimePlayedTextKey].IsString())
    {
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(
            json,
            lastTimePlayedTextKey,
            titleHistory.lastTimeUserPlayedText,
            XBL_LAST_TIME_PLAYED_CHAR_SIZE
        ));
    }

    titleHistory.hasUserPlayed = titleHistory.lastTimeUserPlayed != 0;

    return Result<XblTitleHistory>{ titleHistory };
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END