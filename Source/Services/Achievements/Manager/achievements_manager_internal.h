// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/achievements_manager_c.h"
#include "achievements_internal.h"

namespace xbox {
namespace services {
namespace achievements {
namespace manager {

class AchievementsManager;

}
}
}
}

struct XblAchievementsManagerResult : public xbox::services::RefCounter, public std::enable_shared_from_this<XblAchievementsManagerResult>
{
public: 
    XblAchievementsManagerResult(_In_ const XblAchievement& achievement);
    XblAchievementsManagerResult(_In_ Vector<XblAchievement>& achievements, _In_ bool explicitCleanup = false);
    XblAchievementsManagerResult(_In_ XblAchievement* achievements, _In_ size_t achievementCount, _In_ bool explicitCleanup = false);
    virtual ~XblAchievementsManagerResult();

    const Vector<XblAchievement>& Achievements() const;
    
protected:
    // RefCounter
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;

private:
    XblAchievementsManagerResult(const XblAchievementsManagerResult& other) = delete;
    XblAchievementsManagerResult& operator=(XblAchievementsManagerResult other) = delete;

    Vector<XblAchievement> m_achievements;
    XblAchievement* m_achievementsData;
    size_t m_achievementsCount;
    bool m_explicitCleanup;
};

/// <summary>
/// Enumeration values that indicate the lock state of an achievement.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
enum class AchievementsManagerFilterType : uint32_t
{
    /// <summary>
    /// Specifies both locked and unlocked achievements to be included.
    /// </summary>
    All = 0,

    /// <summary>
    /// Specifies that unlocked achievements should be included.
    /// </summary>
    Unlocked = 1,

    /// <summary>
    ///  Specifies that achievements with no progress should be included.
    /// </summary>
    NotStarted = 2, 

    /// <summary>
    ///  Specifies that achievements currently in progress should be included.
    /// </summary>
    InProgress = 3,
};

/// <summary>
/// Collection of options used to customise the subset of achievements 
/// to return back to the caller.
/// </summary>
struct AchievementsManagerSortFilterSettings
{
    /// <summary>
    /// The field to sort the list of achievements on. TitleId will behave
    /// the same as DefaultOrder, as AchievementsManager only handles one title
    /// at a time.
    /// </summary>
    XblAchievementOrderBy sortBy;

    /// <summary>
    /// The direction by which to sort the list of achievements.
    /// </summary>
    XblAchievementsManagerSortOrder sortOrder;

    /// <summary>
    /// The achievement state to include in the list of achievements.
    /// </summary>
    AchievementsManagerFilterType stateFilter;
} ;


NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_BEGIN

class AchievementsManagerUser : public std::enable_shared_from_this<AchievementsManagerUser>
{
public:
    AchievementsManagerUser(
        _In_ User&& localUser,
        _In_ const TaskQueue& queue
    ) noexcept;
    virtual ~AchievementsManagerUser();

    Result<void> Initialize(
        _In_ AsyncContext<HRESULT> async
    );
    
    bool IsInitialized() const;
    uint64_t Xuid() const;
    uint64_t GetAchievementCount() const;

    Vector<XblAchievementsManagerEvent> ProcessEvents();

    Result<XblAchievement> GetAchievement(
        _In_ const String& id
    );
    
    XblAchievement* GetAchievements();
    Vector<XblAchievement> GetAchievements(
        _In_ AchievementsManagerSortFilterSettings sortFilterSettings
    );
    
    Result<void> CanUpdateAchievement(
        _In_ const String& achievementId, 
        _In_ uint8_t percent
    );
    
    Result<void> UpdateAchievement(
        _In_ const String& achievementId, 
        _In_ uint8_t percent
    );

private:

    HRESULT FetchAchievements(
        _In_ AsyncContext<HRESULT> async
    );
    
    HRESULT HandleAchievementsResults(
        _In_ Result<std::shared_ptr<XblAchievementsResult>> result,
        _In_ uint32_t achievementsPerFetch,
        _In_ AsyncContext<HRESULT> async,
        _In_ Vector<XblAchievement> fetchedAchievements = Vector<XblAchievement>()
    );

    std::mutex m_mutex;
    bool m_isInitialized = false;

    Map<String, XblAchievement*> m_userAchievements;
    XblAchievement* m_achievementCache{ nullptr };
    uint64_t m_xuid{ 0 };
   
    Vector<XblAchievementsManagerEvent> m_eventsToProcess;
    Vector<XblAchievementsManagerEvent> m_generatedEvents;

    std::shared_ptr<XblContext> m_xblContext;
    
    XblFunctionContext m_achievementProgressToken{ 0 };
    XblFunctionContext m_rtaResyncToken{ 0 };
    XblFunctionContext m_rtaConnectionToken{ 0 };
    
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;
    
    TaskQueue m_queue;
};

class AchievementsManager : public std::enable_shared_from_this<AchievementsManager>
{
public:
    AchievementsManager() = default;

    // In this case, The object returned from this function is mostly a shallow 
    //  copy of the argument, as most values in an XblAchievement don't change
    //  during gameplay. The ones that do will be deep copied. Should only
    //  be called 
    static XblAchievement CopyAchievementForResult(const XblAchievement & other);
    static HRESULT CleanUpAchievementCopyForResult(XblAchievement & achievement);

    // Deep copy makes dynamic allocations so that the object can be completely 
    //  detatched from the original object. Requires parts of the object to be 
    //  manually deallocated.
    static XblAchievement DeepCopyAchievement(_In_ const XblAchievement& other);
    static Vector<XblAchievement> DeepCopyAchievements(const Vector<XblAchievement>& achievements);
    static XblAchievement* DeepCopyAchievements(XblAchievement* cache, size_t size);

    // Helper function that aids in freeing the dynamically allocated memory used 
    //  when creating a deep copy of an achievement. 
    static HRESULT CleanUpDeepCopyAchievement(_In_ XblAchievement& achievement);
    
    HRESULT AddLocalUser(
        _In_ User&& user,
        _In_ TaskQueue&& queue
    ) noexcept;

    HRESULT RemoveLocalUser(
        _In_ const User& user
    ) noexcept;

    const Vector<XblAchievementsManagerEvent>& DoWork() XBL_NOEXCEPT;

    Result<XblAchievement> GetAchievement(
        _In_ uint64_t xuid, 
        _In_ const String achievementId
    );

    Result<std::pair<XblAchievement*, size_t>> GetAchievements(
        _In_ uint64_t xuid
    );

    Result<Vector<XblAchievement>> GetAchievements(
        _In_ uint64_t xuid,
        _In_ AchievementsManagerSortFilterSettings requestConfig
    );

    Result<void> UpdateAchievement(
        _In_ uint64_t xuid,
        _In_ const String achievementId,
        _In_ uint8_t progress
    );
    
    bool HasUser(
        _In_ uint64_t xuid
    ) const;
    
    bool IsUserInitialized(
        _In_ uint64_t xuid
    );
    
    uint64_t GetUserAchievementCount(
        _In_ uint64_t xuid
    );

private:
    
    std::mutex m_mutex;
    Vector<XblAchievementsManagerEvent> m_publishedEvents;
    Vector<XblAchievementsManagerEvent> m_pendingEvents;
    Map<uint64_t, std::shared_ptr<AchievementsManagerUser>> m_localUsers;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_END