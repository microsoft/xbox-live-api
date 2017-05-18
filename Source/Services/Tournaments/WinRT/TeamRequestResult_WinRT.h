// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "TeamInfo_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a collection of TeamInfo class objects returned by a request.
/// </summary>
public ref class TeamRequestResult sealed
{
public:

    /// <summary>
    /// A collection of TeamInfo objects returned by a request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<TeamInfo^>^ Teams
    { 
        Windows::Foundation::Collections::IVectorView<TeamInfo^>^ get();
    }

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of teams to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

    /// <summary>
    /// Returns an TeamRequestResult object that contains the next page of Tournament.
    /// </summary>
    /// <returns>An TeamRequestResult object that contains a list of tournament objects.</returns>
    /// <remarks>
    /// Returns a concurrency::task{T} object that represents the state of the asynchronous operation.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TeamRequestResult^>^ GetNextAsync();

internal:
    TeamRequestResult(
        _In_ xbox::services::tournaments::team_request_result cppObj
        );

    const xbox::services::tournaments::team_request_result& GetCppObj() const;
private:
    xbox::services::tournaments::team_request_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<TeamInfo^>^ m_teams;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
