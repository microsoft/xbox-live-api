// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "profile_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

STDAPI XblProfileGetUserProfileAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t xboxUserId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    return XblProfileGetUserProfilesAsync(xboxLiveContextHandle, &xboxUserId, 1, async);
}
CATCH_RETURN()

STDAPI XblProfileGetUserProfilesAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || xboxUserIds == nullptr || xboxUserIdsCount == 0 || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            xuids = xsapi_internal_vector<uint64_t>(xboxUserIds, xboxUserIds + xboxUserIdsCount),
            profiles = xsapi_internal_vector<XblUserProfile>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->ProfileService()->GetUserProfiles(
                std::move(xuids),
                AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>>{
                data->async->queue,
                    [
                        &profiles,
                        asyncBlock{ data->async }
                    ]
                (Result<xsapi_internal_vector<XblUserProfile>> result)
                {
                    if (Succeeded(result))
                    {
                        profiles = result.ExtractPayload();
                    }
                    XAsyncComplete(asyncBlock, result.Hresult(), profiles.size() * sizeof(XblUserProfile));
                }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto profilesPtr = static_cast<XblUserProfile*>(data->buffer);
            for (auto& profile : profiles)
            {
                *profilesPtr++ = profile;
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

STDAPI XblProfileGetUserProfilesForSocialGroupAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_z_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || socialGroup == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            group = xsapi_internal_string{ socialGroup },
            profiles = xsapi_internal_vector<XblUserProfile>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->ProfileService()->GetUserProfilesForSocialGroup(
                std::move(group),
                AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>>{
                data->async->queue,
                    [
                        &profiles,
                        asyncBlock{ data->async }
                    ]
                (Result<xsapi_internal_vector<XblUserProfile>> result)
                {
                    if (Succeeded(result))
                    {
                        profiles = result.ExtractPayload();
                    }
                    XAsyncComplete(asyncBlock, result.Hresult(), profiles.size() * sizeof(XblUserProfile));
                }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto profilesPtr = static_cast<XblUserProfile*>(data->buffer);
            for (auto& profile : profiles)
            {
                *profilesPtr++ = profile;
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

STDAPI XblProfileGetUserProfilesResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* profileCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || profileCount == nullptr);

    size_t bufferSize;
    auto hr = XAsyncGetResultSize(async, &bufferSize);

    if (SUCCEEDED(hr))
    {
        *profileCount = bufferSize / sizeof(XblUserProfile);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblProfileGetUserProfileResult(
    _In_ XAsyncBlock* async,
    _Out_ XblUserProfile* profile
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblUserProfile), profile, nullptr);
}
CATCH_RETURN()

STDAPI XblProfileGetUserProfilesResult(
    _In_ XAsyncBlock* async,
    _In_ size_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(profilesCount == 0, S_OK);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);

    size_t actualProfilesCount = 0;
    auto hr = XblProfileGetUserProfilesResultCount(async, &actualProfilesCount);
    if (SUCCEEDED(hr))
    {
        RETURN_HR_INVALIDARGUMENT_IF(actualProfilesCount > profilesCount);
    }

    hr = XAsyncGetResult(async, nullptr, profilesCount * sizeof(XblUserProfile), profiles, nullptr);
    return hr;
}
CATCH_RETURN()

STDAPI XblProfileGetUserProfilesForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* profileCount
) XBL_NOEXCEPT
try
{
    return XblProfileGetUserProfilesResultCount(async, profileCount);
}
CATCH_RETURN()

STDAPI XblProfileGetUserProfilesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
) XBL_NOEXCEPT
try
{
    return XblProfileGetUserProfilesResult(async, profilesCount, profiles);
}
CATCH_RETURN()
