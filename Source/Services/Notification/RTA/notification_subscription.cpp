#include "pch.h"
#include "notification_subscription.h"
#include "real_time_activity_manager.h"
#include "multiplayer_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

NotificationSubscription::NotificationSubscription(
    _In_ User&& user,
    _In_ TaskQueue queue,
    _In_ uint32_t titleId
) noexcept :
    m_user{ std::move(user) },
    m_queue{ std::move(queue) }
{
    Stringstream ss;
    ss << "https://notify.xboxlive.com/users/xuid(" << m_user.Xuid() << ")/deviceId/current/titleId/" << titleId;
    m_resourceUri = ss.str();
}

const String& NotificationSubscription::ResourceUri() const noexcept
{
    return m_resourceUri;
}

XblFunctionContext NotificationSubscription::AddHandler(MPSDInviteHandler&& handler) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_mpsdInviteHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

XblFunctionContext NotificationSubscription::AddHandler(MultiplayerActivityInviteHandler&& handler) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_mpaInviteHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

XblFunctionContext NotificationSubscription::AddHandler(AchievementUnlockHandler&& handler) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_achievementUnlockHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

size_t NotificationSubscription::RemoveHandler(XblFunctionContext token) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    
    m_mpaInviteHandlers.erase(token);
    m_mpsdInviteHandlers.erase(token);
    m_achievementUnlockHandlers.erase(token);

    // return the total number of handlers remaining
    return m_mpaInviteHandlers.size() + m_mpsdInviteHandlers.size() + m_achievementUnlockHandlers.size();
}

void NotificationSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    // Notifications from several services are delivered via the same RTA subscription. Examine the payload to 
    // distinguish between them.

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
    else if (data.HasMember("KickNotification"))
    {
        XalUserGetTokenAndSignatureArgs args{};
        args.forceRefresh = true;
        args.url = "https://xboxlive.com/";
        args.method = "GET";

        auto async = MakeUnique<XAsyncBlock>();
        async->queue = m_queue.GetHandle();
        async->callback = [](XAsyncBlock* async)
        {
            // Take ownership of async block
            UniquePtr<XAsyncBlock> asyncUnique{ async };

            // Get the result even though we aren't using it so Xal can release it
            size_t bufferSize{};
            HRESULT hr = XalUserGetTokenAndSignatureSilentlyResultSize(async, &bufferSize);

            if (SUCCEEDED(hr))
            {
                Vector<uint8_t> buffer(bufferSize);
                XalUserGetTokenAndSignatureData* xalTokenSignatureData{ nullptr };

                hr = XalUserGetTokenAndSignatureSilentlyResult(async, bufferSize, buffer.data(), &xalTokenSignatureData, nullptr);
                UNREFERENCED_PARAMETER(hr);
            }
        };

        if (SUCCEEDED(XalUserGetTokenAndSignatureSilentlyAsync(m_user.Handle(), &args, async.get())))
        {
            async.release();
        }
    }
    else if (data.HasMember("service"))
    {
        String originatingService;
        JsonUtils::ExtractJsonString(data, "service", originatingService);

        if (originatingService == "achievements")
        {
            // Deserialize and send invite notification
            auto deserializationResult = AchievementUnlockEvent::Deserialize(data);
            if (Succeeded(deserializationResult))
            {
                std::unique_lock<std::mutex> lock{ m_mutex };
                auto handlers{ m_achievementUnlockHandlers };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(deserializationResult.Payload());
                }
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
    m_inviteContext{ other.m_inviteContext },
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
    if (!m_inviteContext.empty())
    {
        inviteContext = m_inviteContext.data();
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
    if (inviteHandle.IsObject() && inviteHandle.HasMember("inviteAttributes"))
    {
        const JsonValue& inviteAttributesValue = inviteHandle["inviteAttributes"];

        if (inviteAttributesValue.HasMember("context"))
        {
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inviteAttributesValue, "context", result.m_inviteContext, true));
        }
    }

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

AchievementUnlockEvent::AchievementUnlockEvent(AchievementUnlockEvent&& event) noexcept :
    m_achievementId(std::move(event.m_achievementId)),
    m_achievementDescription(std::move(event.m_achievementDescription)),
    m_achievementName(std::move(event.m_achievementName)),
    m_achievementIconUri(std::move(event.m_achievementIconUri)),
    m_deepLink(event.m_deepLink)
{
    // strings for the C API
    achievementId = m_achievementId.c_str();
    achievementName = m_achievementName.c_str();
    achievementDescription = m_achievementDescription.c_str();
    achievementIcon = m_achievementIconUri.c_str();
    titleId = event.titleId;
    gamerscore = event.gamerscore;
    rarityPercentage = event.rarityPercentage;
    rarityCategory = event.rarityCategory;
    timeUnlocked = event.timeUnlocked;
    xboxUserId = event.xboxUserId;
}

AchievementUnlockEvent::AchievementUnlockEvent(const AchievementUnlockEvent& event) :
    m_achievementId(event.m_achievementId),
    m_achievementDescription(event.m_achievementDescription),
    m_achievementName(event.m_achievementName),
    m_achievementIconUri(event.m_achievementIconUri),
    m_deepLink(event.m_deepLink)
{
    // strings for the C API
    achievementId = m_achievementId.c_str();
    achievementName = m_achievementName.c_str();
    achievementDescription = m_achievementDescription.c_str();
    achievementIcon = m_achievementIconUri.c_str();

    titleId = event.titleId;
    gamerscore = event.gamerscore;
    rarityPercentage = event.rarityPercentage;
    rarityCategory = event.rarityCategory;
    timeUnlocked = event.timeUnlocked;
    xboxUserId = event.xboxUserId;
}


Result<AchievementUnlockEvent> AchievementUnlockEvent::Deserialize(_In_ const JsonValue& json) noexcept
{
    AchievementUnlockEvent result{};
    if (!json.IsObject())
    {
        return result;
    }

    double rarityPercentage = 0.0; // We are guaranteed that rarityPercentage is only float, but can only extract as double.
    uint64_t titleId = 0; // We are guaranteed that titleId is only uint32, but can only extract as uint64.
    String rarityCategory; // Will be converted into an enum value after extraction

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementId", result.m_achievementId, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementDescription", result.m_achievementDescription, false));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementName", result.m_achievementName, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementIcon", result.m_achievementIconUri, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToUInt64(json, "titleId", titleId, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(json, "gamerscore", result.gamerscore, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", result.xboxUserId, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "extendedInfoUrl", result.m_deepLink, true));

    // RarityPercentage and rarityCategory are only in payload version 2 so make them optional
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonDouble(json, "rarityPercentage", rarityPercentage, false));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "rarityCategory", rarityCategory, false));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "unlockTime", result.timeUnlocked));

    result.titleId = static_cast<uint32_t>(titleId);
    result.rarityPercentage = static_cast<float>(rarityPercentage);
    result.rarityCategory = EnumValue<XblAchievementRarityCategory>(rarityCategory.c_str());

    // strings for the C API
    result.achievementId = result.m_achievementId.c_str();
    result.achievementName = result.m_achievementName.c_str();
    if (!result.m_achievementDescription.empty())
    {
        result.achievementDescription = result.m_achievementDescription.c_str();
    }
    result.achievementIcon = result.m_achievementIconUri.c_str();
    result.m_deepLink = result.m_deepLink.c_str();

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

#endif