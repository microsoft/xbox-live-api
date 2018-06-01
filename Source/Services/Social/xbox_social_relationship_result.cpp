// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_social_relationship_result::xbox_social_relationship_result()
{
}

xbox_social_relationship_result::xbox_social_relationship_result(
    _In_ std::shared_ptr<xbox_social_relationship_result_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_VECTOR_INTERNAL_TYPE(xbox_social_relationship_result, xbox_social_relationship, items);
DEFINE_GET_UINT32(xbox_social_relationship_result, total_count);
DEFINE_GET_BOOL(xbox_social_relationship_result, has_next);

pplx::task<xbox_live_result<xbox_social_relationship_result>>
xbox_social_relationship_result::get_next(
    _In_ uint32_t maxItems
    )
{
    task_completion_event<xbox_live_result<xbox_social_relationship_result>> tce;

    auto result = m_internalObj->get_next(maxItems, nullptr,
        [tce](xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> result) 
    { 
        tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(xbox_social_relationship_result, result));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<xbox_social_relationship_result>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<xbox_social_relationship_result>>(tce);
}

xbox_social_relationship_result_internal::xbox_social_relationship_result_internal(
    _In_ xsapi_internal_vector<std::shared_ptr<xbox_social_relationship_internal>> socialRelationships, 
    _In_ uint32_t totalCount
    ) : 
    m_socialRelationships(std::move(socialRelationships)),
    m_totalCount(totalCount)
{
}

void xbox_social_relationship_result_internal::init_next_page_info(
    _In_ std::shared_ptr<social_service_impl> socialImpl,
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t continuationSkip
    )
{
    m_socialImpl = socialImpl;
    m_filter = filter;
    m_continuationSkip = continuationSkip;
}

const xsapi_internal_vector<std::shared_ptr<xbox_social_relationship_internal>>& xbox_social_relationship_result_internal::items() const
{
    return m_socialRelationships;
}

uint32_t xbox_social_relationship_result_internal::total_count() const
{
    return m_totalCount;
}

bool xbox_social_relationship_result_internal::has_next() const
{
    return (m_continuationSkip < m_totalCount);
}

uint32_t xbox_social_relationship_result_internal::continuation_skip() const
{
    return m_continuationSkip;
}

xbox_social_relationship_filter xbox_social_relationship_result_internal::filter() const
{
    return m_filter;
}

xbox_live_result<void> xbox_social_relationship_result_internal::get_next(
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
    )
{
    return m_socialImpl->get_social_relationships(
        m_filter,
        m_continuationSkip,
        maxItems,
        queue,
        callback
        );
}

xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>
xbox_social_relationship_result_internal::deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto result = xsapi_allocate_shared<xbox_social_relationship_result_internal>(
        utils::extract_json_vector<std::shared_ptr<xbox_social_relationship_internal>>(xbox_social_relationship_internal::deserialize, json, "people", errc, true),
        utils::extract_json_int(json, _T("totalCount"), errc, false, 0)
        );

    return xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END