// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/leaderboard.h"
#include "Macros_WinRT.h"
#include "LeaderboardColumn.h"
#include "LeaderboardRow.h"
#include "../../Stats/Manager/WinRT/LeaderboardQuery_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
public ref class LeaderboardResult sealed
{
public:
    
    /// <summary>
    /// DEPRECATED
    /// The display name of the leaderboard results.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayName, display_name)

    /// <summary>
    /// The total number of rows in the leaderboard results.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TotalRowCount, total_row_count, uint32);

    /// <summary>
    /// The collection of columns in the leaderboard results.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<LeaderboardColumn^>^ Columns
    {
        Windows::Foundation::Collections::IVectorView<LeaderboardColumn^>^ get();
    }

    /// <summary>
    /// The collection of rows in the leaderboard results.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<LeaderboardRow^>^ Rows
    {
        Windows::Foundation::Collections::IVectorView<LeaderboardRow^>^ get();
    }

    /// <summary>
    /// Indicates whether there is a next page of results.
    /// </summary>
    /// <returns>True if there is another page of results; otherwise false.</returns>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

    /// <summary>
    /// Get the next page of a previous leaderboard call using the same service config Id and leaderboard name.
    /// </summary>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>A LeaderboardResults object that contains the next set of results.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// Calls V1 GET 
    ///  /scids/{scid}/leaderboards/{leaderboardname}?
    ///  [maxItems={maxItems}]
    ///  [continuationToken={token}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ GetNextAsync(_In_ uint32 maxItems);

    /// <summary>
    /// Gets a query to be used to retrieve more data about a leaderboard. 
    /// This query is only to be used to retrieve a leaderboard with stats 2017.
    /// </summary>
    LeaderboardQuery^ GetNextQuery();


internal:
    LeaderboardResult(
        _In_ xbox::services::leaderboard::leaderboard_result cppObj
        );

private:
    xbox::services::leaderboard::leaderboard_result m_cppObj;
    Windows::Foundation::Collections::IVector<LeaderboardColumn^>^ m_columns;
    Windows::Foundation::Collections::IVector<LeaderboardRow^>^ m_rows;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END