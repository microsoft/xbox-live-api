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

#include "Achievement_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents a collection of Achievement class objects returned by a request.
/// </summary>
public ref class AchievementsResult sealed
{
public:
    /// <summary>
    /// The collection of Achievement objects returned by a request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Achievement^>^ Items { Windows::Foundation::Collections::IVectorView<Achievement^>^ get(); }

    /// <summary>
    /// Returns an AchievementsResult object that contains the next page of achievements.
    /// </summary>
    /// <param name="maxItems">The maximum number of items that the result can contain.  Pass 0 to attempt
    /// to retrieve all items.</param>
    /// <returns>An AchievementsResult object that contains a list of Achievement objects.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<AchievementsResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of achievements to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

internal:
    AchievementsResult(
        _In_ xbox::services::achievements::achievements_result cppObj
        );

private:
    xbox::services::achievements::achievements_result m_cppObj;
    Windows::Foundation::Collections::IVector<Achievement^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END