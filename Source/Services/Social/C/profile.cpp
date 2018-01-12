// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "profile_service_impl.h"
#include "xbox_live_context_impl_c.h"
#include "profile_helpers.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfile(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_USER_PROFILE_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
{
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    auto result = profileService->get_user_profile(
        utils::utf16_from_utf8(xboxUserId), 
        taskGroupId,
        [](xbox_live_result<xbox_user_profile> result, void* context)
        {
            auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);

            XBL_RESULT_INFO cResult;
            std::string errorMessage = result.err_message();
            cResult.errorMessage = errorMessage.data();
            cResult.errorCode = ::utils_c::xsapi_result_from_xbox_live_result_err(result.err());

            auto callback = (XBL_GET_USER_PROFILE_COMPLETION_ROUTINE)(clientCallbackInfo.completionFunction);
            if (!result.err())
            {
                xbl_xbox_user_profiles_wrapper wrapper(result.payload());
                callback(cResult, wrapper.xbl_xbox_user_profile(), clientCallbackInfo.clientContext);
            }
            else
            {
                callback(cResult, nullptr, clientCallbackInfo.clientContext);
            }
        },
        context);

    return utils_c::xsapi_result_from_xbox_live_result_err(result.err());
}

void get_user_profiles_complete(
    xbox_live_result<std::vector<xbox_user_profile>> result, 
    void* context
    )
{
    auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);

    XBL_RESULT_INFO cResult;
    std::string errorMessage = result.err_message();
    cResult.errorMessage = errorMessage.data();
    cResult.errorCode = ::utils_c::xsapi_result_from_xbox_live_result_err(result.err());

    auto callback = (XBL_GET_USER_PROFILES_COMPLETION_ROUTINE)(clientCallbackInfo.completionFunction);
    if (!result.err())
    {
        xbl_xbox_user_profiles_wrapper wrapper(result.payload());
        callback(cResult, wrapper.xbl_xbox_user_profile(), wrapper.profiles_count(), clientCallbackInfo.clientContext);
    }
    else
    {
        callback(cResult, nullptr, 0, clientCallbackInfo.clientContext);
    }
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfiles(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR *xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_USER_PROFILES_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
{
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    auto result = profileService->get_user_profiles(
        utils::string_array_to_string_vector(xboxUserIds, xboxUserIdsCount),
        taskGroupId,
        get_user_profiles_complete,
        context);

    return utils_c::xsapi_result_from_xbox_live_result_err(result.err());
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetUserProfilesForSocialGroup(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR socialGroup,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_USER_PROFILES_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
{
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    auto result = profileService->get_user_profiles_for_social_group(
        utils::utf16_from_utf8(socialGroup),
        taskGroupId,
        get_user_profiles_complete,
        context);

    return utils_c::xsapi_result_from_xbox_live_result_err(result.err());
}
