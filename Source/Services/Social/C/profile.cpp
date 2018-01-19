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
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || xboxUserId == nullptr);
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();



    //auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    //auto result = profileService->get_user_profile(
    //    xboxUserId,
    //    taskGroupId,
    //    [](xbox_live_result<xbox_user_profile> result, void* context)
    //    {
    //        auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);

    //        XBL_RESULT cResult = utils::create_xbl_result(result.err());
    //        auto callback = (XBL_GET_USER_PROFILE_COMPLETION_ROUTINE)(clientCallbackInfo.completionFunction);
    //        if (!result.err())
    //        {
    //            xbl_xbox_user_profiles_wrapper wrapper(result.payload());
    //            callback(cResult, wrapper.xbl_xbox_user_profile(), clientCallbackInfo.clientContext);
    //        }
    //        else
    //        {
    //            callback(cResult, nullptr, clientCallbackInfo.clientContext);
    //        }
    //    },
    //    context);

    auto result = profileService->get_user_profile(
        xboxUserId,
        taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<xbox_user_profile> result)
    {
        XBL_RESULT cResult = utils::create_xbl_result(result.err());
        if (!result.err())
        {
            xbl_xbox_user_profiles_wrapper wrapper(result.payload());
            completionRoutine(cResult, wrapper.xbl_xbox_user_profile(), completionRoutineContext);
        }
        else
        {
            completionRoutine(cResult, nullptr, completionRoutineContext);
        }
    });

    return utils::create_xbl_result(result.err());
}

void get_user_profiles_complete(
    xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result,
    XBL_GET_USER_PROFILES_COMPLETION_ROUTINE completionRoutine,
    void *completionRoutineContext
    )
{
    XBL_RESULT cResult = utils::create_xbl_result(result.err());
    if (!result.err())
    {
        xbl_xbox_user_profiles_wrapper wrapper(result.payload());
        completionRoutine(cResult, wrapper.xbl_xbox_user_profile(), wrapper.profiles_count(), completionRoutineContext);
    }
    else
    {
        completionRoutine(cResult, nullptr, 0, completionRoutineContext);
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
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || xboxUserIds == nullptr);
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto result = profileService->get_user_profiles(
        utils::string_array_to_internal_string_vector(xboxUserIds, xboxUserIdsCount),
        taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result)
        {
            get_user_profiles_complete(result, completionRoutine, completionRoutineContext);
        });

    return utils::create_xbl_result(result.err());
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
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || socialGroup == nullptr);
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto result = profileService->get_user_profiles_for_social_group(
        socialGroup,
        taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result)
        {
            get_user_profiles_complete(result, completionRoutine, completionRoutineContext);
        });

    return utils::create_xbl_result(result.err());
}
