// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "profile_internal.h"
#include "xbox_live_context_impl_c.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

XBL_API XSAPI_RESULT XBL_CALLING_CONV
XSAPIGetUserProfile(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ XSAPI_GET_USER_PROFILE_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
) XBL_NOEXCEPT
{
    auto profileService = pContext->pImpl->cppObject()->profile_service_impl();

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    auto result = profileService->get_user_profile(
        ::utils_c::to_utf16string(xboxUserId),
        [](xbox_live_result<xbox_user_profile> result, void* context)
        {
            auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);

            XSAPI_RESULT_INFO cResult;
            std::string errorMessage = result.err_message();
            cResult.errorMessage = errorMessage.data();
            cResult.errorCode = ::utils_c::xsapi_result_from_xbox_live_result_err(result.err());

            XSAPI_XBOX_USER_PROFILE cProfile;
            std::string gamertag;
            if (!result.err())
            {
                gamertag = ::utils_c::to_utf8string(result.payload().gamertag());
                cProfile.gamertag = gamertag.data();
            }
            ((XSAPI_GET_USER_PROFILE_COMPLETION_ROUTINE)(clientCallbackInfo.completionFunction))(cResult, cProfile, clientCallbackInfo.clientContext);
        },
        context, taskGroupId);

    // TODO translate c++ result and return it
    return XSAPI_RESULT_OK;
}