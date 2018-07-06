// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/leaderboard_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

enum leaderboard_version
{
    leaderboard_version_2013,
    leaderboard_version_2017
};

class leaderboard_result_internal
{
public:
    leaderboard_result_internal();

    leaderboard_result_internal(
        _In_ xsapi_internal_string displayName,
        _In_ uint32_t totalRowCount,
        _In_ xsapi_internal_string continuationToken,
        _In_ std::vector<leaderboard_column> columns,
        _In_ std::vector<leaderboard_row> rows,
        _In_ std::shared_ptr<leaderboard_service_impl> leaderboardService);

    const xsapi_internal_string& display_name() const;
    uint32_t total_row_count() const;
    const std::vector<leaderboard_column>& columns() const;
    const std::vector<leaderboard_row>& rows() const;
    bool has_next() const;
    xbox_live_result<leaderboard_query> get_next_query() const;

    HRESULT get_next(
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback) const;

    void _Set_next_query(std::shared_ptr<leaderboard_global_query> query);
    void _Set_next_query(std::shared_ptr<leaderboard_social_query> query);
    void _Set_next_query(const leaderboard_query& query);
    void _Parse_additional_columns(const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames);

private:
    xsapi_internal_string m_displayName;
    uint32_t m_totalRowCount;
    xsapi_internal_string m_continuationToken;
    std::vector<leaderboard_column> m_columns;
    std::vector<leaderboard_row> m_rows;

    std::shared_ptr<leaderboard_service_impl> m_leaderboardService;

    std::shared_ptr<leaderboard_global_query> m_globalQuery;
    std::shared_ptr<leaderboard_social_query> m_socialQuery;
    leaderboard_query m_nextQuery;
    leaderboard_version m_version;
};


class leaderboard_service_impl : public std::enable_shared_from_this<leaderboard_service_impl>
{
public:
    leaderboard_service_impl(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config_internal> appConfig,
        _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl
        );

    HRESULT get_leaderboard_for_social_group_internal(
        _In_ uint64_t xuid,
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& statName,
        _In_ XblSocialGroupType socialGroup,
        _In_ uint32_t skipToRank,
        _In_ uint64_t skipToXuid,
        _In_ sort_order order,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken,
        _In_ leaderboard_version version,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback
        );

    HRESULT get_leaderboard_internal(
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& name,
        _In_ uint32_t skipToRank,
        _In_ uint64_t skipToXuid,
        _In_ uint64_t xuid,
        _In_ XblSocialGroupType socialGroup,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken,
        _In_ const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames,
        _In_ leaderboard_version version,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback
        );

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
    std::weak_ptr<xbox_live_context_impl> m_xboxLiveContextImpl;

    friend leaderboard_result;
    friend stats::manager::stats_manager_impl;
    friend xbox_live_context_impl;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END