// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/presence_c.h"
#include "presence_internal.h"
#include "xbox_live_context_internal.h"
#include "real_time_activity_manager.h"

using namespace xbox::services;
using namespace xbox::services::presence;

STDAPI XblPresenceRecordGetXuid(
    _In_ XblPresenceRecordHandle record,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(record == nullptr || xuid == nullptr);
    *xuid = record->Xuid();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceRecordGetUserState(
    _In_ XblPresenceRecordHandle record,
    _Out_ XblPresenceUserState* userState
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(record == nullptr || userState == nullptr);
    *userState = record->UserState();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceRecordGetDeviceRecords(
    _In_ XblPresenceRecordHandle record,
    _Out_ const XblPresenceDeviceRecord** deviceRecords,
    _Out_ size_t* deviceRecordsCount
) XBL_NOEXCEPT 
try
{
    RETURN_HR_INVALIDARGUMENT_IF(record == nullptr || deviceRecords == nullptr || deviceRecordsCount == nullptr);

    *deviceRecords = record->DeviceRecords().data();
    *deviceRecordsCount = record->DeviceRecords().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceRecordDuplicateHandle(
    _In_ XblPresenceRecordHandle record,
    _Out_ XblPresenceRecordHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(record == nullptr || duplicatedHandle == nullptr);

    record->AddRef();
    *duplicatedHandle = record;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblPresenceRecordCloseHandle(
    _In_ XblPresenceRecordHandle record
) XBL_NOEXCEPT
try
{
    if (record)
    {
        record->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblPresenceSetPresenceAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ bool isUserActiveInTitle,
    _In_opt_ XblPresenceRichPresenceIds* richPresenceIds,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            titleRequest = TitleRequest{ isUserActiveInTitle, richPresenceIds }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_PENDING_OR_HR(xblContext->PresenceService()->SetPresence(
                std::move(titleRequest),
                AsyncContext<HRESULT>{ data->async }
            ));
        }

        default:
        {
            return S_OK;
        }
        } // end switch
    });
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            xuid,
            presenceRecord = std::shared_ptr<XblPresenceRecord>{ nullptr } // result
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_PENDING_OR_HR(xblContext->PresenceService()->GetPresence(
                xuid,
                AsyncContext<Result<std::shared_ptr<XblPresenceRecord>>>{
                TaskQueue{ data->async->queue },
                    [
                        &presenceRecord,
                        asyncBlock{ data->async }
                    ]
                (Result<std::shared_ptr<XblPresenceRecord>> result)
                {
                    if (Succeeded(result))
                    {
                        presenceRecord = result.ExtractPayload();
                    }
                    XAsyncComplete(asyncBlock, result.Hresult(), sizeof(XblPresenceRecordHandle));
                }
            }));
        }
        case XAsyncOp::GetResult:
        {
            auto handlePtr = static_cast<XblPresenceRecordHandle*>(data->buffer);
            presenceRecord->AddRef();
            *handlePtr = presenceRecord.get();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        } // end switch
    });
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceResult(
    _In_ XAsyncBlock* async,
    _Out_ XblPresenceRecordHandle* presenceRecordHandle
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblPresenceRecordHandle), presenceRecordHandle, nullptr);
}
CATCH_RETURN()

STDAPI GetBatchPresenceProvider(
    _In_ XblContextHandle xblContextHandle,
    _In_ UserBatchRequest&& request,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            request,
            presenceRecords = Vector<std::shared_ptr<XblPresenceRecord>>{} // result
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_PENDING_OR_HR(xblContext->PresenceService()->GetBatchPresence(
                std::move(request),
                {
                TaskQueue{ data->async->queue },
                    [
                        &presenceRecords,
                        asyncBlock{ data->async }
                    ]
                (Result<Vector<std::shared_ptr<XblPresenceRecord>>> result)
                {
                    if (Succeeded(result))
                    {
                        presenceRecords = result.ExtractPayload();
                    }
                    XAsyncComplete(asyncBlock, result.Hresult(), sizeof(XblPresenceRecordHandle)* presenceRecords.size());
                }
                }));
        }
        case XAsyncOp::GetResult:
        {
            auto handlesPtr = static_cast<XblPresenceRecordHandle*>(data->buffer);
            for (const auto& record : presenceRecords)
            {
                record->AddRef();
                *handlesPtr++ = record.get();
            }
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        } // end switch
    });
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForMultipleUsersAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_ XblPresenceQueryFilters* filters,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || xuids == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return GetBatchPresenceProvider(
        xblContextHandle,
        UserBatchRequest{ xuids, xuidsCount, filters },
        async
    );
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForMultipleUsersResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(resultCount);

    size_t bufferSize;
    auto hr = XAsyncGetResultSize(async, &bufferSize);

    if (SUCCEEDED(hr))
    {
        *resultCount = bufferSize / sizeof(XblPresenceRecordHandle);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForMultipleUsersResult(
    _In_ XAsyncBlock* async,
    _Out_writes_(presenceRecordHandlesCount) XblPresenceRecordHandle* presenceRecordHandles,
    _In_ size_t presenceRecordHandlesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(presenceRecordHandlesCount == 0, S_OK);
    RETURN_HR_INVALIDARGUMENT_IF(presenceRecordHandles == nullptr);

    return XAsyncGetResult(async, nullptr, sizeof(XblPresenceRecordHandle) * presenceRecordHandlesCount, presenceRecordHandles, nullptr);
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForSocialGroupAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_z_ const char* socialGroupName,
    _In_opt_ uint64_t* socialGroupOwnerXuid,
    _In_opt_ XblPresenceQueryFilters* filters,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || socialGroupName == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return GetBatchPresenceProvider(
        xblContextHandle,
        UserBatchRequest{ socialGroupName, socialGroupOwnerXuid, filters },
        async
    );
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultCount
) XBL_NOEXCEPT
try
{
    return XblPresenceGetPresenceForMultipleUsersResultCount(async, resultCount);
}
CATCH_RETURN()

STDAPI XblPresenceGetPresenceForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _Out_ XblPresenceRecordHandle* presenceRecordHandles,
    _In_ size_t presenceRecordHandlesCount
) XBL_NOEXCEPT
try
{
    return XblPresenceGetPresenceForMultipleUsersResult(async, presenceRecordHandles, presenceRecordHandlesCount);
}
CATCH_RETURN()

namespace xbox {
    namespace services {
        namespace presence {
            namespace legacy {
                struct Subscription : public XblRealTimeActivitySubscription
                {
                    Subscription(
                        std::shared_ptr<PresenceService> presenceService,
                        uint64_t xuid
                    ) noexcept
                        : m_presenceService{ presenceService },
                        m_xuid{ xuid } 
                    {
                        presenceService->TrackUsers(Vector<uint64_t>{ m_xuid });
                    }

                    virtual ~Subscription() noexcept
                    {
                        if (auto presenceService{ m_presenceService.lock() })
                        {
                            presenceService->StopTrackingUsers(Vector<uint64_t>{ m_xuid });
                        }
                    }

                protected:
                    std::weak_ptr<PresenceService> m_presenceService;

                private:
                    uint64_t m_xuid{ 0 };
                };

                struct TitleSubscription : public Subscription
                {
                    TitleSubscription(
                        std::shared_ptr<PresenceService> presenceService,
                        uint64_t xuid,
                        uint32_t titleId
                    ) noexcept
                        : Subscription{ presenceService, xuid },
                        m_titleId{ titleId }
                    {
                        presenceService->TrackAdditionalTitles(Vector<uint32_t>{ m_titleId });
                    }

                    ~TitleSubscription() noexcept
                    {
                        if (auto presenceService{ m_presenceService.lock() })
                        {
                            presenceService->StopTrackingAdditionalTitles(Vector<uint32_t>{ m_titleId });
                        }
                    }

                private:
                    uint32_t m_titleId{ 0 };
                };
            }
        }
    }
}

STDAPI XblPresenceSubscribeToDevicePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(subscriptionHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || subscriptionHandle == nullptr);

    *subscriptionHandle = Make<presence::legacy::Subscription>(xblContextHandle->PresenceService(), xuid);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceUnsubscribeFromDevicePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || subscriptionHandle == nullptr);

    Delete(subscriptionHandle);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceSubscribeToTitlePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(subscriptionHandle);
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || subscriptionHandle == nullptr);

    *subscriptionHandle = Make<presence::legacy::TitleSubscription>(xblContextHandle->PresenceService(), xuid, titleId);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceUnsubscribeFromTitlePresenceChange(
    _In_ XblContextHandle xblContext,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || subscriptionHandle == nullptr);

    Delete(subscriptionHandle);
    return S_OK;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblPresenceAddDevicePresenceChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblPresenceDevicePresenceChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    if (xblContext == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    return xblContext->PresenceService()->AddDevicePresenceChangedHandler(
        [
            handler,
            context
        ]
    (uint64_t xuid, XblPresenceDeviceType deviceType, bool isOnline)
    {
        try
        {
            handler(context, xuid, deviceType, isOnline);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI XblPresenceRemoveDevicePresenceChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContext);
    xblContext->PresenceService()->RemoveDevicePresenceChangedHandler(token);
    return S_OK;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblPresenceAddTitlePresenceChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblPresenceTitlePresenceChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    if (xblContext == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    return xblContext->PresenceService()->AddTitlePresenceChangedHandler(
        [
            handler,
            context
        ]
    (uint64_t xuid, uint32_t titleId, XblPresenceTitleState state)
    {
        try
        {
            handler(context, xuid, titleId, state);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI XblPresenceRemoveTitlePresenceChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContext);
    xblContext->PresenceService()->RemoveTitlePresenceChangedHandler(token);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblPresenceTrackUsers(
    _In_ XblContextHandle xblContext,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || xuids == nullptr);
    return xblContext->PresenceService()->TrackUsers(Vector<uint64_t>(xuids, xuids + xuidsCount));
}
CATCH_RETURN()

STDAPI XblPresenceStopTrackingUsers(
    _In_ XblContextHandle xblContext,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || xuids == nullptr);
    return xblContext->PresenceService()->StopTrackingUsers(Vector<uint64_t>(xuids, xuids + xuidsCount));
}
CATCH_RETURN()

STDAPI XblPresenceTrackAdditionalTitles(
    _In_ XblContextHandle xblContext,
    _In_ const uint32_t* titleIds,
    _In_ size_t titleIdsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || titleIds == nullptr);
    return xblContext->PresenceService()->TrackAdditionalTitles(Vector<uint32_t>(titleIds, titleIds + titleIdsCount));
}
CATCH_RETURN()

STDAPI XblPresenceStopTrackingAdditionalTitles(
    _In_ XblContextHandle xblContext,
    _In_ const uint32_t* titleIds,
    _In_ size_t titleIdsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || titleIds == nullptr);
    return xblContext->PresenceService()->StopTrackingAdditionalTitles(Vector<uint32_t>(titleIds, titleIds + titleIdsCount));
}
CATCH_RETURN()