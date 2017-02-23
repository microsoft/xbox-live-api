// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "BrowseCatalogResult_WinRT.h"
#include "CatalogItem_WinRT.h"

using namespace xbox::services::marketplace;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

BrowseCatalogResult::BrowseCatalogResult(
    _In_ xbox::services::marketplace::browse_catalog_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<CatalogItem, catalog_item>(m_cppObj.items())->GetView();
}

Windows::Foundation::Collections::IVectorView<CatalogItem^>^ 
BrowseCatalogResult::Items::get()
{
    return m_items;
}

Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ 
BrowseCatalogResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox::services::xbox_live_result<browse_catalog_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new BrowseCatalogResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END