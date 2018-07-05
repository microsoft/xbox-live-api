// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "shared_macros.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN
namespace serializers {

leaderboard_row deserialize_row(_In_ const web::json::value& json, _In_ std::error_code& errc);

leaderboard_column deserialize_column(_In_ const web::json::value& json, _In_ std::error_code& errc);

xbox_live_result<std::shared_ptr<leaderboard_result_internal>> deserialize_result(
    _In_ const web::json::value& json,
    _In_ std::shared_ptr<leaderboard_service_impl> leaderboardService,
    _In_ leaderboard_version version
    );

}
NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END