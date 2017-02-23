// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameServerPlatformService_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::game_server_platform;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameServerPlatformService::GameServerPlatformService(
    _In_ game_server_platform_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<ClusterResult^>^ 
GameServerPlatformService::AllocateClusterAsync(
    _In_ uint32 gameServerTitleId,
    _In_ String^ serviceConfigurationId,
    _In_ String^ sessionTemplateName,
    _In_ String^ sessionName,
    _In_ bool abortIfQueued
    )
{
    auto task = m_cppObj.allocate_cluster(
        gameServerTitleId,
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(sessionTemplateName),
        STRING_T_FROM_PLATFORM_STRING(sessionName),
        abortIfQueued)
    .then([](xbox_live_result<cluster_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ClusterResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<ClusterResult^>^ 
GameServerPlatformService::AllocateClusterInlineAsync(
    _In_ uint32 gameServerTitleId,
    _In_ String^ serviceConfigurationId,
    _In_ String^ sandboxId,
    _In_ String^ ticketId,
    _In_ String^ gsiSetId,
    _In_ String^ gameVariantId,
    _In_ uint64 maxAllowedPlayers,
    _In_ String^ location,
    _In_ bool abortIfQueued
    )
{
    auto task = m_cppObj.allocate_cluster_inline(
        gameServerTitleId,
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(sandboxId),
        STRING_T_FROM_PLATFORM_STRING(ticketId),
        STRING_T_FROM_PLATFORM_STRING(gsiSetId),
        STRING_T_FROM_PLATFORM_STRING(gameVariantId),
        maxAllowedPlayers,
        STRING_T_FROM_PLATFORM_STRING(location),
        abortIfQueued)
    .then([](xbox_live_result<cluster_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ClusterResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<GameServerTicketStatus^>^ 
GameServerPlatformService::GetTicketStatusAsync(
    _In_ uint32 gameServerTitleId,
    _In_ String^ ticketId
    )
{ 
    auto task = m_cppObj.get_ticket_status(
        gameServerTitleId,
        STRING_T_FROM_PLATFORM_STRING(ticketId)
        )
    .then([](xbox_live_result<game_server_ticket_status> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new GameServerTicketStatus(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<GameServerMetadataResult^>^ 
GameServerPlatformService::GetGameServerMetadataAsync(
    _In_ uint32 titleId,
    _In_ uint32 maxAllowedPlayers,
    _In_ bool publisherOnly,
    _In_ uint32 maxVariants,
    _In_opt_ Platform::String^ locale
    )
{ 
    return GetGameServerMetadataAsync(titleId, maxAllowedPlayers, publisherOnly, maxVariants, locale, nullptr);
}

IAsyncOperation<GameServerMetadataResult^>^ 
GameServerPlatformService::GetGameServerMetadataAsync(
    _In_ uint32 titleId,
    _In_ uint32 maxAllowedPlayers,
    _In_ bool publisherOnly,
    _In_ uint32 maxVariants,
    _In_opt_ Platform::String^ locale,
    _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ filterTags
    )
{ 
    std::map<std::wstring, std::wstring> filterMap = UtilsWinRT::CreateStdMapStringFromMapObj(filterTags);
    auto task = m_cppObj.get_game_server_metadata(
        titleId,
        maxAllowedPlayers,
        publisherOnly,
        maxVariants,
        filterMap
        )
    .then([](xbox_live_result<game_server_metadata_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new GameServerMetadataResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<QualityOfServiceServer^>^>^ 
GameServerPlatformService::GetQualityOfServiceServersAsync()
{
    auto task = m_cppObj.get_quality_of_service_servers()
    .then([](xbox_live_result<std::vector<quality_of_service_server>> servers)
    {
        THROW_IF_ERR(servers);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<QualityOfServiceServer, quality_of_service_server>(servers.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<AllocationResult^>^ 
GameServerPlatformService::AllocateSessionHost(
    _In_ uint32 gameServerTitleId,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ locations,
    _In_ Platform::String^ sessionId,
    _In_ Platform::String^ cloudGameId,
    _In_ Platform::String^ gameModeId,
    _In_ Platform::String^ sessionCookie
    )
{
    auto task = m_cppObj.allocate_session_host(
        gameServerTitleId,
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(locations),
        STRING_T_FROM_PLATFORM_STRING(sessionId),
        STRING_T_FROM_PLATFORM_STRING(cloudGameId),
        STRING_T_FROM_PLATFORM_STRING(gameModeId),
        STRING_T_FROM_PLATFORM_STRING(sessionCookie)
        )
    .then([](xbox_live_result<allocation_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new AllocationResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}


Windows::Foundation::IAsyncOperation<AllocationResult^>^ 
GameServerPlatformService::GetSessionHostAllocationStatus(
    _In_ uint32 gameServerTitleId,
    _In_ Platform::String^ sessionId
    )
{
    auto task = m_cppObj.get_session_host_allocation_status(
        gameServerTitleId,
        STRING_T_FROM_PLATFORM_STRING(sessionId)                
        )
    .then([](xbox_live_result<allocation_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new AllocationResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
