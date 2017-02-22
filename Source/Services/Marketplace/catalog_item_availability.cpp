// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

catalog_item_availability::catalog_item_availability() :
    m_isPurchasable(false),
    m_listPrice(0),
    m_price(0),
    m_consumableQuantity(0)
{
}

catalog_item_availability::catalog_item_availability(
    _In_ string_t contentId,
    _In_ string_t signedOffer
    ) :
    m_contentId(std::move(contentId)),
    m_signedOffer(std::move(signedOffer)),
    m_isPurchasable(false),
    m_listPrice(0),
    m_price(0),
    m_consumableQuantity(0)
{
}

catalog_item_availability::catalog_item_availability(
    _In_ string_t contentId,
    _In_ std::vector<string_t> acceptablePaymentInstrumentTypes,
    _In_ string_t availabilityTitle,
    _In_ string_t availabilityDescription,
    _In_ string_t currencyCode,
    _In_ string_t displayPrice,
    _In_ string_t displayListPrice,
    _In_ string_t distributionType,
    _In_ bool isPurchasable,
    _In_ double listPrice,
    _In_ double price,
    _In_ uint32_t consumableQuantity,
    _In_ string_t promotionalText,
    _In_ string_t signedOffer,
    _In_ string_t offerId,
    _In_ string_t offerDisplayDataJson
    ) :
    m_contentId(std::move(contentId)),
    m_acceptablePaymentInstrumentTypes(std::move(acceptablePaymentInstrumentTypes)),
    m_availabilityTitle(std::move(availabilityTitle)),
    m_availabilityDescription(std::move(availabilityDescription)),
    m_currencyCode(std::move(currencyCode)),
    m_displayPrice(std::move(displayPrice)),
    m_displayListPrice(std::move(displayListPrice)),
    m_distributionType(std::move(distributionType)),
    m_isPurchasable(isPurchasable),
    m_listPrice(listPrice),
    m_price(price),
    m_consumableQuantity(consumableQuantity),
    m_promotionalText(std::move(promotionalText)),
    m_signedOffer(std::move(signedOffer)),
    m_offerId(std::move(offerId)),
    m_offerDisplayDataJson(std::move(offerDisplayDataJson))
{
}

const string_t& catalog_item_availability::content_id() const
{
    return m_contentId;
}

const std::vector<string_t>& catalog_item_availability::acceptable_payment_Instrument_types() const
{
    return m_acceptablePaymentInstrumentTypes;
}

const string_t& catalog_item_availability::availability_title() const
{
    return m_availabilityTitle;
}

const string_t& catalog_item_availability::availability_description() const
{
    return m_availabilityDescription;
}

const string_t& catalog_item_availability::currency_code() const
{
    return m_currencyCode;
}

const string_t& catalog_item_availability::display_price() const
{
    return m_displayPrice;
}

const string_t& catalog_item_availability::display_list_price() const
{
    return m_displayListPrice;
}

const string_t& catalog_item_availability::distribution_type() const
{
    return m_distributionType;
}

bool catalog_item_availability::is_purchasable() const
{
    return m_isPurchasable;
}

double catalog_item_availability::list_price() const
{
    return m_listPrice;
}

double catalog_item_availability::price() const
{
    return m_price;
}

uint32_t catalog_item_availability::consumable_quantity() const
{
    return m_consumableQuantity;
}

const string_t& catalog_item_availability::promotional_text() const
{
    return m_promotionalText;
}

const string_t& catalog_item_availability::signed_offer() const
{
    return m_signedOffer;
}

const string_t& catalog_item_availability::offer_id() const
{
    return m_offerId;
}

const string_t& catalog_item_availability::offer_display_data_json() const
{
    return m_offerDisplayDataJson;
}

xbox_live_result<catalog_item_availability>
catalog_item_availability::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<catalog_item_availability>();

    std::error_code errc;
    web::json::value offerDisplayDataJson = utils::extract_json_field(json, _T("OfferDisplayData"), errc, false);
    if (!offerDisplayDataJson.is_null())
    {
        web::json::value offerDisplayDataJsonObject = utils::json_get_value_from_string(offerDisplayDataJson.as_string());

        auto result = catalog_item_availability(
            utils::extract_json_string(json, _T("ContentId"), true),
            utils::extract_json_vector<string_t>(utils::json_string_extractor, offerDisplayDataJsonObject, _T("acceptablePaymentInstrumentTypes"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("availabilityTitle"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("availabilityDescription"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("currencyCode"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("displayPrice"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("displayListPrice"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("distributionType"), errc, false),
            utils::extract_json_bool(offerDisplayDataJsonObject, _T("isPurchasable"), errc, false),
            utils::extract_json_double(offerDisplayDataJsonObject, _T("listPrice"), errc, false),
            utils::extract_json_double(offerDisplayDataJsonObject, _T("price"), errc, false),
            utils::extract_json_int(offerDisplayDataJsonObject, _T("quantity"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("promotionalText"), errc, false),
            utils::extract_json_string(json, _T("SignedOffer"), errc, false),
            utils::extract_json_string(offerDisplayDataJsonObject, _T("offerId"), errc, true),
            offerDisplayDataJsonObject.serialize()
            );

        return xbox_live_result<catalog_item_availability>(result, errc);
    }

    auto result = catalog_item_availability(
        utils::extract_json_string(json, _T("ContentId"), errc, true),
        utils::extract_json_string(json, _T("SignedOffer"), errc, false)
        );

    return xbox_live_result<catalog_item_availability>(result, errc);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END
