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
#include "xsapi/tournaments.h"
#include "Tournament_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a collection of tournament class objects returned by a request.
/// </summary>
public ref class TournamentRequestResult sealed
{
public:

    /// <summary>
    /// The collection of tournament objects returned by a request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Tournament^>^ Items
    { 
        Windows::Foundation::Collections::IVectorView<Tournament^>^ get();
    }

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of Tournaments to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

    /// <summary>
    /// Returns an TournamentRequestResult object that contains the next page of Tournament.
    /// </summary>
    /// <returns>An TournamentRequestResult object that contains a list of tournament objects.</returns>
    /// <remarks>
    /// Returns a concurrency::task<T> object that represents the state of the asynchronous operation.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TournamentRequestResult^>^ GetNextAsync();

internal:
    TournamentRequestResult(
        _In_ xbox::services::tournaments::tournament_request_result cppObj
        );

    const xbox::services::tournaments::tournament_request_result& GetCppObj() const;
private:
    xbox::services::tournaments::tournament_request_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<Tournament^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
