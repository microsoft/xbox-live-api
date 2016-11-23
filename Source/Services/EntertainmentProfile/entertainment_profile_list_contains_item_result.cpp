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
#include "user_context.h"
#include "xsapi/entertainment_profile.h"
#if TV_API || UNIT_TEST_SERVICES

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_BEGIN

bool entertainment_profile_list_contains_item_result::is_contained() const
{
    return m_isContained;
}

const string_t& entertainment_profile_list_contains_item_result::provider_id() const
{
    return m_providerId;
}

const string_t& entertainment_profile_list_contains_item_result::provider() const
{
    return m_provider;
}

xbox_live_result<entertainment_profile_list_contains_item_result> entertainment_profile_list_contains_item_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    std::error_code errCode;

    entertainment_profile_list_contains_item_result item;

    web::json::value itemJson = utils::extract_json_field(json, _T("ContainedItems"), errCode, true)[0];
    if(!itemJson.is_null())
    {
        item.m_isContained = utils::extract_json_bool(itemJson, _T("Contained"), errCode, true);
        web::json::value innerItemJson = utils::extract_json_field(itemJson, _T("Item"), errCode, false);
        if(!innerItemJson.is_null())
        {
            item.m_provider = utils::extract_json_string(innerItemJson, _T("Provider"), errCode, true);
            item.m_providerId = utils::extract_json_string(innerItemJson, _T("ProviderId"), errCode, true);
        }
    }

    return xbox_live_result<entertainment_profile_list_contains_item_result>(item, errCode);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_END
#endif