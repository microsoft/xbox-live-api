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

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// 
/// </summary>
public enum class TeamOrderBy
{
    /// <summary>
    /// No order provided.
    /// </summary>
    None = xbox::services::tournaments::team_order_by::none,

    /// <summary>
    /// Order by name.
    /// </summary>
    Name = xbox::services::tournaments::team_order_by::name,

    /// <summary>
    /// Order by ranking.
    /// </summary>
    Ranking = xbox::services::tournaments::team_order_by::ranking
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END