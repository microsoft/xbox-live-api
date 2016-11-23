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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public enum class CatalogSortOrder 
{
    /// <summary>
    /// Sort by count of free and paid purchases, for the most recent day
    /// </summary>
    FreeAndPaidCountDaily = xbox::services::marketplace::catalog_sort_order::free_and_paid_count_daily,

    /// <summary>
    /// Sort by count of only paid purchases, for all time.
    /// </summary>
    PaidCountAllTime = xbox::services::marketplace::catalog_sort_order::paid_count_all_time,

    /// <summary>
    /// Sort by count of only paid purchases, for the most recent day.
    /// </summary>
    PaidCountDaily = xbox::services::marketplace::catalog_sort_order::paid_count_daily,

    /// <summary>
    /// Sort by date available for download
    /// </summary>
    DigitalReleaseDate = xbox::services::marketplace::catalog_sort_order::digital_release_date,

    /// <summary>
    /// Sort by date available in stores, falling back to digital release date (if available)
    /// </summary>
    ReleaseDate = xbox::services::marketplace::catalog_sort_order::release_date,

    /// <summary>
    /// Sort by average user ratings
    /// </summary>
    UserRatings = xbox::services::marketplace::catalog_sort_order::user_ratings
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END