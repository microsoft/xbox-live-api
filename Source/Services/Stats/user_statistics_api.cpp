// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/user_statistics_c.h"
#include "user_statistics_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::user_statistics;

STDAPI XblUserStatisticsGetSingleUserStatisticAsync(
    _In_ XblContextHandle xblContext,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* statisticName,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    return XblUserStatisticsGetSingleUserStatisticsAsync(
        xblContext,
        xboxUserId,
        serviceConfigurationId,
        &statisticName,
        1,
        async
    );
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetSingleUserStatisticResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetSingleUserStatisticResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblUserStatisticsResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetSingleUserStatisticsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF(statisticNames == nullptr || statisticNamesCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            resultPayload = UserStatisticsResult{},
            xboxUserId,
            scid = String{ serviceConfigurationId },
            statisticNames{ Vector<String>(statisticNames, statisticNames + statisticNamesCount) }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->UserStatisticsService()->GetSingleUserStatistics(
                xboxUserId,
                scid,
                statisticNames,
                {
                    TaskQueue{ data->async->queue }.DeriveWorkerQueue(),
                    [
                        &resultPayload,
                        async{ data->async }
                    ]
                    (Result<UserStatisticsResult> result)
                    {
                        if (Succeeded(result))
                        {
                            resultPayload = std::move(result.ExtractPayload());
                        }
                        XAsyncComplete(async, result.Hresult(), resultPayload.SizeOf());
                    }
                }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            buffer = resultPayload.Serialize(buffer);
            assert(static_cast<void*>(buffer) == static_cast<void*>(static_cast<char*>(data->buffer) + resultPayload.SizeOf()));
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetSingleUserStatisticsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetSingleUserStatisticsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblUserStatisticsResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsAsync(
    _In_ XblContextHandle xblContext,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF(statisticNames == nullptr || statisticNamesCount == 0);

    XblRequestedStatistics requestedStats{ {}, statisticNames, static_cast<uint32_t>(statisticNamesCount) };
    utils::strcpy(requestedStats.serviceConfigurationId, sizeof(requestedStats.serviceConfigurationId), serviceConfigurationId);

    return XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
        xblContext,
        xboxUserIds,
        static_cast<uint32_t>(xboxUserIdsCount),
        &requestedStats,
        1,
        async
    );

}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** results,
    _Out_ size_t* resultsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(results == nullptr || resultsCount == nullptr);

    size_t bufferUsedTemp{};
    if (bufferUsed == nullptr)
    {
        bufferUsed = &bufferUsedTemp;
    }

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *results = static_cast<XblUserStatisticsResult*>(buffer);
        auto sizePtr = reinterpret_cast<size_t*>(static_cast<char*>(buffer) + *bufferUsed) - 1;
        *resultsCount = *sizePtr;
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ const XblRequestedStatistics* requestedStats,
    _In_ uint32_t requestedStatsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xboxUserIds == nullptr || xboxUserIdsCount == 0);
    RETURN_HR_INVALIDARGUMENT_IF(requestedStats == nullptr || requestedStatsCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            resultPayload{ Vector<UserStatisticsResult>{} },
            xuids{ Vector<uint64_t>(xboxUserIds, xboxUserIds + xboxUserIdsCount) },
            requestedStats{ Vector<RequestedStatistics>(requestedStats, requestedStats + requestedStatsCount) }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->UserStatisticsService()->GetMultipleUserStatisticsForMultipleServiceConfigurations(
                xuids,
                requestedStats,
                {
                    TaskQueue{ data->async->queue }.DeriveWorkerQueue(),
                    [
                        &resultPayload,
                        async{ data->async }
                    ]
                    (Result<Vector<UserStatisticsResult>> result)
                    {
                        size_t bufferSize{ 0 };
                        if (Succeeded(result))
                        {
                            resultPayload = std::move(result.ExtractPayload());
                            for (auto& userStatResult : resultPayload)
                            {
                                bufferSize += userStatResult.SizeOf();
                            }
                            // Add some padding at the end of the buffer to store the number of XblUserStatisticsResult
                            // objects since there is no way to deduce it just from the buffer size
                            bufferSize += sizeof(size_t);
                        }

                        XAsyncComplete(async, result.Hresult(), bufferSize);
                    }
                }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            XblUserStatisticsResult * resultArr = reinterpret_cast<XblUserStatisticsResult*>(buffer);
            buffer += sizeof(XblUserStatisticsResult) * resultPayload.size();
            size_t bufferSize{};

            for (size_t i = 0; i < resultPayload.size(); i++)
            {
                bufferSize += resultPayload[i].SizeOf();
                buffer = resultPayload[i].Serialize(&resultArr[i], buffer);
            }

            // Validate that everything that was expected to be written was written to the buffer
            size_t * resultArrSize = reinterpret_cast<size_t*>(buffer);
            XSAPI_ASSERT(static_cast<void*>(resultArrSize) == static_cast<void*>(static_cast<char*>(data->buffer) + bufferSize));
            *resultArrSize = resultPayload.size();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** results,
    _Out_ size_t* resultsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(results == nullptr || resultsCount == nullptr);

    size_t bufferUsedTemp{};
    if (bufferUsed == nullptr)
    {
        bufferUsed = &bufferUsedTemp;
    }

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *results = static_cast<XblUserStatisticsResult*>(buffer);
        auto sizePtr = reinterpret_cast<size_t*>(static_cast<char*>(buffer) + *bufferUsed) - 1;
        *resultsCount = *sizePtr;
    }
    return hr;
}
CATCH_RETURN()

namespace xbox {
    namespace services {
        namespace user_statistics {
            namespace legacy {
                struct Subscription : public XblRealTimeActivitySubscription
                {
                    Subscription(uint64_t _xuid, String _scid, String _statName) noexcept
                        : xuid{ _xuid },
                        scid{ std::move(_scid) },
                        statName{ std::move(_statName) }
                    {
                    }
                    uint64_t xuid;
                    String scid;
                    String statName;
                };
            }
        }
    }
}

STDAPI XblUserStatisticsSubscribeToStatisticChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _In_z_ const char* scid,
    _In_z_ const char* statisticName,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscriptionHandle);

    HRESULT hr = XblUserStatisticsTrackStatistics(
        xblContextHandle,
        &xuid,
        1,
        scid,
        &statisticName,
        1
    );

    if (SUCCEEDED(hr))
    {
        *subscriptionHandle = Make<user_statistics::legacy::Subscription>(xuid, scid, statisticName);
    }
    return hr;
}
CATCH_RETURN();

STDAPI XblUserStatisticsUnsubscribeFromStatisticChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscriptionHandle);

    auto sub{ static_cast<user_statistics::legacy::Subscription*>(subscriptionHandle) };
    const char* statName{ sub->statName.data() };

    HRESULT hr = XblUserStatisticsStopTrackingStatistics(
        xblContextHandle,
        &sub->xuid,
        1,
        sub->scid.data(),
        &statName,
        1
    );

    assert(SUCCEEDED(hr));
    Delete(sub);
    return hr;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblUserStatisticsAddStatisticChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblStatisticChangedHandler handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT
try
{
    if (xblContextHandle == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    return xblContextHandle->UserStatisticsService()->AddStatisticChangedHandler(
        [
            handler,
            handlerContext
        ]
    (const StatisticChangeEventArgs& eventArgs)
    {
        try
        {
            handler(eventArgs, handlerContext);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN_WITH(-1)

STDAPI_(void) XblUserStatisticsRemoveStatisticChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext context
) XBL_NOEXCEPT
try
{
    if (xblContextHandle != nullptr)
    {
        xblContextHandle->UserStatisticsService()->RemoveStatisticChangedHandler(context);
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblUserStatisticsTrackStatistics(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xboxUserIds == nullptr || xboxUserIdsCount == 0);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF(statisticNames == nullptr || statisticNamesCount == 0);

    return xblContextHandle->UserStatisticsService()->TrackStatistics(
        Vector<uint64_t>{ xboxUserIds, xboxUserIds + xboxUserIdsCount },
        serviceConfigurationId,
        Vector<String>{ statisticNames, statisticNames + statisticNamesCount }
    );
}
CATCH_RETURN()

STDAPI XblUserStatisticsStopTrackingStatistics(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xboxUserIds == nullptr || xboxUserIdsCount == 0);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF(statisticNames == nullptr || statisticNamesCount == 0);

    return xblContextHandle->UserStatisticsService()->StopTrackingStatistics(
        Vector<uint64_t>{ xboxUserIds, xboxUserIds + xboxUserIdsCount },
        serviceConfigurationId,
        Vector<String>{ statisticNames, statisticNames + statisticNamesCount }
    );
}
CATCH_RETURN()

STDAPI XblUserStatisticsStopTrackingUsers(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xboxUserIds == nullptr || xboxUserIdsCount == 0);

    return xblContextHandle->UserStatisticsService()->StopTrackingUsers(
        Vector<uint64_t>{ xboxUserIds, xboxUserIds + xboxUserIdsCount }
    );
}
CATCH_RETURN()
