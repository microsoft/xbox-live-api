// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/leaderboard.h"
#include "Macros_WinRT.h"
#include "LeaderboardColumn.h"
#include "LeaderboardRow.h"
#include "LeaderboardResult.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary>
/// Represents the leaderboard service.
/// </summary>
public ref class LeaderboardService sealed
{
public:

    /// <summary>
    /// Get a leaderboard for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName
        );

    /// <summary>
    /// Get a page of leaderboard results for a single leaderboard given a service configuration ID
    /// and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to retrieve.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipItems={skipItems}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ uint32 skipToRank,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard that shows members of a specified social group for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard that shows members of a specified social group for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ uint32 skipToRank,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard with additional columns for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns
        );

    /// <summary>
    /// Get a leaderboard that shows members of a specified social group with additional columns for a single 
    /// leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a page of leaderboard results with additional columns for a single leaderboard given a service configuration ID
    /// and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipItems={skipItems}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ uint32 skipToRank,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a page of leaderboard results with additional columns for a single leaderboard given a service configuration ID
    /// and a leaderboard name.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipItems={skipItems}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ uint32 skipToRank,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="skipToXboxUserId">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// There is no continuation Token provided for this query.
    ///
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipToUser={xuid}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithSkipToUserAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_opt_ Platform::String^ skipToXboxUserId,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToXboxUserId">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// There is no continuation Token provided for this query.
    ///
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipToUser={xuid}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithSkipToUserAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ Platform::String^ skipToXboxUserId,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToXboxUserId">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// There is no continuation Token provided for this query.
    ///
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipToUser={xuid}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ skipToXboxUserId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="leaderboardName">The name of the leaderboard.</param>
    /// <param name="skipToXboxUserId">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <param name="additionalColumns">The name of the stats for the additional columns.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// There is no continuation Token provided for this query.
    ///
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipToUser={xuid}][&amp;maxItems={maxItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ leaderboardName,
        _In_ Platform::String^ xuid,
        _In_ Platform::String^ socialGroup,
        _In_ Platform::String^ skipToXboxUserId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get an unsorted leaderboard that shows members of a specified social group.
    /// </summary>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="maxItems">The maximum number of items to retrieve.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V1 GET
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardForSocialGroupAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName,
        _In_ Platform::String^ socialGroup,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a sorted leaderboard that shows members of a specified social group.
    /// </summary>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardForSocialGroupAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName,
        _In_ Platform::String^ socialGroup,
        _In_ Platform::String^ sortOrder,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a sorted leaderboard, starting at a specified rank, that shows members of a specified social group.
    /// </summary>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve.</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// </returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToRank={skipToUser}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardForSocialGroupWithSkipToRankAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName,
        _In_ Platform::String^ socialGroup,
        _In_ uint32 skipToRank,
        _In_ Platform::String^ sortOrder,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get a sorted leaderboard, starting at a specified player, that shows members of a specified social group.
    /// </summary>
    /// <param name="xboxUserId">The Xbox user ID of the requesting user.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToXboxUserId">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve.</param>
    /// <returns>
    /// A LeaderboardResult object that contains a page of leaderboard results around the specified player regardless
    /// of that player's rank or score.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// There is no continuation Token provided for this query.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToUser={skipToUser}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetLeaderboardForSocialGroupWithSkipToUserAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName,
        _In_ Platform::String^ socialGroup,
        _In_opt_ Platform::String^ skipToXboxUserId,
        _In_ Platform::String^ sortOrder,
        _In_ uint32 maxItems
        );

internal:
    LeaderboardService(
        _In_ xbox::services::leaderboard::leaderboard_service cppObj);

private:
    xbox::services::leaderboard::leaderboard_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END