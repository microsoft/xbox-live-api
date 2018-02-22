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
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary> 
/// The order to sort the leaderboard in.
/// </summary>
public enum class SortOrder
{
	/// <summary> 
	/// Sort the leaderboard by ascending values (lower values indicate better ranking).
	/// </summary>
    Ascending = xbox::services::leaderboard::sort_order::ascending,

	/// <summary> 
	/// Sort the leaderboard by descending values (higher values indicate better ranking).
	/// </summary>
    Descending = xbox::services::leaderboard::sort_order::descending
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END