// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "title_managed_statistics_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::user_statistics;

STDAPI XblTitleManagedStatsWriteAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_ const XblTitleManagedStatistic* statistics,
    _In_ size_t statisticsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    // It is only valid to write stats for the local user, but the service will return 200 if we try to write
    // stats for another user (it just won't actually update the SVD). Add a client side check so games get
    // an error in this case.
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle->Xuid() != xboxUserId);
    RETURN_HR_INVALIDARGUMENT_IF(statistics == nullptr || statisticsCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext = xblContextHandle->shared_from_this(),
            statList = Vector<TitleManagedStatistic>(statistics, statistics + statisticsCount)
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->TitleManagedStatisticsService()->WriteTitleManagedStatisticsAsync(
                statList,
                data->async
            ));

            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

HRESULT UpdateStatsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ Vector<TitleManagedStatistic>&& stats,
    _In_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext = xblContextHandle->shared_from_this(),
            stats{ std::move(stats) }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->TitleManagedStatisticsService()->UpdateTitleManagedStatistics(
                stats,
                data->async
            ));

            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}

STDAPI XblTitleManagedStatsUpdateStatsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblTitleManagedStatistic* statistics,
    _In_ size_t statisticsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(statistics == nullptr || statisticsCount == 0);

    return UpdateStatsAsync(
        xblContextHandle,
        Vector<TitleManagedStatistic>(statistics, statistics + statisticsCount),
        async
    );
}
CATCH_RETURN()

STDAPI XblTitleManagedStatsDeleteStatsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char** names,
    _In_ size_t count,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(names == nullptr || count == 0);

    return UpdateStatsAsync(
        xblContextHandle,
        Vector<TitleManagedStatistic>(names, names + count),
        async
    );
}
CATCH_RETURN()