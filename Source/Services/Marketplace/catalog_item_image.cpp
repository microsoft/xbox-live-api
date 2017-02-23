// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

catalog_item_image::catalog_item_image() :
    m_height(0),
    m_width(0)
{
}

catalog_item_image::catalog_item_image(
    _In_ string_t id,
    _In_ web::uri resizeUrl,
    _In_ std::vector<string_t> purposes,
    _In_ string_t purpose,
    _In_ uint32_t height,
    _In_ uint32_t width
    ) :
    m_id(std::move(id)),
    m_resizeUrl(std::move(resizeUrl)),
    m_purposes(std::move(purposes)),
    m_purpose(std::move(purpose)),
    m_height(height),
    m_width(width)
{
}

const string_t& catalog_item_image::id() const
{
    return m_id;
}

const web::uri& catalog_item_image::resize_url() const
{
    return m_resizeUrl;
}

const std::vector<string_t>& catalog_item_image::purposes() const
{
    return m_purposes;
}

const string_t& catalog_item_image::purpose() const
{
    return m_purpose;
}

uint32_t catalog_item_image::height() const
{
    return m_height;
}

uint32_t catalog_item_image::width() const
{
    return m_width;
}

xbox_live_result<catalog_item_image>
catalog_item_image::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<catalog_item_image>();

    std::error_code errc;
    catalog_item_image result;
    result.m_id = utils::extract_json_string(json, _T("ID"), errc, true);
    result.m_resizeUrl = utils::extract_json_string(json, _T("ResizeUrl"), errc, false);
    result.m_purpose = utils::extract_json_string(json, _T("Purpose"), errc, false);
    result.m_purposes = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("Purposes"), errc, false);
    result.m_height = utils::extract_json_int(json, _T("Height"), errc, true);
    result.m_width = utils::extract_json_int(json, _T("Width"), errc, true);

    return xbox_live_result<catalog_item_image>(result, errc);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END