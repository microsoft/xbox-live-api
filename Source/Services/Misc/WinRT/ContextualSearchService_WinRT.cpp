//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "ContextualSearchConfiguredStat_WinRT.h"
#include "ContextualSearchBroadcast_WinRT.h"
#include "ContextualSearchService_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

ContextualSearchService::ContextualSearchService(
    _In_ xbox::services::contextual_search::contextual_search_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchConfiguredStat^>^>^ 
ContextualSearchService::GetConfigurationAsync(
    _In_ uint32 titleId
    )
{
    auto task = m_cppObj.get_configuration(
        titleId
        )
    .then([](xbox_live_result<std::vector<contextual_search_configured_stat>> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchConfiguredStat, contextual_search_configured_stat>(cppResult.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ 
ContextualSearchService::GetBroadcastsAsync(
    _In_ uint32 titleId
    )
{
    auto task = m_cppObj.get_broadcasts(
        titleId
        )
    .then([](xbox_live_result<std::vector<contextual_search_broadcast>> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchBroadcast, contextual_search_broadcast>(cppResult.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ 
ContextualSearchService::GetBroadcastsAsync(
    _In_ uint32 titleId,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems,
    _In_ Platform::String^ orderByStatName,
    _In_ bool orderAscending,
    _In_ Platform::String^ searchQuery
    )
{
    auto task = m_cppObj.get_broadcasts(
        titleId,
        skipItems,
        maxItems,
        STRING_T_FROM_PLATFORM_STRING(orderByStatName),
        orderAscending,
        STRING_T_FROM_PLATFORM_STRING(searchQuery)
        )
    .then([](xbox_live_result<std::vector<contextual_search_broadcast>> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchBroadcast, contextual_search_broadcast>(cppResult.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ 
ContextualSearchService::GetBroadcastsAsync(
    _In_ uint32 titleId,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems,
    _In_ Platform::String^ orderByStatName,
    _In_ bool orderAscending,
    _In_ Platform::String^ filterStatName,
    _In_ ContextualSearchFilterOperator filterOperator,
    _In_ Platform::String^ filterStatValue
    )
{
    auto task = m_cppObj.get_broadcasts(
        titleId,
        skipItems,
        maxItems,
        STRING_T_FROM_PLATFORM_STRING(orderByStatName),
        orderAscending,
        STRING_T_FROM_PLATFORM_STRING(filterStatName),
        static_cast<contextual_search_filter_operator>(filterOperator),
        STRING_T_FROM_PLATFORM_STRING(filterStatValue)
        )
    .then([](xbox_live_result<std::vector<contextual_search_broadcast>> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchBroadcast, contextual_search_broadcast>(cppResult.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<ContextualSearchGameClipsResult^>^
ContextualSearchService::GetGameClipsAsync(
    _In_ uint32 titleId,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems,
    _In_ Platform::String^ orderByStatName,
    _In_ bool orderAscending,
    _In_ Platform::String^ filterStatName,
    _In_ ContextualSearchFilterOperator filterOperator,
    _In_ Platform::String^ filterStatValue
    )
{
    auto task = m_cppObj.get_game_clips(
        titleId,
        skipItems,
        maxItems,
        STRING_T_FROM_PLATFORM_STRING(orderByStatName),
        orderAscending,
        STRING_T_FROM_PLATFORM_STRING(filterStatName),
        static_cast<contextual_search_filter_operator>(filterOperator),
        STRING_T_FROM_PLATFORM_STRING(filterStatValue)
        )
    .then([](xbox_live_result<contextual_search_game_clips_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ContextualSearchGameClipsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<ContextualSearchGameClipsResult^>^
ContextualSearchService::GetGameClipsAsync(
    _In_ uint32 titleId,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems,
    _In_ Platform::String^ orderByStatName,
    _In_ bool orderAscending,
    _In_ Platform::String^ searchQuery
    )
{
    auto task = m_cppObj.get_game_clips(
        titleId,
        skipItems,
        maxItems,
        STRING_T_FROM_PLATFORM_STRING(orderByStatName),
        orderAscending,
        STRING_T_FROM_PLATFORM_STRING(searchQuery)
        )
    .then([](xbox_live_result<contextual_search_game_clips_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ContextualSearchGameClipsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END


