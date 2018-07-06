// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/leaderboard.h"
#include "leaderboard_service_impl.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_service::leaderboard_service(
    _In_ std::shared_ptr<leaderboard_service_impl> serviceImpl
    ) :
    m_serviceImpl(std::move(serviceImpl))
{
}


pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        NO_XUID,
        NO_SOCIAL_GROUP,
        NO_MAX_ITEMS,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        skipToRank,
        NO_SKIP_XUID,
        NO_XUID,
        NO_SOCIAL_GROUP,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}


pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        skipToRank,
        NO_SKIP_XUID,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        skipToXuid,
        NO_XUID,
        NO_SOCIAL_GROUP,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}

_XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        skipToXuid,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        NO_SORT_ORDER,
        maxItems,
        NO_CONTINUATION,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        sortOrder,
        maxItems,
        NO_CONTINUATION,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_rank(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        skipToRank,
        NO_SKIP_XUID,
        sortOrder,
        maxItems,
        NO_CONTINUATION,
        leaderboard_version_2013);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_xuid(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        skipToXuid,
        sortOrder,
        maxItems,
        NO_CONTINUATION,
        leaderboard_version_2013);
}

XblSocialGroupType string_to_social_group(_In_ const string_t& socialGroup)
{
    if (utils::str_icmp(socialGroup, _T("People")) == 0)
    {
        return XblSocialGroupType_People;
    }

    if (utils::str_icmp(socialGroup, _T("Favorites")) == 0)
    {
        return XblSocialGroupType_Favorites;
    }

    return XblSocialGroupType_None;
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_internal(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ const std::vector<string_t>& additionalColumnNames,
    _In_ leaderboard_version version
    )
{
    pplx::task_completion_event<xbox_live_result<leaderboard_result>> tce;

    auto hr = m_serviceImpl->get_leaderboard_internal(
        utils::internal_string_from_string_t(scid),
        utils::internal_string_from_string_t(name),
        skipToRank,
        utils::string_t_to_uint64(skipToXuid),
        utils::string_t_to_uint64(xuid),
        string_to_social_group(socialGroup),
        maxItems,
        utils::internal_string_from_string_t(continuationToken),
        utils::internal_string_vector_from_std_string_vector(additionalColumnNames),
        version,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<leaderboard_result_internal>> result)
    {
        tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(leaderboard_result, result));
    });

    if (FAILED(hr))
    {
        std::error_code errorCode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return pplx::task_from_result(xbox_live_result<leaderboard_result>(errorCode));
    }
    return pplx::task<xbox_live_result<leaderboard_result>>(tce);
}

sort_order convert_sort_order_string_to_enum(_In_ const string_t& sortOrder)
{
    if (utils::str_icmp(sortOrder, _T("ascending")) == 0)
    {
        return sort_order::ascending;
    }

    return sort_order::descending;
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_internal(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ leaderboard_version version
    )
{
    pplx::task_completion_event<xbox_live_result<leaderboard_result>> tce;

    auto hr = m_serviceImpl->get_leaderboard_for_social_group_internal(
        utils::string_t_to_uint64(xuid),
        utils::internal_string_from_string_t(scid),
        utils::internal_string_from_string_t(statName),
        string_to_social_group(socialGroup),
        skipToRank,
        utils::string_t_to_uint64(skipToXuid),
        convert_sort_order_string_to_enum(sortOrder),
        maxItems,
        utils::internal_string_from_string_t(continuationToken),
        version,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<leaderboard_result_internal>> result)
    {
        tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(leaderboard_result, result));
    });

    if (FAILED(hr))
    {
        std::error_code errorCode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return pplx::task_from_result(xbox_live_result<leaderboard_result>(errorCode));
    }
    return pplx::task<xbox_live_result<leaderboard_result>>(tce);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END

