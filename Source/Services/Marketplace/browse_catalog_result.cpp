// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

const wchar_t c_browseCatalogContractVersionHeaderValue[] = L"3.2";

browse_catalog_result::browse_catalog_result() :
    m_parentMediaItemType(media_item_type::unknown),
    m_childMediaItemType(media_item_type::unknown),
    m_orderBy(catalog_sort_order::digital_release_date),
    m_skipItems(0),
    m_bundleRelationship(bundle_relationship_type::unknown),
    m_totalCount(0),
    m_isBundleRelated(false)
{
}

browse_catalog_result::browse_catalog_result(
    _In_ string_t parentId,
    _In_ media_item_type parentMediaItemType,
    _In_ media_item_type childMediaItemType,
    _In_ catalog_sort_order orderBy,
    _In_ uint32_t skipItems,
    _In_ string_t productId,
    _In_ bundle_relationship_type bundleRelationship,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::vector<catalog_item> items,
    _In_ uint32_t totalCount,
    _In_ string_t continuationToken,
    _In_ bool m_isBundleRelated
    ) :
    m_parentId(std::move(parentId)),
    m_parentMediaItemType(parentMediaItemType),
    m_childMediaItemType(childMediaItemType),
    m_orderBy(orderBy),
    m_skipItems(skipItems),
    m_productId(std::move(productId)),
    m_bundleRelationship(bundleRelationship),
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_items(std::move(items)),
    m_totalCount(totalCount),
    m_continuationToken(std::move(continuationToken)),
    m_isBundleRelated(m_isBundleRelated)
{
}

const std::vector<catalog_item>& browse_catalog_result::items() const
{
    return m_items;
}

uint32_t browse_catalog_result::total_count() const
{
    return m_totalCount;
}

bool
browse_catalog_result::has_next() const
{
    return m_skipItems < m_totalCount;
}

xbox_live_result<browse_catalog_result>
browse_catalog_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<browse_catalog_result>();

    std::error_code errc;
    browse_catalog_result result;
    result.m_items = utils::extract_json_vector<catalog_item>(catalog_item::_Deserialize, json, _T("Items"), errc, true);

    std::vector<catalog_total> catalogTotals = utils::extract_json_vector<catalog_total>(
        catalog_total::_Deserialize,
        json,
        _T("Totals"),
        errc,
        false
        );

    for (auto& catalogTotal : catalogTotals)
    {
        result.m_totalCount = result.m_totalCount > catalogTotal.total() ? result.m_totalCount : catalogTotal.total();
    }

    return xbox_live_result<browse_catalog_result>(result);
}

void browse_catalog_result::_Initialize(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ string_t parentId,
    _In_ media_item_type parentMediaType,
    _In_ media_item_type childItemType,
    _In_ catalog_sort_order orderBy,
    _In_ uint32_t skipItems
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_parentId = std::move(parentId);
    m_parentMediaItemType = parentMediaType;
    m_childMediaItemType = childItemType;
    m_orderBy = orderBy;
    m_skipItems = skipItems;
}

void browse_catalog_result::_Set_browse_bundles(
    _In_ string_t productId,
    _In_ bundle_relationship_type relationship
    )
{
    m_productId = std::move(productId);
    m_bundleRelationship = relationship;
    m_isBundleRelated = true;
}

pplx::task<xbox_live_result<browse_catalog_result>> 
browse_catalog_result::get_next(
    _In_ uint32_t maxItems
    )
{
    catalog_service service(m_userContext, m_xboxLiveContextSettings, m_appConfig);

    //  Is this a bundle related call or a standard BrowseCall?
    if (m_isBundleRelated)
    {
        return service.browse_catalog_bundles(
            m_parentId,
            m_parentMediaItemType,
            m_productId,
            m_bundleRelationship,
            m_skipItems,
            maxItems
            );
    }

    //  Standard Browse Call
    return service.browse_catalog(
        m_parentId,
        m_parentMediaItemType,
        m_childMediaItemType,
        m_orderBy,
        m_skipItems,
        maxItems
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END