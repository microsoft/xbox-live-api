// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "peoplehub_service.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

namespace
{

// Optional PeopleHub bool field. A missing or null value yields 'defaultValue' rather than failing,
// since PeopleHub may null out decorations it could not resolve for a profile.
HRESULT ExtractOptionalBool(
    _In_ const JsonValue& json,
    _In_z_ const char* name,
    _Out_ bool& outBool,
    _In_ bool defaultValue = false
) noexcept
{
    outBool = defaultValue;
    if (!json.HasMember(name) || json[name].IsNull())
    {
        return S_OK;
    }
    if (!json[name].IsBool())
    {
        return WEB_E_INVALID_JSON_STRING;
    }
    outBool = json[name].GetBool();
    return S_OK;
}

// Optional PeopleHub string field copied into a fixed-size buffer. A missing or null value yields an
// empty string. A value too long for the buffer is truncated on a UTF-8 character boundary when
// 'truncate' is set (free-form display text). Otherwise (identifiers such as gamertags and URLs, where a
// truncated value would be a different, wrong value) it fails and the buffer is left empty.
HRESULT ExtractOptionalString(
    _In_ const JsonValue& json,
    _In_z_ const char* name,
    _Out_writes_z_(size) char* buffer,
    _In_ size_t size,
    _In_ bool truncate
) noexcept
{
    buffer[0] = '\0';
    if (!json.HasMember(name) || json[name].IsNull())
    {
        return S_OK;
    }

    const JsonValue& field{ json[name] };
    if (!field.IsString())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    const char* value{ field.GetString() };
    size_t length{ field.GetStringLength() };
    if (length >= size)
    {
        if (!truncate)
        {
            return E_INVALIDARG;
        }

        length = size - 1;
        // Back up past UTF-8 continuation bytes so a multi-byte character is never split.
        while (length > 0 && (static_cast<unsigned char>(value[length]) & 0xC0) == 0x80)
        {
            --length;
        }
        LOGS_WARN << "PeopleHub field '" << name << "' exceeds " << size - 1 << " bytes; truncating";
    }

    memcpy(buffer, value, length);
    buffer[length] = '\0';
    return S_OK;
}

} // anonymous namespace

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
    // A people/friends response always resolves the relationship (see DeserializeUser), so only the users are needed.
    return MakeServiceCall(xuid, decorations, RelationshipType::Social, Vector<uint64_t>{}, { async.Queue(),
        [async](Result<PeoplehubBatchResult> result)
    {
        if (Failed(result))
        {
            return async.Complete(result.Hresult());
        }
        return async.Complete(std::move(result.ExtractPayload().users));
    } });
}

HRESULT PeoplehubService::GetSocialUsers(
    _In_ uint64_t xuid,
    _In_ XblSocialManagerExtraDetailLevel decorations,
    _In_ const Vector<uint64_t>& xuids,
    _In_ AsyncContext<Result<PeoplehubBatchResult>> async
) const noexcept
{
    return MakeServiceCall(xuid, decorations, RelationshipType::Batch, xuids, async);
}

uint32_t PeoplehubService::HttpTimeoutWindowInSeconds() const noexcept
{
    return m_httpSettings->HttpTimeoutWindow();
}
    
HRESULT PeoplehubService::MakeServiceCall(
    _In_ uint64_t xuid,
    _In_ XblSocialManagerExtraDetailLevel decorations,
    _In_ RelationshipType relationshipType,
    _In_opt_ const Vector<uint64_t>& batchUsers,
    _In_ AsyncContext<Result<PeoplehubBatchResult>> async
) const noexcept
{
    Stringstream subpath;
    JsonDocument bodyJson;

    subpath << "/users/xuid(" << xuid << ")/people/";

    switch (relationshipType)
    {
    case RelationshipType::Social:
    {
        subpath << "friends";
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

    httpCall->SetXblServiceContractVersion(7);

    if (!bodyJson.IsNull())
    {
        httpCall->SetRequestBody(bodyJson);
    }

    return httpCall->Perform({
        async.Queue(),
        [
            async,
            relationshipType
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

        PeoplehubBatchResult batchResult;
        auto& users{ batchResult.users };

        JsonDocument responseBodyJson = httpCall->GetResponseBodyJson();
        auto peopleJsonArray = JsonUtils::ExtractJsonArray(
            responseBodyJson,
            "people",
            false
        );

        users.reserve(peopleJsonArray.Size());
        for (const auto& userJson : peopleJsonArray)
        {
            users.emplace_back();
            bool relationshipResolved{ false };
            HRESULT deserializeHr{ DeserializeUser(userJson, relationshipType, users.back(), relationshipResolved) };
            if (FAILED(deserializeHr))
            {
                // Skip only the malformed entry. Failing the whole response would discard every other
                // valid profile and leave the caller retrying a response that can never succeed.
                LOGS_ERROR << "Skipping PeopleHub profile that failed to deserialize, hr=" << deserializeHr;
                users.pop_back();
            }
            else if (!relationshipResolved)
            {
                batchResult.unresolvedRelationshipXuids.insert(users.back().xboxUserId);
            }
        }

        if (users.size() != peopleJsonArray.Size())
        {
            LOGS_ERROR << "PeopleHub returned " << peopleJsonArray.Size() << " profiles; " << users.size() << " deserialized";
        }

        return async.Complete(std::move(batchResult));
    } });
}

HRESULT PeoplehubService::DeserializeUser(
    const JsonValue& json,
    _In_ RelationshipType relationshipType,
    _Out_ XblSocialManagerUser& user,
    _Out_ bool& relationshipResolved
)
{
    memset(&user, 0, sizeof(user));
    relationshipResolved = false;

    if (!json.IsObject())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    // A null, empty, or non-numeric xuid parses as 0. Such a profile can't be identified, so it must fail
    // and be skipped rather than tracked as a phantom user 0.
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", user.xboxUserId, true));
    RETURN_HR_IF(user.xboxUserId == 0, WEB_E_INVALID_JSON_STRING);

    // Everything below the xuid is best effort: a field the service returns as null, too long, or of an
    // unexpected type is logged and left at its default instead of discarding the profile.
    auto logFieldFailure = [&user](const char* field, HRESULT hr)
    {
        if (FAILED(hr))
        {
            LOGS_WARN << "PeopleHub profile " << user.xboxUserId << ": ignoring invalid '" << field << "', hr=" << hr;
        }
    };

    // Every profile in a people/friends response is a friend, so there a missing or null isFriend means
    // true. Defaulting it to false would leave the user out of the caller's Friends groups.
    // A batch response can contain friends and non-friends, so there a missing, null, or invalid isFriend leaves
    // the relationship unresolved; the caller keeps any relationship it already knows instead of using the default.
    const bool isFriendDefault{ relationshipType == RelationshipType::Social };
    HRESULT isFriendHr{ ExtractOptionalBool(json, "isFriend", user.isFriend, isFriendDefault) };
    logFieldFailure("isFriend", isFriendHr);
    relationshipResolved = relationshipType == RelationshipType::Social ||
        (SUCCEEDED(isFriendHr) && json.HasMember("isFriend") && json["isFriend"].IsBool());
    logFieldFailure("isFavorite", ExtractOptionalBool(json, "isFavorite", user.isFavorite));
    logFieldFailure("displayName", ExtractOptionalString(json, "displayName", user.displayName, sizeof(user.displayName), true));
    logFieldFailure("realName", ExtractOptionalString(json, "realName", user.realName, sizeof(user.realName), true));
    logFieldFailure("displayPicRaw", ExtractOptionalString(json, "displayPicRaw", user.displayPicUrlRaw, sizeof(user.displayPicUrlRaw), false));
    logFieldFailure("useAvatar", ExtractOptionalBool(json, "useAvatar", user.useAvatar));
    logFieldFailure("gamertag", ExtractOptionalString(json, "gamertag", user.gamertag, sizeof(user.gamertag), false));
    logFieldFailure("modernGamertag", ExtractOptionalString(json, "modernGamertag", user.modernGamertag, sizeof(user.modernGamertag), false));
    logFieldFailure("modernGamertagSuffix", ExtractOptionalString(json, "modernGamertagSuffix", user.modernGamertagSuffix, sizeof(user.modernGamertagSuffix), false));
    logFieldFailure("uniqueModernGamertag", ExtractOptionalString(json, "uniqueModernGamertag", user.uniqueModernGamertag, sizeof(user.uniqueModernGamertag), false));
    logFieldFailure("gamerScore", ExtractOptionalString(json, "gamerScore", user.gamerscore, sizeof(user.gamerscore), false));

    // isFavorite should reflect both isFavorite && isFriend from the service response
    user.isFavorite = user.isFavorite && user.isFriend;
    // Shim isFollowingUser and isFollowedByCaller to be true if isFriend is true
    // This is purely for compatibility purposes and doesn't reflect a follower/following relationship
    user.isFollowedByCaller = user.isFriend;
    user.isFollowingUser = user.isFriend;

    HRESULT hr{ DeserializePresenceRecord(json, user.presenceRecord) };
    if (FAILED(hr))
    {
        logFieldFailure("presence", hr);
        memset(&user.presenceRecord, 0, sizeof(user.presenceRecord));
    }

    if (json.HasMember("titleHistory"))
    {
        auto titleHistoryResult = DeserializeTitleHistory(json["titleHistory"]);
        logFieldFailure("titleHistory", titleHistoryResult.Hresult());
        if (Succeeded(titleHistoryResult))
        {
            user.titleHistory = titleHistoryResult.ExtractPayload();
        }
    }

    if (json.HasMember("preferredColor"))
    {
        auto preferredColorResult = DeserializePreferredColor(json["preferredColor"]);
        logFieldFailure("preferredColor", preferredColorResult.Hresult());
        if (Succeeded(preferredColorResult))
        {
            user.preferredColor = preferredColorResult.ExtractPayload();
        }
    }

    return S_OK;
}

HRESULT PeoplehubService::DeserializePresenceRecord(
    const JsonValue& json,
    _Out_ XblSocialManagerPresenceRecord& record
)
{
    memset(&record, 0, sizeof(record));

    if (!json.IsObject())
    {
        return S_OK;
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
        if (record.presenceTitleRecordCount >= XBL_NUM_PRESENCE_RECORDS)
        {
            break;
        }

        auto& slot{ record.presenceTitleRecords[record.presenceTitleRecordCount] };
        if (SUCCEEDED(DeserializePresenceTitleRecord(titleRecordJson, slot)))
        {
            ++record.presenceTitleRecordCount;
        }
        else
        {
            // Deserializing in place means a partial write can survive a failure. Unused slots must
            // stay zeroed so callers never see residual data past presenceTitleRecordCount.
            memset(&slot, 0, sizeof(slot));
        }
    }

    return S_OK;
}

HRESULT PeoplehubService::DeserializePresenceTitleRecord(
    const JsonValue& json,
    _Out_ XblSocialManagerPresenceTitleRecord& record
)
{
    memset(&record, 0, sizeof(record));

    if (!json.IsObject())
    {
        return S_OK;
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

    //get titleName from Presence string: format should be "Title - Rich Presence Text".
    //The title and rich presence text are separated by " - " (space-hyphen-space). A bare
    //hyphen can legitimately appear inside a title (e.g. "Gears of War: E-Day"), so match the
    //full delimiter rather than the first '-'. If no delimiter is present, the whole presence
    //string is the title name.
    const char* delimiter = strstr(record.presenceText, " - ");
    size_t titleNameLength = (delimiter != nullptr)
        ? static_cast<size_t>(delimiter - record.presenceText)
        : strlen(record.presenceText);
    if (titleNameLength >= sizeof(record.titleName))
    {
        titleNameLength = sizeof(record.titleName) - 1;
    }
    memcpy(record.titleName, record.presenceText, titleNameLength);
    record.titleName[titleNameLength] = '\0';

    return S_OK;
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