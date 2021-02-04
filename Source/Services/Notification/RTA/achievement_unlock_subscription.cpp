// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievement_unlock_subscription.h"


#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

AchievementUnlockSubscription::AchievementUnlockSubscription(uint64_t xuid, uint32_t titleId) noexcept
{
    Stringstream ss;
    ss << "https://notify.xboxlive.com/users/xuid(" << xuid << ")/deviceId/current/titleId/" << titleId;
    m_resourceUri = ss.str();
}

const String &
AchievementUnlockSubscription::ResourceUri() const noexcept
{
    return m_resourceUri;
}

XblFunctionContext 
AchievementUnlockSubscription::AddHandler(EventHandler handler) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_handlers[m_nextId] = std::move(handler);
    return m_nextId++;
}

size_t 
AchievementUnlockSubscription::RemoveHandler(XblFunctionContext id) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_handlers.erase(id);
    return m_handlers.size();
}


void 
AchievementUnlockSubscription::OnEvent(_In_ const JsonValue& data) noexcept
{
    String originatingService;
    JsonUtils::ExtractJsonString(data, "service", originatingService);
    if (!data.IsObject() || originatingService != "achievements")
    {
        return;
    }

    // Deserialize and send invite notification
    auto deserializationResult = AchievementUnlockEvent::Deserialize(data);
    if (Succeeded(deserializationResult))
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        auto handlers{ m_handlers };
        lock.unlock();

        for (auto& handler : handlers)
        {
            handler.second(deserializationResult.Payload());
        }
    }
}

void
AchievementUnlockSubscription::OnResync() noexcept 
{
    LOGS_ERROR << __FUNCTION__ << ": Achievement Unlock event may have been discarded by RTA service";
}

AchievementUnlockEvent::AchievementUnlockEvent( AchievementUnlockEvent&& event ) :
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

AchievementUnlockEvent::AchievementUnlockEvent( const AchievementUnlockEvent& event ) :
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


Result<AchievementUnlockEvent> AchievementUnlockEvent::Deserialize( _In_ const JsonValue& json ) noexcept
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

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementDescription", result.m_achievementDescription, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementName", result.m_achievementName, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementIcon", result.m_achievementIconUri, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToUInt64( json, "titleId", titleId, true));
        
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64( json, "gamerscore", result.gamerscore, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "xuid", result.xboxUserId, true));
        
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "extendedInfoUrl", result.m_deepLink, true));

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonDouble( json, "rarityPercentage", rarityPercentage, true));
        
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString( json, "rarityCategory", rarityCategory, true));
    
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT( json, "unlockTime", result.timeUnlocked));

    result.titleId = static_cast<uint32_t>(titleId);
    result.rarityPercentage = static_cast<float>(rarityPercentage);
    result.rarityCategory = EnumValue<XblAchievementRarityCategory>(rarityCategory.c_str());

    // strings for the C API
    result.achievementId = result.m_achievementId.c_str();
    result.achievementName = result.m_achievementName.c_str();
    result.achievementDescription = result.m_achievementDescription.c_str();
    result.achievementIcon = result.m_achievementIconUri.c_str();
    result.m_deepLink = result.m_deepLink.c_str();

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

#endif