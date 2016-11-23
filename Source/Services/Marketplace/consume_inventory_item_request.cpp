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

consume_inventory_item_request::consume_inventory_item_request()
{
}

consume_inventory_item_request::consume_inventory_item_request(
    _In_ string_t transactionId,
    _In_ uint32_t removeQuantity
    ) :
    m_transactionId(std::move(transactionId)),
    m_removeQuantity(removeQuantity)
{
}

xbox_live_result<consume_inventory_item_request>
consume_inventory_item_request::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<consume_inventory_item_request>();

    std::error_code errc;
    consume_inventory_item_request result;
    result.m_transactionId = utils::extract_json_string(json, _T("TransactionId"), errc, true);
    result.m_removeQuantity = utils::extract_json_int(json, _T("RemoveQuantity"), errc, true);

    return result;
}

web::json::value
consume_inventory_item_request::_Serialize()
{
    web::json::value serializedObject;
    serializedObject[_T("RemoveQuantity")] = web::json::value(m_removeQuantity);
    serializedObject[_T("TransactionId")] = web::json::value(m_transactionId);

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END