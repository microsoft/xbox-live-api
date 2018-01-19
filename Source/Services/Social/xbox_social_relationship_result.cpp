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
xbox_social_relationship_result::xbox_social_relationship_result():
    m_totalCount(0),
    m_filter(xbox_social_relationship_filter::all),
    m_continuationSkip(0)
{
}

xbox_social_relationship_result::xbox_social_relationship_result(
    _In_ std::vector<xbox_social_relationship> socialRelationships, 
    _In_ uint32_t totalCount
    ) : 
    m_socialRelationships(std::move(socialRelationships)),
    m_totalCount(totalCount)
{
}

void xbox_social_relationship_result::_Init_next_page_info(
    _In_ std::shared_ptr<social_service_impl> socialImpl,
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t continuationSkip
    )
{
    m_socialImpl = socialImpl;
    m_filter = filter;
    m_continuationSkip = continuationSkip;
}

const std::vector< xbox_social_relationship >&
xbox_social_relationship_result::items() const
{
    return m_socialRelationships;
}

uint32_t xbox_social_relationship_result::total_count() const
{
    return m_totalCount;
}

bool
xbox_social_relationship_result::has_next()
{
    return (m_continuationSkip < m_totalCount);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
xbox_social_relationship_result::get_next(
    _In_ uint32_t maxItems
    )
{   
    // TODO 
    return pplx::task_from_result<xbox_live_result<xbox_social_relationship_result>>(xbox_live_result<xbox_social_relationship_result>());
    //return m_socialImpl->get_social_relationships(m_filter, m_continuationSkip, maxItems);
} 

xbox_live_result<xbox_social_relationship_result>
xbox_social_relationship_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<xbox_social_relationship_result>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto result = xbox_social_relationship_result(
        utils::extract_json_vector<xbox_social_relationship>(xbox_social_relationship::_Deserialize, json, _T("people"), errc, true),
        utils::extract_json_int(json, _T("totalCount"), errc, false, 0)
        );

    return xbox_live_result<xbox_social_relationship_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END