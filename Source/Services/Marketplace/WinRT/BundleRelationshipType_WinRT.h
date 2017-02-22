// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

/// <summary>Enumeration values that indicate the media item type</summary>
public enum class BundleRelationshipType
{
    /// <summary>Unknown media type.</summary>
    Unknown = xbox::services::marketplace::bundle_relationship_type::unknown,

    /// <summary>Find which bundles include this product</summary>
    BundlesWithProduct = xbox::services::marketplace::bundle_relationship_type::bundles_with_product,

    /// <summary>Find which products are included in a specified bundle</summary>
    ProductsInBundle = xbox::services::marketplace::bundle_relationship_type::products_in_bundle
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
