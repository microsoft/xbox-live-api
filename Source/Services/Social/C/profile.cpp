// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "profile_internal.h"
#include "xbox_live_context_impl_c.h"
#include "profile_helpers.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfile(
    _In_ XBL_XBOX_LIVE_CONTEXT* xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_USER_PROFILE_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || xboxUserId == nullptr);
    auto profileService = xboxLiveContext->pImpl->cppObject()->profile_service_impl();

    auto result = profileService->get_user_profile(
        xboxUserId,
        taskGroupId,
        [callback, callbackContext](xbox_live_result<std::shared_ptr<xbox_user_profile_internal>> result)
    {
        XBL_RESULT cResult = utils::create_xbl_result(result.err());
        if (!result.err())
        {
            xbl_xbox_user_profiles_wrapper wrapper(result.payload());
            callback(cResult, wrapper.xbl_xbox_user_profile(), callbackContext);
        }
        else
        {
            callback(cResult, nullptr, callbackContext);
        }
    });

    return utils::create_xbl_result(result.err());
}
CATCH_RETURN()

void get_user_profiles_complete(
    xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>> result,
    void *callbackContext,
    XBL_GET_USER_PROFILES_COMPLETION_ROUTINE callback
    )
{
    XBL_RESULT cResult = utils::create_xbl_result(result.err());
    if (!result.err())
    {
        xbl_xbox_user_profiles_wrapper wrapper(result.payload());
        callback(cResult, wrapper.xbl_xbox_user_profile(), wrapper.profiles_count(), callbackContext);
    }
    else
    {
        callback(cResult, nullptr, 0, callbackContext);
    }
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfiles(
    _In_ XBL_XBOX_LIVE_CONTEXT* xboxLiveContext,
    _In_ PCSTR *xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_USER_PROFILES_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || xboxUserIds == nullptr);
    auto profileService = xboxLiveContext->pImpl->cppObject()->profile_service_impl();

    auto result = profileService->get_user_profiles(
        utils::string_array_to_internal_string_vector(xboxUserIds, xboxUserIdsCount),
        taskGroupId,
        [callback, callbackContext](xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>> result)
        {
            get_user_profiles_complete(result, callbackContext, callback);
        });

    return utils::create_xbl_result(result.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfilesForSocialGroup(
    _In_ XBL_XBOX_LIVE_CONTEXT* xboxLiveContext,
    _In_ PCSTR socialGroup,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_USER_PROFILES_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || socialGroup == nullptr);
    auto profileService = xboxLiveContext->pImpl->cppObject()->profile_service_impl();

    auto result = profileService->get_user_profiles_for_social_group(
        socialGroup,
        taskGroupId,
        [callback, callbackContext](xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>> result)
        {
            get_user_profiles_complete(result, callbackContext, callback);
        });

    return utils::create_xbl_result(result.err());
}
CATCH_RETURN()