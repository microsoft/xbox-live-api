// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

catalog_item_details::catalog_item_details() :
    m_isBundle(false),
    m_isPartOfAnyBundle(false),
    m_mediaItemType(media_item_type::unknown)
{
}

catalog_item_details::catalog_item_details(
    _In_ string_t id,
    _In_ string_t name,
    _In_ string_t reducedName,
    _In_ string_t description,
    _In_ uint32_t titleId,
    _In_ utility::datetime releaseDate,
    _In_ string_t productId,
    _In_ string_t sandboxId,
    _In_ bool isBundle,
    _In_ bool isPartOfAnyBundle,
    _In_ std::vector<catalog_item_image> images,
    _In_ std::vector<catalog_item_availability> availabilities,
    _In_ media_item_type mediaItemType
    ) :
    m_id(std::move(id)),
    m_name(std::move(name)),
    m_reducedName(std::move(reducedName)),
    m_description(std::move(description)),
    m_titleId(titleId),
    m_releaseDate(std::move(releaseDate)),
    m_productId(std::move(productId)),
    m_sandboxId(std::move(sandboxId)),
    m_isBundle(isBundle),
    m_isPartOfAnyBundle(isPartOfAnyBundle),
    m_images(std::move(images)),
    m_availabilities(std::move(availabilities)),
    m_mediaItemType(mediaItemType)
{
}

const media_item_type catalog_item_details::item_type() const
{
    return m_mediaItemType;
}

const string_t& catalog_item_details::id() const
{
    return m_id;
}

const string_t& catalog_item_details::name() const
{
    return m_name;
}

const string_t& catalog_item_details::reduced_name() const
{
    return m_reducedName;
}

const string_t& catalog_item_details::description() const
{
    return m_description;
}

uint32_t catalog_item_details::title_id() const
{
    return m_titleId;
}

const utility::datetime& catalog_item_details::release_date() const
{
    return m_releaseDate;
}

const string_t& catalog_item_details::product_id() const
{
    return m_productId;
}

const string_t& catalog_item_details::sandbox_id() const
{
    return m_sandboxId;
}

bool catalog_item_details::is_bundle() const
{
    return m_isBundle;
}

bool catalog_item_details::is_part_of_any_bundle() const
{
    return m_isPartOfAnyBundle;
}

const std::vector<catalog_item_image>& catalog_item_details::images() const
{
    return m_images;
}

const std::vector<catalog_item_availability>& catalog_item_details::availabilities() const
{
    return m_availabilities;
}

xbox_live_result<catalog_item_details>
catalog_item_details::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<catalog_item_details>();

    std::error_code errc;
    catalog_item_details result;
    result.m_id = utils::extract_json_string(json, _T("ID"), errc, true);
    result.m_name = utils::extract_json_string(json, _T("Name"), errc, true);
    result.m_reducedName = utils::extract_json_string(json, _T("ReducedName"), errc, false);
    result.m_description = utils::extract_json_string(json, _T("Description"), errc, false);
    result.m_titleId = utils::extract_json_int(json, _T("TitleId"), errc, false);
    result.m_releaseDate = utils::extract_json_time(json, _T("ReleaseDate"), errc, false);
    result.m_productId = utils::extract_json_string(json, _T("ID"), errc, true); //ProductId has been deprecated, use ID instead.
    result.m_sandboxId = utils::extract_json_string(json, _T("SandboxId"), errc, false);
    result.m_isBundle = utils::extract_json_bool(json, _T("IsBundle"), errc, false);
    result.m_isPartOfAnyBundle = utils::extract_json_bool(json, _T("IsPartOfAnyBundle"), errc, false);
    result.m_images = utils::extract_json_vector<catalog_item_image>(catalog_item_image::_Deserialize, json, _T("Images"), errc, false);
    result.m_availabilities = utils::extract_json_vector<catalog_item_availability>(catalog_item_availability::_Deserialize, json, _T("Availabilities"), errc, false);
    result.m_mediaItemType = catalog_service::_Convert_string_to_media_item_type(utils::extract_json_string(json, _T("MediaItemType"), errc, true));

    return xbox_live_result<catalog_item_details>(result, errc);

}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END