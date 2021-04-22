// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_title_association::achievement_title_association(
    const XblAchievementTitleAssociation& association
) :
    m_titleId{ association.titleId }
{
    m_name = Utils::StringTFromUtf8(association.name);
}

const string_t& achievement_title_association::name() const
{
    return m_name;
}

uint32_t achievement_title_association::title_id() const
{
    return m_titleId;
}

achievement_requirement::achievement_requirement(
    const XblAchievementRequirement& requirement
)
{
    m_id = Utils::StringTFromUtf8(requirement.id);
    m_currentProgressValue = Utils::StringTFromUtf8(requirement.currentProgressValue);
    m_targetProgressValue = Utils::StringTFromUtf8(requirement.targetProgressValue);
}

const string_t& achievement_requirement::id() const
{
    return m_id;
}

const string_t& achievement_requirement::current_progress_value() const
{
    return m_currentProgressValue;
}

const string_t& achievement_requirement::target_progress_value() const
{
    return m_targetProgressValue;
}

achievement_progression::achievement_progression(
    const XblAchievementProgression& progression
)
{
    for (size_t i = 0; i < progression.requirementsCount; ++i)
    {
        m_requirements.push_back(achievement_requirement{ progression.requirements[i] });
    }
    m_timeUnlocked = Utils::DatetimeFromTimeT(progression.timeUnlocked);
}

const std::vector<achievement_requirement>& achievement_progression::requirements() const
{
    return m_requirements;
}

const utility::datetime& achievement_progression::time_unlocked() const
{
    return m_timeUnlocked;
}

achievement_time_window::achievement_time_window(
    const XblAchievementTimeWindow& timeWindow
) :
    m_startDate{ Utils::DatetimeFromTimeT(timeWindow.startDate) },
    m_endDate{ Utils::DatetimeFromTimeT(timeWindow.endDate) }
{
}

const utility::datetime& achievement_time_window::start_date() const
{
    return m_startDate;
}

const utility::datetime& achievement_time_window::end_date() const
{
    return m_endDate;
}

achievement_media_asset::achievement_media_asset(
    const XblAchievementMediaAsset* mediaAsset
)
{
    if (mediaAsset)
    {
        m_type = static_cast<achievement_media_asset_type>(mediaAsset->mediaAssetType);
        if (mediaAsset->name)
        {
            m_name = Utils::StringTFromUtf8(mediaAsset->name);
        }
        if (mediaAsset->url)
        {
            m_url = Utils::StringTFromUtf8(mediaAsset->url);
        }
    }
}

const string_t& achievement_media_asset::name() const
{
    return m_name;
}

achievement_media_asset_type achievement_media_asset::media_asset_type() const
{
    return m_type;
}

const web::uri& achievement_media_asset::url() const
{
    return m_url;
}

achievement_reward::achievement_reward(
    const XblAchievementReward& reward
) :
    m_name{ Utils::StringTFromUtf8(reward.name) },
    m_description{ Utils::StringTFromUtf8(reward.description) },
    m_value{ Utils::StringTFromUtf8(reward.value) },
    m_rewardType{ static_cast<achievement_reward_type>(reward.rewardType) },
    m_valueType{ Utils::StringTFromUtf8(reward.valueType) },
    m_mediaAsset{ reward.mediaAsset }
{
}

const string_t& achievement_reward::name() const
{
    return m_name;
}

const string_t& achievement_reward::description() const
{
    return m_description;
}

const string_t& achievement_reward::value() const
{
    return m_value;
}

achievement_reward_type achievement_reward::reward_type() const
{
    return m_rewardType;
}

const string_t& achievement_reward::value_type() const
{
    return m_valueType;
}

const achievement_media_asset& achievement_reward::media_asset() const
{
    return m_mediaAsset;
}

achievement::achievement(
    XblAchievementsResultHandle handle, 
    const XblAchievement* achievement
) :
    m_achievement{ achievement }
{
    XblAchievementsResultDuplicateHandle(handle, &m_handle);
}

achievement::achievement(const achievement& other)
    : m_achievement{ other.m_achievement }
{
    XblAchievementsResultDuplicateHandle(other.m_handle, &m_handle);
}

achievement& achievement::operator=(achievement other)
{
    std::swap(m_handle, other.m_handle);
    m_achievement = other.m_achievement;
    return *this;
}

achievement::~achievement()
{
    XblAchievementsResultCloseHandle(m_handle);
}

string_t achievement::id() const
{
    return Utils::StringTFromUtf8(m_achievement->id);
}

string_t achievement::service_configuration_id() const
{
    return Utils::StringTFromUtf8(m_achievement->serviceConfigurationId);
}

string_t achievement::name() const
{
    return Utils::StringTFromUtf8(m_achievement->name);
}

std::vector<achievement_title_association> achievement::title_associations() const
{
    std::vector<achievement_title_association> titleAssociations;
    for (size_t i = 0; i < m_achievement->titleAssociationsCount; ++i)
    {
        titleAssociations.push_back(achievement_title_association{ m_achievement->titleAssociations[i] });
    }
    return titleAssociations;
}

achievement_progress_state achievement::progress_state() const
{
    return static_cast<achievement_progress_state>(m_achievement->progressState);
}

achievement_progression achievement::progression() const
{
    return achievement_progression{ m_achievement->progression };
}

std::vector<achievement_media_asset> achievement::media_assets() const
{
    std::vector<achievement_media_asset> mediaAssets;
    for (size_t i = 0; i < m_achievement->mediaAssetsCount; ++i)
    {
        mediaAssets.push_back(achievement_media_asset{ &m_achievement->mediaAssets[i] });
    }
    return mediaAssets;
}

std::vector<string_t> achievement::platforms_available_on() const
{
    return Utils::StringTVectorFromCStringArray(m_achievement->platformsAvailableOn, m_achievement->platformsAvailableOnCount);
}

bool achievement::is_secret() const
{
    return m_achievement->isSecret;
}

string_t achievement::unlocked_description() const
{
    return Utils::StringTFromUtf8(m_achievement->unlockedDescription);
}

string_t achievement::locked_description() const
{
    return Utils::StringTFromUtf8(m_achievement->lockedDescription);
}

string_t achievement::product_id() const
{
    return Utils::StringTFromUtf8(m_achievement->productId);
}

achievement_type achievement::type() const
{
    return static_cast<achievement_type>(m_achievement->type);
}

achievement_participation_type achievement::participation_type() const
{
    return static_cast<achievement_participation_type>(m_achievement->participationType);
}

achievement_time_window achievement::available() const
{
    return achievement_time_window{ m_achievement->available };
}

std::vector<achievement_reward> achievement::rewards() const
{
    std::vector<achievement_reward> rewards;
    for (size_t i = 0; i < m_achievement->rewardsCount; ++i)
    {
        rewards.push_back(achievement_reward{ m_achievement->rewards[i] });
    }
    return rewards;
}

std::chrono::seconds achievement::estimated_unlock_time() const
{
    return std::chrono::seconds{ m_achievement->estimatedUnlockTime };
}

string_t achievement::deep_link() const
{
    return Utils::StringTFromUtf8(m_achievement->deepLink);
}

bool achievement::is_revoked() const
{
    return m_achievement->isRevoked;
}

achievements_result::achievements_result(XblAchievementsResultHandle handle)
{
    if (handle != nullptr)
    {
        XblAchievementsResultDuplicateHandle(handle, &m_handle);
    }
}

achievements_result::achievements_result(const achievements_result& other)
{
    if (other.m_handle != nullptr)
    {
        XblAchievementsResultDuplicateHandle(other.m_handle, &m_handle);
    }
}

achievements_result& achievements_result::operator=(achievements_result other)
{
    std::swap(m_handle, other.m_handle);
    return *this;
}

achievements_result::~achievements_result()
{
    XblAchievementsResultCloseHandle(m_handle);
}

std::vector<achievement> achievements_result::items() const
{
    std::vector<achievement> achievementsVector;

    const XblAchievement* achievements;
    size_t achievementsCount;
    HRESULT hr = XblAchievementsResultGetAchievements(m_handle, &achievements, &achievementsCount);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < achievementsCount; ++i)
        {
            achievementsVector.push_back(achievement{ m_handle, achievements + i });
        }
    }
    return achievementsVector;
}

bool achievements_result::has_next() const
{
    bool hasNext{ false };
    XblAchievementsResultHasNext(m_handle, &hasNext);
    return hasNext;
}

pplx::task<xbox::services::xbox_live_result<achievements_result>> achievements_result::get_next(
    _In_ uint32_t maxItems
)
{
    auto asyncWrapper = new AsyncWrapper<achievements_result>(
        [](XAsyncBlock* async, achievements_result& result)
    {
        XblAchievementsResultHandle resultHandle;
        auto hr = XblAchievementsResultGetNextResult(async, &resultHandle);
        if (SUCCEEDED(hr))
        {
            result = achievements_result{ resultHandle };
            XblAchievementsResultCloseHandle(resultHandle);
        }
        return hr;
    });

    auto hr = XblAchievementsResultGetNextAsync(m_handle, maxItems, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}


achievement_service::achievement_service(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

achievement_service::achievement_service(const achievement_service& other)
{
    XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
}

achievement_service& achievement_service::operator=(achievement_service other)
{
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    return *this;
}

achievement_service::~achievement_service()
{
    XblContextCloseHandle(m_xblContextHandle);
}

pplx::task<xbox::services::xbox_live_result<void>> achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
)
{
    auto xblContext = m_xblContextHandle;

    auto asyncWrapper = new AsyncWrapper<void>();

    HRESULT hr = XblAchievementsUpdateAchievementAsync(
        xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        Utils::StringFromStringT(achievementId).c_str(),
        percentComplete,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);

}

pplx::task<xbox::services::xbox_live_result<void>> achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
)
{
    auto xblContext = m_xblContextHandle;

    auto asyncWrapper = new AsyncWrapper<void>();

    HRESULT hr = XblAchievementsUpdateAchievementForTitleIdAsync(
        xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        titleId,
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(achievementId).c_str(),
        percentComplete,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);

}

pplx::task<xbox::services::xbox_live_result<achievements_result>> achievement_service::get_achievements_for_title_id(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
)
{
    auto xblContext = m_xblContextHandle;

    auto asyncWrapper = new AsyncWrapper<achievements_result>(
        [](XAsyncBlock* async, achievements_result& result)
        {
            XblAchievementsResultHandle resultHandle;
            HRESULT hr = XblAchievementsGetAchievementsForTitleIdResult(async, &resultHandle);
            if (SUCCEEDED(hr))
            {
                result = achievements_result(resultHandle);
            }
            return hr;
        });

    HRESULT hr = XblAchievementsGetAchievementsForTitleIdAsync(
        xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        titleId,
        static_cast<XblAchievementType>(type),
        unlockedOnly,
        static_cast<XblAchievementOrderBy>(orderBy),
        skipItems,
        maxItems,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox::services::xbox_live_result<achievement>> achievement_service::get_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId
)
{
    auto xblContext = m_xblContextHandle;

    auto asyncWrapper = new AsyncWrapper<achievement>(
        [](XAsyncBlock* async, achievement result)
        {
            XblAchievementsResultHandle resultHandle;
            HRESULT hr = XblAchievementsGetAchievementResult(async, &resultHandle);
            if (SUCCEEDED(hr))
            {
                const XblAchievement* achievements = nullptr;
                size_t achievementsCount = 0;
                hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);
                if (SUCCEEDED(hr))
                {
                    result = achievement(resultHandle, achievements);
                }
            }
            return hr;
        });

    HRESULT hr = XblAchievementsGetAchievementAsync(
        xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(achievementId).c_str(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END