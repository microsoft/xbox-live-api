// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xsapi/marketplace.h"

using namespace pplx;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

const string_t::value_type c_inventoryContractVersionHeaderValue[] = _T("4");

inventory_service::inventory_service()
{
}

inventory_service::inventory_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<inventory_items_result>> 
inventory_service::get_inventory_items(
    _In_ media_item_type mediaItemType,
    _In_ bool expandSatisfyingEntitlements,
    _In_ bool includeAllItemStatesAndAvailabilities
    )
{
    auto inventoryItemState = inventory_item_state::enabled;
    auto inventoryItemAvailability = inventory_item_availability::available;
    if(includeAllItemStatesAndAvailabilities)
    {
        inventoryItemState = inventory_item_state::all;
        inventoryItemAvailability = inventory_item_availability::all;
    }

    return get_inventory_items(
        mediaItemType,
        inventoryItemState,
        inventoryItemAvailability,
        string_t(),
        false,
        0,
        string_t(),
        expandSatisfyingEntitlements,
        std::vector<string_t>()
        );
}

pplx::task<xbox_live_result<inventory_items_result>>
inventory_service::get_inventory_items(
    _In_ const std::vector<string_t>& productIds,
    _In_ bool expandSatisfyingEntitlements
    )
{
    return get_inventory_items(
        media_item_type::all,
        inventory_item_state::all,
        inventory_item_availability::all,
        string_t(),
        false,
        0,
        string_t(),
        expandSatisfyingEntitlements,
        productIds
        );
}

pplx::task<xbox_live_result<inventory_items_result>> 
inventory_service::get_inventory_items(
    _In_ media_item_type mediaItemType,
    _In_ inventory_item_state inventoryItemState,
    _In_ inventory_item_availability inventoryItemAvailability,
    _In_ const string_t& inventoryItemContainerId,
    _In_ uint32_t maxItems,
    _In_ bool expandSatisfyingEntitlements
    )
{
    return get_inventory_items(
        mediaItemType,
        inventoryItemState,
        inventoryItemAvailability,
        inventoryItemContainerId,
        false,
        maxItems,
        string_t(),
        expandSatisfyingEntitlements,
        std::vector<string_t>()
        );
}

pplx::task<xbox_live_result<inventory_items_result>> 
inventory_service::get_inventory_items_for_all_users(
    _In_ media_item_type mediaItemType,
    _In_ bool expandSatisfyingEntitlements,
    _In_ bool includeAllItemStatesAndAvailabilities
    )
{
    auto inventoryItemState = inventory_item_state::enabled;
    auto inventoryItemAvailability = inventory_item_availability::available;
    if(includeAllItemStatesAndAvailabilities)
    {
        inventoryItemState = inventory_item_state::all;
        inventoryItemAvailability = inventory_item_availability::all;
    }

    return get_inventory_items(
        mediaItemType,
        inventoryItemState,
        inventoryItemAvailability,
        string_t(),
        true,
        0,
        string_t(),
        expandSatisfyingEntitlements,
        std::vector<string_t>()
        );
}

pplx::task<xbox_live_result<inventory_items_result>>
inventory_service::get_inventory_items_for_all_users(
    _In_ const std::vector<string_t>& productIds,
    _In_ bool expandSatisfyingEntitlements
    )
{
    return get_inventory_items(
        media_item_type::all,
        inventory_item_state::all,
        inventory_item_availability::all,
        string_t(),
        true,
        0,
        string_t(),
        expandSatisfyingEntitlements,
        productIds
        );
}

pplx::task<xbox_live_result<inventory_items_result>> 
inventory_service::get_inventory_items_for_all_users(
    _In_ media_item_type mediaItemType,
    _In_ inventory_item_state inventoryItemState,
    _In_ inventory_item_availability inventoryItemAvailability,
    _In_ const string_t& inventoryItemContainerId,
    _In_ uint32_t maxItems,
    _In_ bool expandSatisfyingEntitlements
    )
{
    return get_inventory_items(
        mediaItemType,
        inventoryItemState,
        inventoryItemAvailability,
        inventoryItemContainerId,
        true,
        maxItems,
        string_t(),
        expandSatisfyingEntitlements,
        std::vector<string_t>()
        );
}

pplx::task<xbox_live_result<inventory_item>>
inventory_service::get_inventory_item(
    _In_ inventory_item inventoryItem
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(inventoryItem.url().is_empty(), inventory_item, "inventoryItem url cannot be empty");

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        inventoryItem.url().to_string(),
        nullptr,
        xbox_live_api::get_inventory_item
        );

    httpCall->set_xbox_contract_version_header_value(c_inventoryContractVersionHeaderValue);

    return httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto result = inventory_item::_Deserialize(response->response_body_json());
        return utils::generate_xbox_live_result(
            result,
            response
            );
    });
}

pplx::task<xbox_live_result<consume_inventory_item_result>> 
inventory_service::consume_inventory_item(
    _In_ inventory_item inventoryItem,
    _In_ uint32_t quantityToConsume,
    _In_ const string_t& transactionId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(inventoryItem.consumable_url().to_string().empty(), consume_inventory_item_result, "inventoryItems consumableUrl cannot be empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(quantityToConsume == 0, consume_inventory_item_result, "quantityToConsume");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(transactionId, consume_inventory_item_result, "transactionId cannot be empty");

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        inventoryItem.consumable_url().to_string(),
        _T(""),
        xbox_live_api::consume_inventory_item
        );
    consume_inventory_item_request request(
        transactionId,
        quantityToConsume
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_request_body(request._Serialize().serialize());
    httpCall->set_xbox_contract_version_header_value(c_inventoryContractVersionHeaderValue);

    return httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto result = consume_inventory_item_result::_Deserialize(response->response_body_json());
        return utils::generate_xbox_live_result<consume_inventory_item_result>(
            result,
            response
            );
    });
}

pplx::task<xbox_live_result<inventory_items_result>> 
inventory_service::get_inventory_items(
    _In_ media_item_type mediaItemType,
    _In_ inventory_item_state inventoryItemState,
    _In_ inventory_item_availability inventoryItemAvailability,
    _In_ const string_t& inventoryItemContainerId,
    _In_ bool allUsersAuthRequired,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ bool expandSatisfyingEntitlements,
    _In_ const std::vector<string_t>& productIds
    )
{
    auto mediaItemTypeResult = convert_media_item_type_to_string(mediaItemType);
    RETURN_TASK_CPP_IF_ERR(mediaItemTypeResult, inventory_items_result);
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(productIds.size() == 0 && mediaItemType == media_item_type::all, inventory_items_result, "mediaItemType::All can only be used when passing a non-zero list of ProductIds");

    auto inventoryItemStateResult = convert_inventory_item_state_to_string(inventoryItemState);
    RETURN_TASK_CPP_IF_ERR(inventoryItemStateResult, inventory_items_result);
    auto inventoryItemAvailabilityResult = convert_inventory_item_availability_to_string(inventoryItemAvailability);
    RETURN_TASK_CPP_IF_ERR(inventoryItemAvailabilityResult, inventory_items_result);

    auto subpathAndQuery = get_inventory_items_sub_path(
        mediaItemType,
        mediaItemTypeResult.payload(),
        inventoryItemStateResult.payload(),
        inventoryItemAvailabilityResult.payload(),
        inventoryItemContainerId,
        maxItems,
        continuationToken,
        expandSatisfyingEntitlements,
        productIds
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("inventory"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::get_inventory_items
        );

    httpCall->set_xbox_contract_version_header_value(c_inventoryContractVersionHeaderValue);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    auto task = httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::json_body, allUsersAuthRequired)
    .then([userContext, xboxLiveContextSettings, appConfig, mediaItemType, inventoryItemState, inventoryItemAvailability, inventoryItemContainerId, allUsersAuthRequired, expandSatisfyingEntitlements](std::shared_ptr<http_call_response> response)
    {
        auto inventoryResult = inventory_items_result::_Deserialize(response->response_body_json());

        auto& result = inventoryResult.payload();
        // Initialize the request params for get_next()
        result._Initialize(
            userContext,
            xboxLiveContextSettings,
            appConfig,
            mediaItemType,
            inventoryItemState,
            inventoryItemAvailability,
            inventoryItemContainerId,
            allUsersAuthRequired,
            expandSatisfyingEntitlements
            );

        return utils::generate_xbox_live_result<inventory_items_result>(
            result,
            response
            );
    });

    return utils::create_exception_free_task<inventory_items_result>(
        task
        );
}

const string_t inventory_service::get_inventory_items_sub_path(
    _In_ media_item_type mediaItemType,
    _In_ const string_t& mediaItemTypeString,
    _In_ const string_t& inventoryItemState,
    _In_ const string_t& inventoryItemAvailability,
    _In_ const string_t& inventoryItemContainerId,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ bool expandSatisfyingEntitlements,
    _In_ const std::vector<string_t>& productIds
    )
{
    stringstream_t source;
    source << _T("/users/me/inventory");

    web::uri_builder builder;
    builder.set_path(source.str());
    std::vector<string_t> params;

    if(mediaItemType != media_item_type::all)
    {
        builder.append_query(_T("itemType"), mediaItemTypeString);
    }

    if (!inventoryItemState.empty())
    {
        builder.append_query(_T("state"), inventoryItemState);
    }

    if (!inventoryItemContainerId.empty())
    {
        builder.append_query(_T("container"), inventoryItemContainerId);
    }

    if (!inventoryItemAvailability.empty())
    {
        builder.append_query(_T("availability"), inventoryItemAvailability);
    }
    
    if(expandSatisfyingEntitlements)
    {
        builder.append_query(_T("expandSatisfyingEntitlements"), _T("true"));
    }

    if (!productIds.empty())
    {
        stringstream_t param;
        uint32_t addedIds = 0;
        for (auto& productId : productIds)
        {
            if ( addedIds != 0 )
            {
                param << _T(",");
            }

            param << web::uri::encode_uri(productId);
            ++addedIds;
        }
        builder.append_query(_T("productIds"), param.str());
    }

    utils::append_paging_info(
        builder,
        0,
        maxItems,
        continuationToken
    );

    return builder.to_string();
}

const xbox_live_result<string_t>
inventory_service::convert_media_item_type_to_string(
    _In_ media_item_type mediaItemType
    )
{
    switch (mediaItemType)
    {
    case media_item_type::game_v2: return xbox_live_result<string_t>(_T("GameV2"));

    case media_item_type::game_content: return xbox_live_result<string_t>(_T("GameContent"));

    case media_item_type::game_consumable: return xbox_live_result<string_t>(_T("GameConsumable"));

    case media_item_type::subscription: return xbox_live_result<string_t>(_T("Subscription"));

    case media_item_type::all: return xbox_live_result<string_t>(_T("All"));

    default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported media_item_type type");
    }
}

const xbox_live_result<string_t>
inventory_service::convert_inventory_item_state_to_string(
    _In_ inventory_item_state inventorytItemState
    )
{
    switch (inventorytItemState)
    {
    case inventory_item_state::all:
        return xbox_live_result<string_t>();

    case inventory_item_state::enabled:
        return xbox_live_result<string_t>(_T("Enabled"));

    case inventory_item_state::suspended:
        return xbox_live_result<string_t>(_T("Suspended"));

    case inventory_item_state::expired:
        return xbox_live_result<string_t>(_T("Expired"));

    case inventory_item_state::canceled:
        return xbox_live_result<string_t>(_T("Canceled"));

    default:
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported inventory_item_state type");
    }
}

const xbox_live_result<string_t>
inventory_service::convert_inventory_item_availability_to_string(
    _In_ inventory_item_availability inventorytItemAvailability
    )
{
    switch (inventorytItemAvailability)
    {
    case inventory_item_availability::all:
        return xbox_live_result<string_t>(_T("All"));

    case inventory_item_availability::available:
        return xbox_live_result<string_t>(_T("Available"));

    case inventory_item_availability::unavailable:
        return xbox_live_result<string_t>(_T("Unavailable"));

    default:
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported inventory_item_availability type");
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END