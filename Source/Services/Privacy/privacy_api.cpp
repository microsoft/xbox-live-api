// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_service_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::privacy;

STDAPI XblPrivacyGetAvoidListAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            avoidList = xsapi_internal_vector<uint64_t>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->PrivacyService()->GetAvoidList({
                data->async->queue,
                [
                    &avoidList,
                    async{ data->async }
                ]
            (Result<xsapi_internal_vector<uint64_t>> result)
            {
                if (Succeeded(result))
                {
                    avoidList = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), avoidList.size() * sizeof(uint64_t));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            memcpy(data->buffer, avoidList.data(), avoidList.size() * sizeof(uint64_t));
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

STDAPI XblPrivacyGetAvoidListResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* xuidCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || xuidCount == nullptr);

    size_t resultSize{ 0 };
    RETURN_HR_IF_FAILED(XAsyncGetResultSize(async, &resultSize));
    *xuidCount = resultSize / sizeof(uint64_t);

    return S_OK;
}
CATCH_RETURN()

STDAPI XblPrivacyGetAvoidListResult(
    _In_ XAsyncBlock* async,
    _In_ size_t xuidCount,
    _Out_writes_(xuidCount) uint64_t* xuids
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(xuidCount == 0, S_OK);
    return XAsyncGetResult(async, nullptr, xuidCount * sizeof(uint64_t), xuids, nullptr);
}
CATCH_RETURN()

HRESULT CopyPermissionCheckResult(
    const PermissionCheckResult& result,
    void* buffer
) noexcept
try
{
    auto resultPtr = static_cast<XblPermissionCheckResult*>(buffer);
    auto reasonsPtr = reinterpret_cast<XblPermissionDenyReasonDetails*>(resultPtr + 1);

    new (resultPtr) XblPermissionCheckResult{
        result.isAllowed,
        result.targetXuid,
        result.targetUserType,
        result.permissionRequested,
        reasonsPtr,
        result.reasonsCount
    };

    memcpy(reasonsPtr, result.reasons, sizeof(XblPermissionDenyReasonDetails) * result.reasonsCount);

    return S_OK;
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPermission permission,
    _In_ uint64_t targetXuid,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            result = PermissionCheckResult{},
            targetXuid,
            permission
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->PrivacyService()->CheckPermission(permission, targetXuid, { data->async->queue,
                [
                    &result,
                    async{ data->async }
                ]
            (Result<PermissionCheckResult> r)
            {
                if (Succeeded(r))
                {
                    result = r.ExtractPayload();
                }
                XAsyncComplete(async, r.Hresult(), result.SizeOf());
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            return CopyPermissionCheckResult(result, data->buffer);
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** result,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(result);
    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *result = static_cast<XblPermissionCheckResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionForAnonymousUserAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPermission permission,
    _In_ XblAnonymousUserType userType,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            result = PermissionCheckResult{},
            permission,
            userType
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->PrivacyService()->CheckPermission(permission, userType, { data->async->queue,
                [
                    &result,
                    async{ data->async }
                ]
            (Result<PermissionCheckResult> r)
            {
                if (Succeeded(r))
                {
                    result = r.ExtractPayload();
                }
                XAsyncComplete(async, r.Hresult(), result.SizeOf());
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            return CopyPermissionCheckResult(result, data->buffer);
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionForAnonymousUserResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblPrivacyCheckPermissionForAnonymousUserResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** result,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    return XblPrivacyCheckPermissionResult(async, bufferSize, buffer, result, bufferUsed);
}
CATCH_RETURN()

STDAPI XblPrivacyBatchCheckPermissionAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_reads_(permissionsCount) XblPermission* permissionsToCheck,
    _In_ size_t permissionsCount,
    _In_reads_(xuidsCount) uint64_t* targetXuids,
    _In_ size_t xuidsCount,
    _In_reads_(userTypesCount) XblAnonymousUserType* targetUserTypes,
    _In_ size_t userTypesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF(permissionsToCheck == nullptr || permissionsCount == 0);
    RETURN_HR_INVALIDARGUMENT_IF(targetXuids == nullptr && xuidsCount != 0);
    RETURN_HR_INVALIDARGUMENT_IF(targetUserTypes == nullptr && userTypesCount != 0);
    RETURN_HR_INVALIDARGUMENT_IF(xuidsCount == 0 && userTypesCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            permissions = xsapi_internal_vector<XblPermission>(permissionsToCheck, permissionsToCheck + permissionsCount),
            xuids = xsapi_internal_vector<uint64_t>(targetXuids, targetXuids + xuidsCount),
            userTypes = xsapi_internal_vector<XblAnonymousUserType>(targetUserTypes, targetUserTypes + userTypesCount),
            result = xsapi_internal_vector<PermissionCheckResult>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->PrivacyService()->BatchCheckPermission(std::move(permissions), xuids, userTypes, { data->async->queue,
                [
                    &result,
                    async{ data->async }
                ]
            (Result<xsapi_internal_vector<PermissionCheckResult>> r)
            {
                if (Succeeded(r))
                {
                    result = r.ExtractPayload();
                }

                size_t requiredBufferSize{ 0 };
                for (auto& i : result)
                {
                    requiredBufferSize += i.SizeOf();
                }
                XAsyncComplete(async, r.Hresult(), requiredBufferSize);
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto resultPtr = static_cast<XblPermissionCheckResult*>(data->buffer);
            auto reasonsPtr = reinterpret_cast<XblPermissionDenyReasonDetails*>(resultPtr + result.size());

            for (const auto& elt : result)
            {
                new (resultPtr) XblPermissionCheckResult{
                    elt.isAllowed,
                    elt.targetXuid,
                    elt.targetUserType,
                    elt.permissionRequested,
                    reasonsPtr,
                    elt.reasonsCount
                };

                memcpy(reasonsPtr, elt.reasons, sizeof(XblPermissionDenyReasonDetails) * elt.reasonsCount);

                resultPtr++;
                reasonsPtr += elt.reasonsCount;
            }
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

STDAPI XblPrivacyBatchCheckPermissionResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblPrivacyBatchCheckPermissionResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** results,
    _Out_ size_t* resultsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(results == nullptr || resultsCount == nullptr);

    // bufferUsed is needed to calculate resultsCount
    auto bufferUsedPtr = MakeUnique<size_t>();
    if (bufferUsed == nullptr)
    {
        bufferUsed = bufferUsedPtr.get();
    }

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *results = static_cast<XblPermissionCheckResult*>(buffer);

        // Calulate how many items are in the results array
        size_t count{ 0 };
        size_t verifiedSize{ 0 };
        for(; verifiedSize < *bufferUsed; ++count)
        {
            verifiedSize += sizeof(XblPermissionCheckResult);
            verifiedSize += ((*results)[count].reasonsCount * sizeof(XblPermissionDenyReasonDetails));
        }
        assert(verifiedSize == *bufferUsed);
        *resultsCount = count;
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblPrivacyGetMuteListAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            muteList = xsapi_internal_vector<uint64_t>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->PrivacyService()->GetMuteList({
                data->async->queue,
                [
                    &muteList,
                    async{ data->async }
                ]
            (Result<xsapi_internal_vector<uint64_t>> result)
            {
                if (Succeeded(result))
                {
                    muteList = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), muteList.size() * sizeof(uint64_t));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            memcpy(data->buffer, muteList.data(), muteList.size() * sizeof(uint64_t));
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

STDAPI XblPrivacyGetMuteListResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* xuidCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || xuidCount == nullptr);

    size_t resultSize{ 0 };
    RETURN_HR_IF_FAILED(XAsyncGetResultSize(async, &resultSize));
    *xuidCount = resultSize / sizeof(uint64_t);

    return S_OK;
}
CATCH_RETURN()

STDAPI XblPrivacyGetMuteListResult(
    _In_ XAsyncBlock* async,
    _In_ size_t xuidCount,
    _Out_writes_(xuidCount) uint64_t* xuids
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(xuidCount == 0, S_OK);
    return XAsyncGetResult(async, nullptr, xuidCount * sizeof(uint64_t), xuids, nullptr);
}
CATCH_RETURN()