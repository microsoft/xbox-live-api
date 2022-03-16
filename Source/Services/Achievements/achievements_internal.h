// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/achievements_c.h"
#include "real_time_activity_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

typedef Callback<const XblAchievementProgressChangeEventArgs&> AchievementProgressChangeHandler;

class AchievementProgressChangeSubscription : public real_time_activity::Subscription
{
public:
    AchievementProgressChangeSubscription(
        _In_ uint64_t xuid, 
        _In_ const xbox::services::String& scid
    ) noexcept;

    XblFunctionContext AddHandler(AchievementProgressChangeHandler handler) noexcept;
    size_t RemoveHandler(XblFunctionContext token) noexcept;

    static XblAchievementProgressChangeEntry DeepCopyProgressChangeEntry (XblAchievementProgressChangeEntry& source);
    static void CleanUpProgressChangeEntry(XblAchievementProgressChangeEntry& entry);

protected:
    void OnEvent(const JsonValue& data) noexcept override;

private:
    Map<XblFunctionContext, AchievementProgressChangeHandler> m_handlers;
    XblFunctionContext m_nextHandlerToken{ 1 };
    mutable std::mutex m_lock;

    uint64_t m_userId;
};


class AchievementsService : public std::enable_shared_from_this<AchievementsService>
{
public:
    AchievementsService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<AppConfig> appConfig,
        _In_ std::weak_ptr<::XblContext> xboxLiveContextImpl,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    );

    HRESULT UpdateAchievement(
        _In_ uint64_t xboxUserId,
        _In_ const String& achievementId,
        _In_ uint32_t percentComplete,
        _In_ AsyncContext<Result<void>> async
    ) const noexcept;

    HRESULT UpdateAchievement(
        _In_ uint64_t xboxUserId,
        _In_ uint32_t titleId,
        _In_ const String& serviceConfigurationId,
        _In_ const String& achievementId,
        _In_ uint32_t percentComplete,
        _In_ AsyncContext<Result<void>> async
    ) const noexcept;
    
    HRESULT GetAchievementsForTitle(
        _In_ uint64_t xboxUserId,
        _In_ uint32_t titleId,
        _In_ XblAchievementType type,
        _In_ bool unlockedOnly,
        _In_ XblAchievementOrderBy orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> async
    ) const noexcept;

    HRESULT GetAchievement(
        _In_ uint64_t xboxUserId,
        _In_ const String& serviceConfigurationId,
        _In_ const String& achievementId,
        _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> async
    ) const noexcept;

    HRESULT GetAchievements(
        _In_ uint64_t xboxUserId,
        _In_ const Vector<uint32_t>& titleIds,
        _In_ XblAchievementType type,
        _In_ bool unlockedOnly,
        _In_ XblAchievementOrderBy orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const String& continuationToken,
        _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> callback
    ) const noexcept;

    XblFunctionContext AddAchievementProgressChangeHandler(
        _In_ AchievementProgressChangeHandler handler
    ) noexcept;

    void RemoveAchievementProgressChangeHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    // Since there is not a destructor for XblAchievement objects, use this helper to free any dynamically
    // allocated memory associated with an XblAchievement and zero it out.
    static void CleanupAchievement(XblAchievement& achievement);

    static Result<XblAchievement> DeserializeAchievement(const JsonValue& json);

private:
    // Achievements endpoints
    static String GetAchievementsSubpath(
        _In_ uint64_t xboxUserId,
        _In_ const Vector<uint32_t>& titleIds,
        _In_ XblAchievementType type,
        _In_ bool unlockedOnly,
        _In_ XblAchievementOrderBy orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const String& continuationToken
    );

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::AppConfig> m_appConfig;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;
    std::weak_ptr<XblContext> m_xboxLiveContextImpl;

    std::shared_ptr<AchievementProgressChangeSubscription> m_achievementProgressChangeSubscription;
    mutable std::mutex m_lock;

#if HC_PLATFORM == HC_PLATFORM_XDK
    static HRESULT WriteOfflineUpdateAchievement(
        _In_ std::shared_ptr<XblContext> xboxLiveContextImpl,
        _In_ const String& achievementId,
        _In_ uint32_t percentComplete
    );

    static ULONG EventWriteAchievementUpdate(
        _In_ PCWSTR userId,
        _In_ PCWSTR achievementId,
        _In_ const uint32_t percentComplete
    );

#else
    static HRESULT WriteOfflineUpdateAchievement(
        _In_ std::shared_ptr<XblContext> xboxLiveContextImpl,
        _In_ const String& achievementId,
        _In_ uint32_t percentComplete
    );
#endif

    // Deserialization helpers
    static Result<XblAchievementTitleAssociation> DeserializeTitleAssociation(const JsonValue& json);
    static Result<XblAchievementRequirement> DeserializeRequirement(const JsonValue& json);
    static Result<XblAchievementProgression> DeserializeProgression(const JsonValue& json);
    static Result<XblAchievementTimeWindow> DeserializeTimeWindow(const JsonValue& json);
    static Result<XblAchievementMediaAsset> DeserializeMediaAsset(const JsonValue& json);
    static Result<XblAchievementReward> DeserializeReward(const JsonValue& json);

    friend struct XblContext;
    friend class achievements_result_internal;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END

struct XblAchievementsResult : public xbox::services::RefCounter, public std::enable_shared_from_this<XblAchievementsResult>
{
public:
    XblAchievementsResult(_In_ std::shared_ptr<const xbox::services::achievements::AchievementsService> service);
    virtual ~XblAchievementsResult();

    static xbox::services::Result<std::shared_ptr<XblAchievementsResult>> Deserialize(
        _In_ const JsonDocument& json,
        _In_ std::shared_ptr<const xbox::services::achievements::AchievementsService> service);

    const Vector<XblAchievement>& Achievements() const;

    bool HasNext() const;

    HRESULT GetNext(
        _In_ uint32_t maxItems,
        _In_ AsyncContext<xbox::services::Result<std::shared_ptr<XblAchievementsResult>>> async
    ) const noexcept;

    void SetNextPageQueryParameters(
        _In_ uint64_t xuid,
        _In_ const xsapi_internal_vector<uint32_t>& titleIds,
        _In_ XblAchievementType type,
        _In_ bool unlockedOnly,
        _In_ XblAchievementOrderBy orderBy
    );

protected:
    // RefCounter
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;

private:
    XblAchievementsResult(const XblAchievementsResult& other) = delete;
    XblAchievementsResult& operator=(XblAchievementsResult other) = delete;

    xsapi_internal_vector<XblAchievement> m_achievements;

    // Info needed for GetNext query
    xsapi_internal_string m_continuationToken;
    uint64_t m_xuid{ 0 };
    xsapi_internal_vector<uint32_t> m_titleIds;
    XblAchievementType m_achievementType{ XblAchievementType::All };
    bool m_unlockedOnly{ false };
    XblAchievementOrderBy m_orderBy{ XblAchievementOrderBy::DefaultOrder };

    std::weak_ptr<const xbox::services::achievements::AchievementsService> m_serviceWeakPointer;
};


