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

catalog_item::catalog_item()
{
}

catalog_item::catalog_item(
    _In_ string_t id,
    _In_ string_t name,
    _In_ string_t reducedName,
    _In_ uint32_t titleId,
    _In_ utility::datetime releaseDate,
    _In_ string_t productId,
    _In_ string_t sandboxId,
    _In_ bool isBundle,
    _In_ bool isPartOfAnyBundle,
    _In_ std::vector<catalog_item_image> images,
    _In_ std::vector<string_t> availabilityContentIds,
    _In_ media_item_type mediaItemType
    ) :
    m_id(std::move(id)),
    m_name(std::move(name)),
    m_reducedName(std::move(reducedName)),
    m_titleId(titleId),
    m_releaseDate(std::move(releaseDate)),
    m_productId(std::move(productId)),
    m_sandboxId(std::move(sandboxId)),
    m_isBundle(isBundle),
    m_isPartOfAnyBundle(isPartOfAnyBundle),
    m_images(std::move(images)),
    m_availabilityContentIds(std::move(availabilityContentIds)),
    m_mediaItemType(mediaItemType)
{
}

media_item_type catalog_item::item_type() const
{
    return m_mediaItemType;
}

const string_t& catalog_item::id() const
{
    return m_id;
}

const string_t& catalog_item::name() const
{
    return m_name;
}

const string_t& catalog_item::reduced_name() const
{
    return m_reducedName;
}

uint32_t catalog_item::title_id() const
{
    return m_titleId;
}

const utility::datetime& catalog_item::release_date() const
{
    return m_releaseDate;
}

const string_t& catalog_item::product_id() const
{
    return m_productId;
}

const string_t& catalog_item::sandbox_id() const
{
    return m_sandboxId;
}

bool catalog_item::is_bundle() const
{
    return m_isBundle;
}

bool catalog_item::is_part_of_any_bundle() const
{
    return m_isPartOfAnyBundle;
}

const std::vector<catalog_item_image>& catalog_item::images() const
{
    return m_images;
}

const std::vector<string_t>& catalog_item::availability_content_ids() const
{
    return m_availabilityContentIds;
}

xbox_live_result<catalog_item>
catalog_item::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<catalog_item>();

    // Turn this into a collection of content ID strings:
    //
    //   "Availabilities":
    //   [
    //      {
    //         "ContentId":"562acf0f-2021-42d8-b158-f3493214116d",
    //      },
    //      {
    //         "ContentId":"562acf0f-2021-42d8-b158-f3493214116e",
    //      }
    //   ],

    std::error_code errc;
    std::vector<catalog_item_availability> availabilities = utils::extract_json_vector<catalog_item_availability>(catalog_item_availability::_Deserialize, json, _T("Availabilities"), errc, true);
    std::vector<string_t> availabilityContentIds;
    for (uint32_t i = 0; i < availabilities.size(); ++i)
    {
        if (!availabilities.at(i).content_id().empty())
        {
            availabilityContentIds.push_back(availabilities.at(i).content_id());
        }
    }

    auto catalogItemResult = catalog_item( 
        utils::extract_json_string(json, _T("ID"), errc, true),
        utils::extract_json_string(json, _T("Name"), errc, true),
        utils::extract_json_string(json, _T("ReducedName"), errc, false),
        utils::extract_json_int(json, _T("TitleId"), errc, false),
        utils::extract_json_time(json, _T("ReleaseDate"), errc, false),
        utils::extract_json_string(json, _T("ID"), errc, true), //ProductId has been deprecated, use ID instead.
        utils::extract_json_string(json, _T("SandboxId"), errc, false),
        utils::extract_json_bool(json, _T("IsBundle"), errc, false),
        utils::extract_json_bool(json, _T("IsPartOfAnyBundle"), errc, false),
        utils::extract_json_vector<catalog_item_image>(catalog_item_image::_Deserialize, json, _T("Images"), errc, false),
        std::move(availabilityContentIds),
        catalog_service::_Convert_string_to_media_item_type(utils::extract_json_string(json, _T("MediaItemType"), errc, true))
    );

    return xbox_live_result<catalog_item>(catalogItemResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END