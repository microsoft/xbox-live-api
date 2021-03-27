// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "notification_internal.h"
#include "real_time_activity_manager.h"
#include "multiplayer_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

GameInviteSubscription::GameInviteSubscription(
    _In_ uint64_t xuid,
    _In_ uint32_t titleId
) noexcept
{
    Stringstream ss;
    ss << "https://notify.xboxlive.com/users/xuid(" << xuid << ")/deviceId/current/titleId/" << titleId;
    m_resourceUri = ss.str();
}

const String& GameInviteSubscription::ResourceUri() const noexcept
{
    return m_resourceUri;
}

XblFunctionContext GameInviteSubscription::AddHandler(
    MPSDInviteHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_mpsdInviteHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

XblFunctionContext GameInviteSubscription::AddHandler(
    MultiplayerActivityInviteHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_mpaInviteHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

size_t GameInviteSubscription::RemoveHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_mpaInviteHandlers.erase(token);
    m_mpsdInviteHandlers.erase(token);
    return m_mpaInviteHandlers.size() + m_mpsdInviteHandlers.size();
}

void GameInviteSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    // MPSD Invites and MPA invites are both delivered via the same RTA subscription, but the payloads for each
    // are slightly different. We examine the json here to distinguish between the two.

    if (!data.IsObject())
    {
        LOGS_ERROR << __FUNCTION__ << ": Ignoring unrecognized payload";
    }
    else if (data.HasMember("inviteHandle"))
    {
        String originatingService;
        JsonUtils::ExtractJsonString(data, "service", originatingService);
        if (originatingService != "achievements")
        {
            // Deserialize and send invite notification
            auto deserializationResult = GameInviteNotificationEventArgs::Deserialize(data["inviteHandle"]);
            if (Succeeded(deserializationResult))
            {
                std::unique_lock<std::mutex> lock{ m_mutex };
                auto handlers{ m_mpsdInviteHandlers };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(deserializationResult.Payload());
                }
            }
        }
    }
    else if (data.HasMember("notificationData"))
    {
        // Deserialize and send invite notification
        auto deserializationResult = MultiplayerActivityInviteData::Deserialize(data["notificationData"]);
        if (Succeeded(deserializationResult))
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            auto handlers{ m_mpaInviteHandlers };
            lock.unlock();

            for (auto& handler : handlers)
            {
                handler.second(deserializationResult.Payload());
            }
        }
    }
    else
    {
        LOGS_ERROR << __FUNCTION__ << ": Ignoring unrecognized payload";
    }
}

GameInviteNotificationEventArgs::GameInviteNotificationEventArgs(
    const GameInviteNotificationEventArgs& other
) noexcept
    : XblGameInviteNotificationEventArgs{ other },
    m_inviteHandleId{ other.m_inviteHandleId },
    m_inviteProtocol{ other.m_inviteProtocol },
    m_senderImageUrl{ other.m_senderImageUrl }
{
    if (!m_inviteHandleId.empty())
    {
        inviteHandleId = m_inviteHandleId.data();
    }
    if (!m_inviteProtocol.empty())
    {
        inviteProtocol = m_inviteProtocol.data();
    }
    if (!m_senderImageUrl.empty())
    {
        senderImageUrl = m_senderImageUrl.data();
    }
}

Result<GameInviteNotificationEventArgs> GameInviteNotificationEventArgs::Deserialize(
    _In_ const JsonValue& inviteHandle
) noexcept
{
    if (!inviteHandle.IsObject())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    GameInviteNotificationEventArgs result{};
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(inviteHandle, "invitedXuid", result.invitedXboxUserId, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(inviteHandle, "senderXuid", result.senderXboxUserId, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteHandle, "id", result.m_inviteHandleId, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteHandle, "inviteProtocol", result.m_inviteProtocol, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(inviteHandle, "expiration", result.expiration, true));
    result.sessionReference = xbox::services::multiplayer::Serializers::DeserializeSessionReference(inviteHandle["sessionRef"]).Payload();

    if (inviteHandle.IsObject() && inviteHandle.HasMember("inviteInfo"))
    {
        const JsonValue& inviteInfoValue = inviteHandle["inviteInfo"];
        String sender, senderMGT, senderMGTSuffix, senderUniqueMGT;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteInfoValue, "sender", sender, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteInfoValue, "senderModernGamertag", senderMGT, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteInfoValue, "senderModernGamertagSuffix", senderMGTSuffix, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteInfoValue, "senderUniqueModernGamertag", senderUniqueMGT, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteInfoValue, "senderImageUrl", result.m_senderImageUrl, true));
        utils::strcpy(result.senderGamertag, sizeof(result.senderGamertag), sender.c_str());
        utils::strcpy(result.senderModernGamertag, sizeof(result.senderModernGamertag), senderMGT.c_str());
        utils::strcpy(result.senderModernGamertagSuffix, sizeof(result.senderModernGamertagSuffix), senderMGTSuffix.c_str());
        utils::strcpy(result.senderUniqueModernGamertag, sizeof(result.senderUniqueModernGamertag), senderUniqueMGT.c_str());
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }
    return result;
}

MultiplayerActivityInviteData::MultiplayerActivityInviteData(
    const MultiplayerActivityInviteData& other
) noexcept
    : XblMultiplayerActivityInviteData{ other },
    m_senderImageUrl{ other.m_senderImageUrl },
    m_titleName{ other.m_titleName },
    m_titleImageUrl{ other.m_titleImageUrl },
    m_connectionString{ other.m_connectionString }
{
    if (!m_titleName.empty())
    {
        titleName = m_titleName.data();
    }
    if (!m_connectionString.empty())
    {
        connectionString = m_connectionString.data();
    }
    if (!m_titleImageUrl.empty())
    {
        titleImageUrl = m_titleImageUrl.data();
    }
    if (!m_senderImageUrl.empty())
    {
        senderImageUrl = m_senderImageUrl.data();
    }
}

Result<MultiplayerActivityInviteData> MultiplayerActivityInviteData::Deserialize(
    const JsonValue& notificationData
) noexcept
{
    if (!notificationData.IsObject())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    MultiplayerActivityInviteData data{};
    if (notificationData.HasMember("sender"))
    {
        auto& senderJson{ notificationData["sender"] };
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(senderJson, "xuid", data.senderXuid, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(senderJson, "imageUrl", data.m_senderImageUrl));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(senderJson, "gamertag", data.senderGamertag, sizeof(data.senderGamertag)));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(senderJson, "modernGamertag", data.senderModernGamertag, sizeof(data.senderModernGamertag)));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(senderJson, "modernGamertagSuffix", data.senderModernGamertagSuffix, sizeof(data.senderModernGamertagSuffix)));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(senderJson, "uniqueModernGamertag", data.senderUniqueModernGamertag, sizeof(data.senderUniqueModernGamertag)));
    }
    else
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(notificationData, "invitedUser", data.invitedXuid, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(notificationData, "titleName", data.m_titleName, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(notificationData, "titleImageUrl", data.m_titleImageUrl));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(notificationData, "connectionString", data.m_connectionString));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(notificationData, "expirationTime", data.expirationTime));

    return data;
}

void GameInviteSubscription::OnResync() noexcept
{
    // Don't think there is much we can do here - just log an error 
    LOGS_ERROR << __FUNCTION__ << ": Game invites may have been discarded by RTA service";
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif