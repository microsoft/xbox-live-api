// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class CatalogItemImage sealed
{
    //      {
    //         "ID":"b3e560ae-dd9c-4ac5-b41d-5ddee19a7944",
    //         "ResizeUrl":"http://images.xboxlive.com/image?url=etc",
    //         "Purposes":
    //         [
    //            "Box_Art"
    //         ],
    //         "Purpose":"Box_Art",
    //         "Height":1080,
    //         "Width":1080
    //      }

public:
    /// <summary>
    /// Returns the URI of the catalog item image
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// Returns the URI of the catalog item image
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(ResizeUrl, resize_url);

    /// <summary>
    /// A collection of purposes 
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Purposes 
    { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
    }

    /// <summary>
    // The default purpose of the catalog item image
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Purpose, purpose);

    /// <summary>
    // The height of the image
    /// </summary>
    DEFINE_PROP_GET_OBJ(Height, height, uint32);

    /// <summary>
    // The width of the image
    /// </summary>
    DEFINE_PROP_GET_OBJ(Width, width, uint32);

internal:
    CatalogItemImage(
        _In_ xbox::services::marketplace::catalog_item_image cppObj
        );

private:
    xbox::services::marketplace::catalog_item_image m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_purposes;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
