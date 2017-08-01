// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubsService_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::clubs;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

ClubsService::ClubsService(
    _In_ xbox::services::clubs::clubs_service cppObj) :
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<Club^>^ ClubsService::GetClubAsync(
    _In_ String^ clubId
    )
{
    auto task = m_cppObj.get_club(STRING_T_FROM_PLATFORM_STRING(clubId))
    .then([](xbox_live_result<club> result)
    {
        THROW_IF_ERR(result);
        return ref new Club(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<Club^>^>^ ClubsService::GetClubsAsync(
    _In_ IVectorView<String^>^ clubIds
    )
{
    auto task = m_cppObj.get_clubs(UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(clubIds))
    .then([](xbox_live_result<std::vector<club>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Club, club>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<ClubsOwnedResult^>^ ClubsService::GetClubsOwnedAsync()
{
    auto task = m_cppObj.get_clubs_owned()
    .then([](xbox_live_result<clubs_owned_result> result)
    {
        THROW_IF_ERR(result);
        return ref new ClubsOwnedResult(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<Club^>^ ClubsService::CreateClubAsync(
    _In_ String^ name,
    _In_ ClubType type,
    _In_ String^ titleFamilyId
    )
{
    auto task = m_cppObj.create_club(
        STRING_T_FROM_PLATFORM_STRING(name), 
        static_cast<club_type>(type), 
        STRING_T_FROM_PLATFORM_STRING(titleFamilyId)
        )
    .then([](xbox_live_result<club> result)
    {
        THROW_IF_ERR(result);
        return ref new Club(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncAction^ ClubsService::DeleteClubAsync(
    _In_ String^ clubId
    )
{
    auto task = m_cppObj.delete_club(STRING_T_FROM_PLATFORM_STRING(clubId))
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<Club^>^>^ ClubsService::GetClubAssociationsAsync()
{
    auto task = m_cppObj.get_club_associations()
    .then([](xbox_live_result<std::vector<club>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Club, club>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<Club^>^>^ ClubsService::GetClubAssociationsAsync(
    _In_ String^ xuid
    )
{
    auto task = m_cppObj.get_club_associations(STRING_T_FROM_PLATFORM_STRING(xuid))
    .then([](xbox_live_result<std::vector<club>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Club, club>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRecommendation^>^>^ ClubsService::GetClubRecommendationsAsync()
{
    auto task = m_cppObj.get_club_recommendations()
    .then([](xbox_live_result<std::vector<club_recommendation_t>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRecommendation, club_recommendation_t>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<ClubsSearchResult^>^ ClubsService::SearchClubsAsync(
    _In_ String^ queryString,
    _In_ IVectorView<String^>^ titleIds,
    _In_ IVectorView<String^>^ tags
    )
{
    auto task = m_cppObj.search_clubs(
        STRING_T_FROM_PLATFORM_STRING(queryString),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(tags),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(titleIds)
        )
    .then([](xbox_live_result<clubs_search_result> result)
    {
        THROW_IF_ERR(result);
        return ref new ClubsSearchResult(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubSearchAutoComplete^>^>^ ClubsService::SuggestClubsAsync(
    _In_ Platform::String^ queryString,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ titleIds,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ tags
    )
{
    auto task = m_cppObj.suggest_clubs(
        STRING_T_FROM_PLATFORM_STRING(queryString),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(tags),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(titleIds)
        )
    .then([](xbox_live_result<std::vector<club_search_auto_complete>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubSearchAutoComplete, club_search_auto_complete>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END