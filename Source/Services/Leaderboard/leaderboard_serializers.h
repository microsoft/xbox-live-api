//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include "shared_macros.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN
namespace serializers {

leaderboard_row deserialize_row(_In_ const web::json::value& json, _In_ std::error_code& errc);

leaderboard_column deserialize_column(_In_ const web::json::value& json, _In_ std::error_code& errc);

xbox_live_result<leaderboard_result> deserialize_result(
    _In_ const web::json::value& json,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    );

}
NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END