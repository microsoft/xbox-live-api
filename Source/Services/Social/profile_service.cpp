// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/profile.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "profile_service_impl.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

profile_service::profile_service(
    _In_ std::shared_ptr<profile_service_impl> serviceImpl
    ) :
    m_serviceImpl(std::move(serviceImpl))
{
}

pplx::task<xbox_live_result<xbox_user_profile>>
profile_service::get_user_profile(
    _In_ string_t xboxUserId
    )
{
    task_completion_event<xbox_live_result<xbox_user_profile>> tce;

    auto result = m_serviceImpl->get_user_profile(
        utils::internal_string_from_external_string(xboxUserId),
        XSAPI_DEFAULT_TASKGROUP,
        [tce](xbox_live_result<xbox_user_profile> result) 
    { 
        tce.set(result); 
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<xbox_user_profile>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<xbox_user_profile>>(tce);
}

pplx::task<xbox_live_result<std::vector<xbox_user_profile>>>
profile_service::get_user_profiles(
    _In_ const std::vector< string_t >& xboxUserIds
    )
{
    task_completion_event<xbox_live_result<std::vector<xbox_user_profile>>> tce;
    auto result = m_serviceImpl->get_user_profiles(
        utils::internal_string_vector_from_std_string_vector(xboxUserIds), 
        XSAPI_DEFAULT_TASKGROUP,
        [tce](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result) 
    {
        tce.set(xbox_live_result<std::vector<xbox_user_profile>>(utils::std_vector_from_internal_vector(result.payload())));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<xbox_user_profile>>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<std::vector<xbox_user_profile>>>(tce);
}

pplx::task<xbox_live_result<std::vector<xbox_user_profile>>> 
profile_service::get_user_profiles_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    task_completion_event<xbox_live_result<std::vector<xbox_user_profile>>> tce;
    auto result = m_serviceImpl->get_user_profiles_for_social_group(
        utils::internal_string_from_external_string(socialGroup), 
        XSAPI_DEFAULT_TASKGROUP,
        [tce](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result)
    {
        tce.set(xbox_live_result<std::vector<xbox_user_profile>>(utils::std_vector_from_internal_vector(result.payload())));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<xbox_user_profile>>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<std::vector<xbox_user_profile>>>(tce);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END