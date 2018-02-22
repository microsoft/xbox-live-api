// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;
using namespace xbox::services::system;
#if TV_API 
using namespace Windows::Storage::Streams;
using namespace Windows::Media::ContentRestrictions;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

const string_t catalog_service::BROWSE_CATALOG_CONTRACT_HEADER_VALUE = _T("3.2");

catalog_service::catalog_service()
{
}

catalog_service::catalog_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

xbox_live_result<string_t>
catalog_service::convert_sort_order_to_string(
    _In_ catalog_sort_order type
    )
{
    switch (type)
    {
    case catalog_sort_order::digital_release_date:
        return xbox_live_result<string_t>(_T("DigitalReleaseDate"));

    case catalog_sort_order::free_and_paid_count_daily:
        return xbox_live_result<string_t>(_T("FreeAndPaidCountDaily"));

    case catalog_sort_order::paid_count_all_time:
        return xbox_live_result<string_t>(_T("PaidCountAllTime"));

    case catalog_sort_order::paid_count_daily:
        return xbox_live_result<string_t>(_T("PaidCountDaily"));

    case catalog_sort_order::release_date:
        return xbox_live_result<string_t>(_T("ReleaseDate"));

    case catalog_sort_order::user_ratings:
        return xbox_live_result<string_t>(_T("UserRatings"));

    default:
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported catalog_sort_order type");
    }
}

string_t 
catalog_service::_Convert_media_item_type_to_string(
    _In_ media_item_type itemType
    )
{
    switch (itemType)
    {
    case media_item_type::game_content:
        return _T("DDurable");

    case media_item_type::game_consumable:
        return _T("DConsumable");

    case media_item_type::subscription:
        return _T("Subscription");

    case media_item_type::application:
        return _T("DApp");

    case media_item_type::game:
        return _T("DGame");

    case media_item_type::movie:
        return _T("Movie");

    case media_item_type::television_show:
        return _T("TVShow");

    case media_item_type::television_series:
        return _T("TVSeries");

    case media_item_type::television_season:
        return _T("TVSeason");

    case media_item_type::application_or_game_unknown:
        return _T("AppOrGameUnknown");

    case media_item_type::game_demo:
        return _T("DGameDemo");

    case media_item_type::web_video:
        return _T("WebVideo");

    default:
        throw std::invalid_argument("Unsupported media_item_type type");
        break;
    }
}

media_item_type 
catalog_service::_Convert_string_to_media_item_type(
    _In_ const string_t& itemType
    )
{
    if (itemType.empty())
    {
        return media_item_type::unknown;
    }
    else if (utils::str_icmp(itemType, _T("DDurable")) == 0)
    {
        return media_item_type::game_content;
    }
    else if (utils::str_icmp(itemType, _T("DConsumable")) == 0)
    {
        return media_item_type::game_consumable;
    }
    else if (utils::str_icmp(itemType, _T("Subscription")) == 0)
    {
        return media_item_type::subscription;
    }
    else if (utils::str_icmp(itemType, _T("DApp")) == 0)
    {
        return media_item_type::application;
    }
    else if (utils::str_icmp(itemType, _T("DGame")) == 0)
    {
        return media_item_type::game;
    }
    else if (utils::str_icmp(itemType, _T("Movie")) == 0)
    {
        return media_item_type::movie;
    }
    else if (utils::str_icmp(itemType, _T("TVShow")) == 0)
    {
        return media_item_type::television_show;
    }
    else if (utils::str_icmp(itemType, _T("TVSeries")) == 0)
    {
        return media_item_type::television_series;
    }
    else if (utils::str_icmp(itemType, _T("TVSeason")) == 0)
    {
        return media_item_type::television_season;
    }
    else if (utils::str_icmp(itemType, _T("AppOrGameUnknown")) == 0)
    {
        return media_item_type::application_or_game_unknown;
    }
    else if (utils::str_icmp(itemType, _T("DGameDemo")) == 0)
    {
        return media_item_type::game_demo;
    }
    else if (utils::str_icmp(itemType, _T("WebVideo")) == 0)
    {
        return media_item_type::web_video;
    }

    return media_item_type::unknown;
}

string_t 
catalog_service::convert_bundle_relationship_type_to_string(
    _In_ bundle_relationship_type type
    )
{
    switch (type)
    {
    case bundle_relationship_type::bundles_with_product:
        return _T("bundledWith");

    case bundle_relationship_type::products_in_bundle:
        return _T("bundledProducts");

    default:
        throw std::invalid_argument("Unsupported bundle_relationship_type type");
        break;
    }
}

pplx::task<xbox_live_result<browse_catalog_result>> 
catalog_service::browse_catalog(
    _In_ const string_t& parentId,
    _In_ media_item_type parentMediaType,
    _In_ media_item_type childMediaType,
    _In_ catalog_sort_order orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(parentId, browse_catalog_result, "parentId was empty");

#if TV_API
    auto contentRestrictions = ref new RatedContentRestrictions();
    auto asyncOp = contentRestrictions->GetBrowsePolicyAsync();

    auto userContextShared = m_userContext;
    auto httpContextShared = m_xboxLiveContextSettings;
    auto appConfigShared = m_appConfig;
    return create_task(asyncOp)
    .then([userContextShared, httpContextShared, appConfigShared, parentId, parentMediaType, childMediaType, orderBy, skipItems, maxItems](task<ContentRestrictionsBrowsePolicy^> browsePolicyTask)   
    {
        ContentRestrictionsBrowsePolicy^ browsePolicy;
        browsePolicy = browsePolicyTask.get();

        web::json::value jsonHeader;
        jsonHeader[_T("version")] = web::json::value::string(_T("2"));

        web::json::value jsonData;
        jsonData[_T("geographicRegion")] = web::json::value::string(browsePolicy->GeographicRegion->Data());
        if (browsePolicy->MaxBrowsableAgeRating != nullptr)
        {
            jsonData[_T("maxAgeRating")] = web::json::value::string(browsePolicy->MaxBrowsableAgeRating->Value.ToString()->Data());
        };
        if (browsePolicy->PreferredAgeRating != nullptr)
        {
            jsonData[_T("preferredAgeRating")] = web::json::value::string(browsePolicy->PreferredAgeRating->Value.ToString()->Data());
        }
        jsonHeader[_T("data")] = jsonData;

        utf8string utf8String = utility::conversions::utf16_to_utf8(jsonHeader.serialize());
        std::vector<unsigned char> input(utf8String.c_str(), utf8String.c_str() + utf8String.size());
        string_t base64ContentRestriction = utility::conversions::to_base64(input);

        return browse_catalog_helper(parentId, parentMediaType, childMediaType, orderBy, skipItems, maxItems, base64ContentRestriction, userContextShared, httpContextShared, appConfigShared);
    });
#else
    return browse_catalog_helper(parentId, parentMediaType, childMediaType, orderBy, skipItems, maxItems, string_t(), m_userContext, m_xboxLiveContextSettings, m_appConfig);
#endif
}

pplx::task<xbox_live_result<browse_catalog_result>> 
catalog_service::browse_catalog_bundles(
    _In_ const string_t& parentId,
    _In_ media_item_type parentMediaType,
    _In_ const string_t& productId,
    _In_ bundle_relationship_type relationship,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(parentId, browse_catalog_result, "parentId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(productId, browse_catalog_result, "productId is empty");

#if TV_API
    auto contentRestrictions = ref new RatedContentRestrictions();
    auto asyncOp = contentRestrictions->GetBrowsePolicyAsync();

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    return create_task(asyncOp)
    .then([parentId, parentMediaType, productId, relationship, skipItems, maxItems, userContext, xboxLiveContextSettings, appConfig](task<ContentRestrictionsBrowsePolicy^> browsePolicyTask)
    {
        ContentRestrictionsBrowsePolicy^ browsePolicy;
        browsePolicy = browsePolicyTask.get();
        
        web::json::value jsonHeader;
        jsonHeader[_T("version")] = web::json::value::string(_T("2"));

        web::json::value jsonData;
        jsonData[_T("geographicRegion")] = web::json::value::string(browsePolicy->GeographicRegion->Data());
        if (browsePolicy->MaxBrowsableAgeRating != nullptr)
        {
            jsonData[_T("maxAgeRating")] = web::json::value::string(browsePolicy->MaxBrowsableAgeRating->Value.ToString()->Data());
        };
        if (browsePolicy->PreferredAgeRating != nullptr)
        {
            jsonData[_T("preferredAgeRating")] = web::json::value::string(browsePolicy->PreferredAgeRating->Value.ToString()->Data());
        }
        jsonHeader[_T("data")] = jsonData;

        utf8string utf8String = utility::conversions::utf16_to_utf8(jsonHeader.serialize());
        std::vector<unsigned char> input(utf8String.c_str(), utf8String.c_str() + utf8String.size());
        string_t base64ContentRestriction = utility::conversions::to_base64(input);

        return browse_catalog_bundles_helper(parentId, parentMediaType, productId, relationship, skipItems, maxItems, base64ContentRestriction, userContext, xboxLiveContextSettings, appConfig);
    });
#else
    return browse_catalog_bundles_helper(parentId, parentMediaType, productId, relationship, skipItems, maxItems, string_t(), m_userContext, m_xboxLiveContextSettings, m_appConfig);
#endif
}

pplx::task<xbox_live_result<std::vector<catalog_item_details>>>
catalog_service::get_catalog_item_details(
    _In_ const std::vector<string_t>& productIds
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(productIds.size() == 0, std::vector<catalog_item_details>, "productIds cannot be empty");

    string_t subpathAndQuery = marketplace_catalog_details_subpath(productIds);

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("eds"), m_appConfig),
        std::move(subpathAndQuery),
        xbox_live_api::get_catalog_item_details
        );

    httpCall->set_xbox_contract_version_header_value(BROWSE_CATALOG_CONTRACT_HEADER_VALUE);
    httpCall->set_custom_header(_T("Accept"), _T("application/json"));
    httpCall->set_custom_header(_T("x-xbl-client-type"), _T("Console"));
    httpCall->set_custom_header(_T("x-xbl-client-version"), _T("0"));
    httpCall->set_custom_header(_T("x-xbl-device-type"), _T("XboxOne"));

    // Need to remove the ContentType for this call
    httpCall->set_content_type_header_value(_T(""));

    return httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc;
        auto result = utils::extract_json_vector<catalog_item_details>(
            catalog_item_details::_Deserialize,
            response->response_body_json(),
            _T("Items"),
            errc,
            true
            );

        auto catalogItemResult = xbox_live_result<std::vector<catalog_item_details>>(
            result,
            errc
            );

        return utils::generate_xbox_live_result<std::vector<catalog_item_details>>(
            catalogItemResult,
            response
            );
    });
}

string_t catalog_service::marketplace_browse_catalog_subpath(
    _In_ const string_t& parentId,
    _In_ const string_t& titleType,
    _In_ const string_t& desiredMediaItemTypes,
    _In_ const string_t& relationship,
    _In_ const string_t& orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    stringstream_t source;
    source << _T("/media/");
#if TV_API
    source << Windows::Xbox::ApplicationModel::Store::Configuration::MarketplaceId->Data();
#else
    source << utils::string_t_from_internal_string(utils::get_locales());
#endif
    source << _T("/browse");

    std::vector<string_t> params;
    params.push_back(_T("fields=all"));

    if (!parentId.empty())
    {
        stringstream_t param;
        param << _T("id=");
        param << web::uri::encode_uri(parentId);
        params.push_back(param.str());
    }

    if (!titleType.empty())
    {
        stringstream_t param;
        param << _T("mediaItemType=");
        param << web::uri::encode_uri(titleType);
        params.push_back(param.str());
    }

    if (!relationship.empty())
    {
        stringstream_t param;
        param << _T("relationship=");
        param << web::uri::encode_uri(relationship);
        params.push_back(param.str());
    }
    
    if (!orderBy.empty())
    {
        stringstream_t param;
        param << _T("orderBy=");
        param << web::uri::encode_uri(orderBy);
        params.push_back(param.str());
    }

    if (!desiredMediaItemTypes.empty())
    {
        stringstream_t param;
        param << _T("desiredMediaItemTypes=");
        param << web::uri::encode_uri(desiredMediaItemTypes);
        params.push_back(param.str());
    }

    if (maxItems > 0)
    {
        stringstream_t param;
        param << _T("maxItems=");
        param << maxItems;
        params.push_back(param.str());
    }

    // use skip items value if continuation token is empty
    if (skipItems > 0)
    {
        stringstream_t param;
        param << _T("skipItems=");
        param << skipItems;
        params.push_back(param.str());
    }

    source << utils::get_query_from_params(params);
    return source.str();
}

string_t 
catalog_service::marketplace_catalog_details_subpath(
    _In_ const std::vector<string_t>& productIds
    )
{
    stringstream_t source;
    source << _T("/media/");
#if TV_API
    source << Windows::Xbox::ApplicationModel::Store::Configuration::MarketplaceId->Data();
#else
    source << utils::string_t_from_internal_string(utils::get_locales());
#endif
    source << _T("/details");

    std::vector<string_t> params;
    params.push_back(_T("fields=all"));
    params.push_back(_T("desiredMediaItemTypes=Subscription.DGame.DGameDemo.DDurable.DConsumable.DApp"));
    
    {
        stringstream_t param;
        param << _T("ids=");
        auto &last = productIds.back();
        for (const string_t& productId : productIds)
        {
            param << productId;
            if (&productId != &last)
            {
                param << _T(".");
            }
        }
        params.push_back(param.str());
    }

    source << utils::get_query_from_params(params);
    return source.str();
}

pplx::task<xbox_live_result<browse_catalog_result>> 
catalog_service::browse_catalog_bundles_helper(
    _In_ const string_t& parentId,
    _In_ media_item_type parentMediaType,
    _In_ const string_t& productId,
    _In_ bundle_relationship_type relationship,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ string_t customHeader,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    )
{
    string_t subpathAndQuery = marketplace_browse_catalog_subpath(
        productId,
        _Convert_media_item_type_to_string(parentMediaType),
        _T("DGame.DDurable.DConsumable.DApp"),  // get all related product types in the bundle
        convert_bundle_relationship_type_to_string(relationship),
        convert_sort_order_to_string( catalog_sort_order::release_date ).payload(),  // bundles_with_product requires a sorting field
        skipItems,
        maxItems
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("eds"), appConfig),
        std::move(subpathAndQuery),
        xbox_live_api::browse_catalog_bundles_helper
        );

    httpCall->set_xbox_contract_version_header_value(BROWSE_CATALOG_CONTRACT_HEADER_VALUE);
    httpCall->set_custom_header(_T("Accept"), _T("application/json"));
    httpCall->set_custom_header(_T("x-xbl-client-type"), _T("Console"));
    httpCall->set_custom_header(_T("x-xbl-client-version"), _T("0"));
    httpCall->set_custom_header(_T("x-xbl-device-type"), _T("XboxOne"));

    // Need to remove the ContentType for this call
    httpCall->set_content_type_header_value(_T(""));
    if( !customHeader.empty() ) 
    {
        httpCall->set_custom_header(_T("x-xbl-contentRestrictions"), customHeader);
    }

    return httpCall->get_response_with_auth(userContext, http_call_response_body_type::json_body)
    .then([userContext, xboxLiveContextSettings, parentId, parentMediaType, productId, relationship, skipItems](std::shared_ptr<http_call_response> response)
    {
        auto catalogResult = browse_catalog_result::_Deserialize(response->response_body_json());
        auto& result = catalogResult.payload();
        uint32_t itemSize = static_cast<uint32_t>(result.items().size());
        if (itemSize > 0)
        {
            uint32_t continuationSkip = skipItems + itemSize;

            // Initialize the request params for get_next()
            result._Initialize(
                userContext,
                xboxLiveContextSettings,
                parentId,
                parentMediaType,
                media_item_type::unknown,
                catalog_sort_order::release_date,     //The Bundle Browse Calls do not support the sort by field, so this is just arbitrary
                continuationSkip
                );

            //Mark this as a bundle related call to use the correct API on a GetNextAsync call
            result._Set_browse_bundles(
                productId,
                relationship
                );
        }
        
        return utils::generate_xbox_live_result<browse_catalog_result>(
            catalogResult,
            response
            );
    });
}

pplx::task<xbox_live_result<browse_catalog_result>> 
catalog_service::browse_catalog_helper(
    _In_ const string_t& parentId,
    _In_ media_item_type parentMediaType,
    _In_ media_item_type childMediaType,
    _In_ catalog_sort_order orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ string_t customHeader,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    )
{
    auto sortOrderResult = convert_sort_order_to_string(orderBy);

    string_t subpathAndQuery = marketplace_browse_catalog_subpath(
        parentId,
        _Convert_media_item_type_to_string(parentMediaType),
        _Convert_media_item_type_to_string(childMediaType),
        string_t(),    // not a bundle search
        sortOrderResult.payload(),
        skipItems,
        maxItems
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("eds"), appConfig),
        std::move(subpathAndQuery),
        xbox_live_api::browse_catalog_helper
        );

    httpCall->set_xbox_contract_version_header_value(BROWSE_CATALOG_CONTRACT_HEADER_VALUE);
    httpCall->set_custom_header(_T("Accept"), _T("application/json"));
    httpCall->set_custom_header(_T("x-xbl-client-type"), _T("Console"));
    httpCall->set_custom_header(_T("x-xbl-client-version"), _T("0"));
    httpCall->set_custom_header(_T("x-xbl-device-type"), _T("XboxOne"));

    // Need to remove the ContentType for this call
    httpCall->set_content_type_header_value(_T(""));
    if( !customHeader.empty()) 
    {
        httpCall->set_custom_header(_T("x-xbl-contentRestrictions"), customHeader);
    }

    return httpCall->get_response_with_auth(userContext, http_call_response_body_type::json_body)
    .then([userContext, xboxLiveContextSettings, parentId, parentMediaType, childMediaType, orderBy, skipItems](std::shared_ptr<http_call_response> response)
    {
        auto catalogResult = browse_catalog_result::_Deserialize(response->response_body_json());
        auto& result = catalogResult.payload();
        uint32_t itemSize = static_cast<uint32_t>(result.items().size());
        if (itemSize > 0)
        {
            uint32_t continuationSkip = skipItems + itemSize;

            // Initialize the request params for get_next()
            result._Initialize(
                userContext,
                xboxLiveContextSettings,
                parentId,
                parentMediaType,
                childMediaType,
                orderBy,
                continuationSkip
                );
        }

        return utils::generate_xbox_live_result(
            catalogResult,
            response
            );
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END