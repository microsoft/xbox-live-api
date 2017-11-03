// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "Club_WinRT.h"
#include "ClubType_WinRT.h"
#include "ClubsOwnedResult_WinRT.h"
#include "ClubsSearchResult_WinRT.h"
#include "ClubSearchAutoComplete_WinRT.h"
#include "ClubRecommendation_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

/// <summary>
/// Represents the leaderboard service.
/// </summary>
public ref class ClubsService sealed
{
public:
    /// <summary>Get information about a club.</summary>
    /// <param name="clubId">The club ID of the club.</param>
    /// <returns>A club object describing the club.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids({clubId})/decoration/settings
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Club^>^ GetClubAsync(
        _In_ Platform::String^ clubId
        );

    /// <summary>Get information about multiple clubs.</summary>
    /// <param name="clubIds">Vector of club Ids to get info about.</param>
    /// <returns>A vector of club objects describing the clubs.</returns>
    /// <remarks>
    /// Calls V4 POST clubhub.xboxlive.com/clubs/Batch
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Club^>^>^ GetClubsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ clubIds
        );

    /// <summary>Get information about the clubs the caller owns.</summary>
    /// <returns>A ClubsOwnedResult containing info about the clubs owned</returns>
    /// <remarks>
    /// Calls V1 GET clubaccounts.xboxlive.com/users/{userId}/clubsowned
    /// </remarks>
    Windows::Foundation::IAsyncOperation<ClubsOwnedResult^>^ GetClubsOwnedAsync();

    /// <summary>Creates a club.</summary>
    /// <param name="name">Name of the club to be created</param>
    /// <param name="type">Type of the club to be created</param>
    /// <param name="titleFamilyId">Information about the title the club is associated with</param>
    /// <returns>A Club object describing the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubaccounts.xboxlive.com/clubs/create
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Club^>^ CreateClubAsync(
        _In_ Platform::String^ name,
        _In_ ClubType type,
        _In_ Platform::String^ titleFamilyId
        );

    /// <summary>Deletes a club.</summary>
    /// <param name="clubId">Club Id of the club to delete</param>
    /// <returns>A Club object describing the deleted club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubaccounts.xboxlive.com/clubs/{clubId}
    /// </remarks>
    Windows::Foundation::IAsyncAction^ DeleteClubAsync(
        _In_ Platform::String^ clubId
        );

    /// <summary>Returns details about all the clubs the caller is associated with.</summary>
    /// <returns>An IVectorView of all clubs the caller is associated with in any way.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Club^>^>^ GetClubAssociationsAsync();

    /// <summary>Returns details about all the clubs a particular user is associated with.</summary>
    /// <param name="xuid">Xuid of the user</param>
    /// <returns>An IVectorView of all clubs the user is associated with in any way.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Club^>^>^ GetClubAssociationsAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary>Get recommended clubs for the caller.</summary>
    /// <returns>
    /// An IVectorView of ClubRecommendation. A ClubRecommendation contains the recommended club and an IVectorView
    /// of localized "reason" strings describing why the club was recommended.
    /// </returns>
    /// <remarks>
    /// Calls V4 POST clubhub.xboxlive.com/clubs/recommendations
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRecommendation^>^>^ GetClubRecommendationsAsync();

    /// <summary>Search for clubs with a query string.</summary>
    /// <param name="queryString">The query string used to search.</param>
    /// <param name="tags">Tags used to filter the search results.</param>
    /// <param name="titleIds">Title Ids used to filter the search results.</param>
    /// <returns>A ClubsSearchResult containing the results of the search.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/search
    /// </remarks>
    Windows::Foundation::IAsyncOperation<ClubsSearchResult^>^ SearchClubsAsync(
        _In_ Platform::String^ queryString,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ titleIds,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ tags
        );

    /// <summary>Gets auto complete search suggestions for a club search query given a partial search string</summary>
    /// <param name="query">The partial query string used to create suggestions.</param>
    /// <param name="tags">Tags used to filter the results.</param>
    /// <param name="titleIds">Title Ids used to filter the results.</param>
    /// <returns>A vector containing auto complete suggestions.</returns>
    /// <remarks>
    /// Calls V1 GET clubsearch.xboxlive.com/suggest
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubSearchAutoComplete^>^>^ SuggestClubsAsync(
        _In_ Platform::String^ queryString,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ titleIds,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ tags
        );

internal:
    ClubsService(
        _In_ xbox::services::clubs::clubs_service cppObj);

private:
    xbox::services::clubs::clubs_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END