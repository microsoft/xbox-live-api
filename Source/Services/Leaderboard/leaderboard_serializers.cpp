// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "utils.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN
namespace serializers {

leaderboard_stat_type parse_stat_type(
    _In_ const string_t& type
    )
{
    if (type == _T("Integer"))
        return leaderboard_stat_type::stat_uint64;
    if (type == _T("Double"))
        return leaderboard_stat_type::stat_double;
    if (type == _T("String"))
        return leaderboard_stat_type::stat_string;
    if (type == _T("DateTime"))
        return leaderboard_stat_type::stat_datetime;
    
    return leaderboard_stat_type::stat_other;
}

leaderboard_row
deserialize_row(
    _In_ const web::json::value& json,
    _In_ std::error_code& errc
    )
{
    string_t gamertag = utils::extract_json_string(json, _T("gamertag"), errc, true);
    string_t xuid = utils::extract_json_string(json, _T("xuid"), errc, true);
    double percentile = utils::extract_json_double(json, _T("percentile"), errc, true);
    int rank = utils::extract_json_int(json, _T("rank"), errc, true);
    std::vector<string_t> values;
    web::json::object jsonObject = json.as_object();
    if(!jsonObject[_T("value")].is_null())
    {
        values.push_back(utils::extract_json_string(json, _T("value"), errc, true));
    }
    else
    {
        values = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("values"), errc, true);
    }
    auto metadata = utils::extract_json_string(json, _T("valuemetadata"), errc, false);

    return leaderboard_row(
        std::move(gamertag),
        std::move(xuid),
        percentile,
        rank,
        values,
        metadata
        );
}

leaderboard_column
deserialize_column(
    _In_ const web::json::value& json,
    _In_ std::error_code& errc
    )
{
    string_t displayName = utils::extract_json_string(json, _T("displayName"), errc);
    string_t statName = utils::extract_json_string(json, _T("statName"), errc, true);
    string_t stat_type = utils::extract_json_string(json, _T("type"), errc, true);

    return leaderboard_column(
        std::move(displayName),
        std::move(statName),
        parse_stat_type(stat_type)
        );
}

xbox_live_result<leaderboard_result>
deserialize_result(
    _In_ const web::json::value& json,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ const string_t& version,
    _In_ leaderboard_query query
    )
{
    std::error_code errc;
    web::json::value lb_info = utils::extract_json_field(json, _T("leaderboardInfo"), errc, true);
    string_t displayName = utils::extract_json_string(lb_info, _T("displayName"), errc);
    int totalCount = utils::extract_json_int(lb_info, _T("totalCount"), errc, true);

    web::json::value paging_info = utils::extract_json_field(json, _T("pagingInfo"), errc, false);
    string_t continuationToken;
    if (!paging_info.is_null())
    {
        continuationToken = utils::extract_json_string(paging_info, _T("continuationToken"), errc, false);
    }

    std::vector<leaderboard_column> columns;
    if (version == _T("2017"))
    {
        web::json::array json_columns = utils::extract_json_as_array(
            utils::extract_json_field(lb_info, _T("columns"), errc, false),
            errc);
        for (const auto& json_column : json_columns)
        {
            columns.push_back(deserialize_column(json_column, errc));
        }
    }
    else
    {
        web::json::value json_column =
            utils::extract_json_field(lb_info, _T("columnDefinition"), errc, true);

        columns.push_back(deserialize_column(json_column, errc));
    }

    std::vector<leaderboard_row> rows;
    web::json::array json_rows = 
        utils::extract_json_as_array(
            utils::extract_json_field(json, _T("userList"), errc, true),
            errc
            );

    for (const auto& row : json_rows)
    {
        rows.push_back(deserialize_row(row, errc));
    }

    auto result = leaderboard_result(
        displayName,
        totalCount,
        continuationToken,
        std::move(columns),
        std::move(rows),
        userContext, 
        xboxLiveContextSettings,
        appConfig
        );

    if (version == _T("2017"))
    {
        query._Set_continuation_token(continuationToken);
        result._Set_next_query(query);
    }

    return xbox_live_result<leaderboard_result>(result, errc);
}

}
NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END