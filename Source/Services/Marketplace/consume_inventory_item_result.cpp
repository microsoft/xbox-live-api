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

consume_inventory_item_result::consume_inventory_item_result() :
    m_consumableBalance(0)
{
}

consume_inventory_item_result::consume_inventory_item_result(
    _In_ web::uri consumableUrl,
    _In_ uint32_t consumableBalance,
    _In_ string_t transactionId
    ) :
    m_consumableUrl(std::move(consumableUrl)),
    m_consumableBalance(consumableBalance),
    m_transactionId(std::move(transactionId))
{
}

const web::uri& consume_inventory_item_result::consumable_url() const
{
    return m_consumableUrl;
}

uint32_t consume_inventory_item_result::consumable_balance() const
{
    return m_consumableBalance;
}

const string_t& consume_inventory_item_result::transaction_id() const
{
    return m_transactionId;
}

xbox_live_result<consume_inventory_item_result>
consume_inventory_item_result::_Deserialize(
_In_ const web::json::value& json
)
{
    if (json.is_null()) return xbox_live_result<consume_inventory_item_result>();

    std::error_code errc;
    consume_inventory_item_result result;
    result.m_consumableUrl = utils::extract_json_string(json, _T("url"), errc, true);
    result.m_consumableBalance = utils::extract_json_int(json, _T("newQuantity"), errc, true);
    result.m_transactionId = utils::extract_json_string(json, _T("transactionId"), errc, true);

    return xbox_live_result<consume_inventory_item_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END