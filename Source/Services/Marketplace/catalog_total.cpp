// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN

catalog_total::catalog_total() :
    m_total(0)
{
}

catalog_total::catalog_total(
    _In_ uint32_t total
    ) :
    m_total(total)
{
}

uint32_t catalog_total::total() const
{
    return m_total;
}

xbox_live_result<catalog_total>
catalog_total::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<catalog_total>();

    std::error_code errc;
    catalog_total result;
    result.m_total = utils::extract_json_int(json, _T("Count"), errc, true);

    return xbox_live_result<catalog_total>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END
