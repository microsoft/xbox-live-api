// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievements_manager_internal.h"
#include <utility>

using namespace xbox::services;
using namespace xbox::services::achievements::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_BEGIN

template<typename Ret, typename TWork>
Ret ApiImpl(Ret&& fallbackReturnValue, TWork&& work) noexcept
{
    auto state{ GlobalState::Get() };
    if (!state)
    {
        return fallbackReturnValue;
    }

    assert(state->AchievementsManager());
    return work(*state->AchievementsManager());
}

template<typename TWork>
HRESULT ApiImpl(TWork&& work) noexcept
{
    return ApiImpl<HRESULT, TWork>(E_XBL_NOT_INITIALIZED, std::move(work));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_END

STDAPI XblAchievementsManagerResultGetAchievements(
    _In_ XblAchievementsManagerResultHandle resultHandle,
    _Out_ const XblAchievement** achievements,
    _Out_ uint64_t* achievementsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(achievementsCount == nullptr || achievements == nullptr || resultHandle == nullptr);
    VERIFY_XBL_INITIALIZED();

    *achievementsCount = resultHandle->Achievements().size();
    *achievements = *achievementsCount > 0 ? resultHandle->Achievements().data() : nullptr;
    
    return S_OK;
}
CATCH_RETURN()

STDAPI XblAchievementsManagerResultDuplicateHandle(
    _In_ XblAchievementsManagerResultHandle handle,
    _Out_ XblAchievementsManagerResultHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);

    handle->AddRef();
    *duplicatedHandle = handle;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblAchievementsManagerResultCloseHandle(
    _In_ XblAchievementsManagerResultHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }

}
CATCH_RETURN_WITH(;)

STDAPI XblAchievementsManagerAddLocalUser(
    _In_ XblUserHandle user,
    _In_opt_ XTaskQueueHandle queue
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

            return achievementsManager.AddLocalUser(wrapUserResult.ExtractPayload(), TaskQueue::DeriveWorkerQueue(queue));
        });
}
CATCH_RETURN()

STDAPI XblAchievementsManagerRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(user);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

            return achievementsManager.RemoveLocalUser(wrapUserResult.Payload());
        });
}
CATCH_RETURN()

STDAPI XblAchievementsManagerIsUserInitialized(
    _In_ uint64_t xboxUserId
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(!achievementsManager.HasUser(xboxUserId));
            if (!achievementsManager.IsUserInitialized(xboxUserId))
            {
                return E_FAIL;
            }
            return S_OK;
        });
}
CATCH_RETURN()


STDAPI XblAchievementsManagerDoWork(
    _Outptr_result_maybenull_ const XblAchievementsManagerEvent** achievementsEvents,
    _Out_ size_t* achievementsEventsCount
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(achievementsEvents);

    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(achievementsEvents == nullptr || achievementsEventsCount == nullptr);
            auto& events = achievementsManager.DoWork();

            *achievementsEvents = nullptr;
            if (!events.empty())
            {
                *achievementsEvents = &(*events.begin());
            }
            *achievementsEventsCount = events.size();
            return S_OK;
        }
    );
}
CATCH_RETURN()

STDAPI XblAchievementsManagerGetAchievement(
    _In_ uint64_t xboxUserId,
    _In_ const char* achievementId,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementResult
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(achievementId == nullptr || achievementResult == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(achievementId);
    *achievementResult = nullptr;

    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            auto achievement = achievementsManager.GetAchievement(xboxUserId, achievementId);
            if (Failed(achievement))
            {
                LOGS_ERROR << achievement.ErrorMessage();
                return achievement.Hresult();
            }

            std::shared_ptr<XblAchievementsManagerResult> resultHandle = MakeShared<XblAchievementsManagerResult>(achievement.ExtractPayload());//resultData);
            *achievementResult = resultHandle.get(); 
            resultHandle->AddRef();
            return S_OK;    
        }
    );
}
CATCH_RETURN()

STDAPI XblAchievementsManagerGetAchievements(
    _In_ uint64_t xboxUserId,
    _In_ XblAchievementOrderBy sortField,
    _In_ XblAchievementsManagerSortOrder sortOrder,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementsResult
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(achievementsResult);
    
    // Can't specify a sort order when just using default field to order on
    RETURN_HR_INVALIDARGUMENT_IF(sortField != XblAchievementOrderBy::UnlockTime && sortOrder != XblAchievementsManagerSortOrder::Unsorted);
    
    // Can't specify a sort field without specifying the order to sort on.
    RETURN_HR_INVALIDARGUMENT_IF(sortField == XblAchievementOrderBy::UnlockTime && sortOrder == XblAchievementsManagerSortOrder::Unsorted);
    
    bool areSortOptionsDefault = sortField != XblAchievementOrderBy::UnlockTime && sortOrder == XblAchievementsManagerSortOrder::Unsorted;
    if (!areSortOptionsDefault)
    {
        return XblAchievementsManagerGetAchievementsByState(
            xboxUserId,
            sortField,
            sortOrder,
            XblAchievementProgressState::Unknown,
            achievementsResult
        );
    }

    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(achievementsResult == nullptr);
            *achievementsResult = nullptr;

            auto achievements = achievementsManager.GetAchievements(xboxUserId);
            if (Failed(achievements))
            {
                LOGS_ERROR << achievements.ErrorMessage();
                return achievements.Hresult();
            }
            
            if (achievements.Payload().second == 0)
            {
                return E_UNEXPECTED;
            }
            
            std::shared_ptr<XblAchievementsManagerResult> resultHandle = MakeShared<XblAchievementsManagerResult>(achievements.Payload().first, achievements.Payload().second, true);
            *achievementsResult = resultHandle.get();
            resultHandle->AddRef();
            
            return S_OK;
        }
    );
}
CATCH_RETURN()

STDAPI XblAchievementsManagerGetAchievementsByState(
    _In_ uint64_t xboxUserId,
    _In_ XblAchievementOrderBy sortField,
    _In_ XblAchievementsManagerSortOrder sortOrder,
    _In_ XblAchievementProgressState achievementState,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementsResult
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(achievementsResult);

    // Can't specify a sort order when just using default field to order on
    RETURN_HR_INVALIDARGUMENT_IF(sortField != XblAchievementOrderBy::UnlockTime && sortOrder != XblAchievementsManagerSortOrder::Unsorted);

    // Can't specify a sort field without specifying the order to sort on.
    RETURN_HR_INVALIDARGUMENT_IF(sortField == XblAchievementOrderBy::UnlockTime && sortOrder == XblAchievementsManagerSortOrder::Unsorted);

    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(achievementsResult == nullptr);

            // Return an invalid arg if one of sortField or sortOrder are set to a 
            //  non-default value while the other is set to a default value.
            RETURN_HR_INVALIDARGUMENT_IF((sortField == XblAchievementOrderBy::UnlockTime && sortOrder == XblAchievementsManagerSortOrder::Unsorted)
                || (sortOrder != XblAchievementsManagerSortOrder::Unsorted && sortField == XblAchievementOrderBy::DefaultOrder));
            
            *achievementsResult = nullptr;

            AchievementsManagerSortFilterSettings sortOptions{ sortField, sortOrder, AchievementsManagerFilterType::All };
            switch (achievementState)
            {
            case XblAchievementProgressState::Achieved:
                sortOptions.stateFilter = AchievementsManagerFilterType::Unlocked;
                break;
            case XblAchievementProgressState::InProgress:
                sortOptions.stateFilter = AchievementsManagerFilterType::InProgress;
                break;
            case XblAchievementProgressState::NotStarted:
                sortOptions.stateFilter = AchievementsManagerFilterType::NotStarted;
                break;
            default:
                sortOptions.stateFilter = AchievementsManagerFilterType::All;
                break;
            }

            auto achievements = achievementsManager.GetAchievements(xboxUserId, sortOptions);
            if (Failed(achievements))
            {
                LOGS_ERROR << achievements.ErrorMessage();
                return achievements.Hresult();
            }

            // don't need to return an error for getting an empty vector, as it means there
            // were no elements that matched made it through the filter.

            std::shared_ptr<XblAchievementsManagerResult> resultHandle = MakeShared<XblAchievementsManagerResult>(achievements.Payload());
            *achievementsResult = resultHandle.get();
            resultHandle->AddRef();
            return S_OK;
        }
    );
}
CATCH_RETURN()

STDAPI XblAchievementsManagerUpdateAchievement(
    _In_ uint64_t xboxUserId,
    _In_ const char* achievementId,
    _In_ uint8_t currentProgress
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](AchievementsManager& achievementsManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(achievementId);
            RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(achievementId);

            currentProgress = static_cast<uint8_t>(fmin(currentProgress, 100));

            auto updateResult = achievementsManager.UpdateAchievement(xboxUserId, achievementId, currentProgress);
            if (Failed(updateResult))
            {
                LOGS_ERROR << updateResult.ErrorMessage();
                return updateResult.Hresult();
            }
            return S_OK;
        }
    );
}
CATCH_RETURN()
