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
#include "xsapi/leaderboard.h"
#include "SortOrder_WinRT.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
public ref class LeaderboardQuery sealed
{
public:

    /// <summary>
    /// Gets/Sets whether or not the resulting leaderboard will start with the 
    /// user that requested the leaderboard.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(SkipResultToMe, skip_result_to_me, bool);

    /// <summary>
    /// Gets/Sets Which rank the resulting leaderboard will start at
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(SkipResultToRank, skip_result_to_rank, uint32_t);

    /// <summary>
    /// Gets/Sets maximum items that the resulting leaderboard will contain
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(MaxItems, max_items, uint32_t);

    /// <summary>
    /// Gets/Sets sort order for the resulting leaderboard
    /// </summary>
    DEFINE_PROP_GETSET_ENUM_OBJ(Order, order, SortOrder, xbox::services::leaderboard::sort_order);

    /// <summary>
    /// Gets the stat name of the previous query. This property will only be set if its a query 
    /// gotten from get_next_query
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(StatName, stat_name);

    /// <summary>
    /// Gets the social group of the previous query. This property will only be set if its a query 
    /// gotten from get_next_query and the previous query was a social query
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SocialGroup, social_group);

    /// <summary>
    /// Gets the continuation token is there is one. If it is empty then 
    /// that means that there is no more data to get from a previous query or there is no previous query.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ContinuationToken, continuation_token);

internal:
    LeaderboardQuery(
        _In_ xbox::services::leaderboard::leaderboard_query cppObj
        );

    const xbox::services::leaderboard::leaderboard_query& GetCppObj() const;

private:
    xbox::services::leaderboard::leaderboard_query m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END