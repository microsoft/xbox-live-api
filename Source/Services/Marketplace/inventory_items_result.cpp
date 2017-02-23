// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

inventory_items_result::inventory_items_result() :
    m_totalItems(0),
    m_allUsersAuthRequired(false),
    m_mediaItemType(media_item_type::unknown),
    m_inventoryItemState(inventory_item_state::unknown),
    m_inventoryItemAvailability(inventory_item_availability::unavailable),
    m_expandSatisfyingEntitlements(false)
{
}

inventory_items_result::inventory_items_result(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ media_item_type mediaItemType,
    _In_ inventory_item_state inventoryItemState,
    _In_ inventory_item_availability inventoryItemAvailability,
    _In_ string_t inventoryItemContainerId,
    _In_ bool allUsersAuthRequired,
    _In_ std::vector<inventory_item> items,
    _In_ uint32_t totalItems,
    _In_ string_t continuationToken,
    _In_ bool expandSatisfyingEntitlements
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_mediaItemType(mediaItemType),
    m_inventoryItemState(inventoryItemState),
    m_inventoryItemAvailability(inventoryItemAvailability),
    m_inventoryItemContainerId(std::move(inventoryItemContainerId)),
    m_allUsersAuthRequired(allUsersAuthRequired),
    m_items(std::move(items)),
    m_totalItems(totalItems),
    m_continuationToken(std::move(continuationToken)),
    m_expandSatisfyingEntitlements(expandSatisfyingEntitlements)
{
}

const std::vector<inventory_item>& inventory_items_result::items() const
{
    return m_items;
}

uint32_t inventory_items_result::total_items() const
{
    return m_totalItems;
}

const string_t& inventory_items_result::continuation_token() const
{
    return m_continuationToken;
}
pplx::task<xbox_live_result<inventory_items_result>> 
inventory_items_result::get_next(
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(m_continuationToken.empty(), inventory_items_result, "Continuation token is empty");

    inventory_service service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    return service.get_inventory_items(
        m_mediaItemType,
        m_inventoryItemState,
        m_inventoryItemAvailability,
        m_inventoryItemContainerId,
        m_allUsersAuthRequired,
        maxItems,
        m_continuationToken,
        m_expandSatisfyingEntitlements,
        std::vector<string_t>()
        );
}

bool
inventory_items_result::has_next() const
{
    return !m_continuationToken.empty();
}

void inventory_items_result::_Initialize(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ media_item_type mediaItemType,
    _In_ inventory_item_state inventoryItemState,
    _In_ inventory_item_availability inventoryItemAvailability,
    _In_ string_t inventoryItemContainerId,
    _In_ bool allUsersAuthRequired,
    _In_ bool expandSatisfyingEntitlements
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = std::move(appConfig);
    m_mediaItemType = mediaItemType;
    m_inventoryItemState = inventoryItemState;
    m_inventoryItemAvailability = inventoryItemAvailability;
    m_inventoryItemContainerId = std::move(inventoryItemContainerId);
    m_allUsersAuthRequired = allUsersAuthRequired;
    m_expandSatisfyingEntitlements = expandSatisfyingEntitlements;
}

xbox_live_result<inventory_items_result>
inventory_items_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<inventory_items_result>();

    std::error_code errc;
    inventory_items_result result;
    result.m_items = utils::extract_json_vector<inventory_item>(inventory_item::_Deserialize, json, _T("items"), errc, true);
    web::json::value pagingInfoJson = utils::extract_json_field(json, _T("pagingInfo"), errc, false);
    if (!pagingInfoJson.is_null())
    {
        result.m_continuationToken = utils::extract_json_string(pagingInfoJson, _T("continuationToken"), errc, false);
        result.m_totalItems = utils::extract_json_int(pagingInfoJson, _T("totalItems"), errc, true);
    }

    return xbox_live_result<inventory_items_result>(result, errc);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END