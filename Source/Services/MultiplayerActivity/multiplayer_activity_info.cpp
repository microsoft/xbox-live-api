// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_activity_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace multiplayer_activity
{

ActivityInfo::ActivityInfo(const XblMultiplayerActivityInfo* info) noexcept
    : XblMultiplayerActivityInfo{ *info },
    connectionString{ info->connectionString }
{
    if (info->groupId)
    {
        groupId = info->groupId;
    }
}

ActivityInfo::ActivityInfo(uint64_t xuid) noexcept
    : XblMultiplayerActivityInfo{ xuid }
{
}

Result<Vector<ActivityInfo>> ActivityInfo::Deserialize(
    const JsonValue& responseJson,
    uint32_t titleId
) noexcept
{
    if (!responseJson.HasMember("userActivities") || !responseJson["userActivities"].IsArray())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    Vector<ActivityInfo> result;

    const auto& users{ responseJson["userActivities"].GetArray() };
    for (const auto& user : users)
    {
        uint64_t xuid{};
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(user, "userId", xuid, true));

        // Users may have multiple activities, but we only care about their activities within our title
        auto activities = JsonUtils::ExtractJsonArray(user, "activities", true);
        for (const auto& activity : activities)
        {
            uint32_t activityTitleId{};
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(activity, "titleId", activityTitleId, true));

            if (activityTitleId == titleId)
            {
                ActivityInfo i{ xuid };
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(activity, "connectionString", i.connectionString));

                String joinRestriction;
                JsonUtils::ExtractJsonString(activity, "joinRestriction", joinRestriction, false);
                i.joinRestriction = EnumValue<XblMultiplayerActivityJoinRestriction>(joinRestriction.data());

                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonSizeT(activity, "maxPlayers", i.maxPlayers));
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonSizeT(activity, "currentPlayers", i.currentPlayers));
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(activity, "groupId", i.groupId));

                String platform;
                JsonUtils::ExtractJsonString(activity, "platform", platform, false);
                i.platform = EnumValue<XblMultiplayerActivityPlatform>(platform.data());

                result.push_back(i);
            }
        }
    }

    return result;
}

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
