// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "XboxSocialRelationship_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN
/// <summary>
/// Contains a collection of social relationships the user has with other Xbox Live users.
/// </summary>

public ref class XboxSocialRelationshipResult sealed
{
public:
    /// <summary>
    /// Collection of XboxSocialRelationship objects returned by a request
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<XboxSocialRelationship^>^ Items { Windows::Foundation::Collections::IVectorView<XboxSocialRelationship^>^ get(); }

    /// <summary>
    /// The total number of XboxSocialRelationship objects that can be requested
    /// </summary>
    DEFINE_PROP_GET_OBJ(TotalCount, total_count, uint32);

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of social relationships to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

    /// <summary>
    /// Returns an XboxSocialRelationshipResult object containing the next page
    /// </summary>
    /// <param name="maxItems">The maximum number of items the response can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>Returns a XboxSocialRelationshipResult object</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people</remarks>
    Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

internal:
    XboxSocialRelationshipResult(
        _In_ xbox::services::social::xbox_social_relationship_result cppObj
        );

private:
    xbox::services::social::xbox_social_relationship_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<XboxSocialRelationship^>^ m_itemView;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END
