//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

inventory_item::inventory_item() :
    m_inventoryItemState(inventory_item_state::unknown),
    m_itemType(media_item_type::unknown),
    m_consumableBalance(0),
    m_titleId(0),
    m_isTrialEntitlement(false)
{
}

inventory_item::inventory_item(
    _In_ web::uri url,
    _In_ inventory_item_state inventoryItemState,
    _In_ media_item_type mediaItemType,
    _In_ string_t productId,
    _In_ uint32_t titleId,
    _In_ std::vector<string_t> containerIds,
    _In_ utility::datetime rightsObtainedDate,
    _In_ utility::datetime startDate,
    _In_ utility::datetime endDate,
    _In_ web::uri consumableUrl,
    _In_ uint32_t consumableBalance,
    _In_ bool isTrialEntitlement,
    _In_ std::chrono::seconds trialTimeRemaining
    ) :
    m_url(std::move(url)),
    m_inventoryItemState(inventoryItemState),
    m_itemType(mediaItemType),
    m_productId(std::move(productId)),
    m_titleId(titleId),
    m_containerIds(std::move(containerIds)),
    m_rightsObtainedDate(std::move(rightsObtainedDate)),
    m_startDate(std::move(startDate)),
    m_endDate(std::move(endDate)),
    m_consumableUrl(std::move(consumableUrl)),
    m_consumableBalance(consumableBalance),
    m_isTrialEntitlement(isTrialEntitlement),
    m_trialTimeRemaining(std::move(trialTimeRemaining))
{
}

const web::uri& inventory_item::url() const
{
    return m_url;
}

inventory_item_state inventory_item::item_state() const
{
    return m_inventoryItemState;
}

media_item_type inventory_item::item_type() const
{
    return m_itemType;
}

const string_t& inventory_item::product_id() const
{
    return m_productId;
}

uint32_t inventory_item::title_id() const
{
    return m_titleId;
}

const std::vector<string_t>& inventory_item::container_ids() const
{
    return m_containerIds;
}

const utility::datetime& inventory_item::rights_obtained_date() const
{
    return m_rightsObtainedDate;
}

const utility::datetime& inventory_item::start_date() const
{
    return m_startDate;
}

const utility::datetime& inventory_item::end_date() const
{
    return m_endDate;
}

const web::uri& inventory_item::consumable_url() const
{
    return m_consumableUrl;
}

uint32_t inventory_item::consumable_balance() const
{
    return m_consumableBalance;
}

bool inventory_item::is_trial_entitlement() const
{
    return m_isTrialEntitlement;
}

const std::chrono::seconds& inventory_item::trial_time_remaining() const
{
    return m_trialTimeRemaining;
}

xbox_live_result<inventory_item>
inventory_item::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<inventory_item>();

    std::error_code errc;
    inventory_item result;
    result.m_url = utils::extract_json_string(json, _T("url"), errc, true);
    result.m_inventoryItemState = convert_string_to_inventory_item_state(utils::extract_json_string(json, _T("state"), errc, true));
    result.m_itemType = convert_string_to_media_item_type(utils::extract_json_string(json, _T("itemType"), errc, true));
    result.m_productId = utils::extract_json_string(json, _T("productId"), errc, true);
    result.m_titleId = utils::extract_json_int(json, _T("titleId"), errc, false);
    result.m_containerIds = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("containers"), errc, false);
    result.m_rightsObtainedDate = utils::extract_json_time(json, _T("obtained"), errc, true);
    result.m_startDate = utils::extract_json_time(json, _T("beginDate"), errc, true);
    result.m_endDate = utils::extract_json_time(json, _T("endDate"), errc, true);
    result.m_isTrialEntitlement = utils::extract_json_bool(json, _T("trial"), errc, false);
    result.m_trialTimeRemaining = utils::extract_json_string_timespan_in_seconds(json, _T("trialTimeRemaining"), errc, false);

    web::json::value consumableJson = utils::extract_json_field(json, _T("consumable"), errc, false);
    if (!consumableJson.is_null())
    {
        result.m_consumableUrl = utils::extract_json_string(consumableJson, _T("url"), errc, true);
        result.m_consumableBalance = utils::extract_json_int(consumableJson, _T("quantity"), errc, true);
    }

    return xbox_live_result<inventory_item>(result);
}

inventory_item_state 
inventory_item::convert_string_to_inventory_item_state(
    _In_ const string_t& itemType
    )
{
    if (itemType.empty())
    {
        return inventory_item_state::unknown;
    }
    else if (utils::str_icmp(itemType, _T("Enabled")) == 0)
    {
        return inventory_item_state::enabled;
    }
    else if (utils::str_icmp(itemType, _T("Suspended")) == 0)
    {
        return inventory_item_state::suspended;
    }
    else if (utils::str_icmp(itemType, _T("Expired")) == 0)
    {
        return inventory_item_state::expired;
    }
    else if (utils::str_icmp(itemType, _T("Canceled")) == 0)
    {
        return inventory_item_state::canceled;
    }

    return inventory_item_state::unknown;
}

media_item_type 
inventory_item::convert_string_to_media_item_type(
    _In_ const string_t& itemType
    )
{
    if (itemType.empty())
    {
        return media_item_type::unknown;
    }
    else if (utils::str_icmp(itemType, _T("GameV2")) == 0)
    {
        return media_item_type::game_v2;
    }
    else if (utils::str_icmp(itemType, _T("GameContent")) == 0)
    {
        return media_item_type::game_content;
    }
    else if (utils::str_icmp(itemType, _T("GameConsumable")) == 0)
    {
        return media_item_type::game_consumable;
    }
    else if (utils::str_icmp(itemType, _T("Subscription")) == 0)
    {
        return media_item_type::subscription;
    }

    return media_item_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END