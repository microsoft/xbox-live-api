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
