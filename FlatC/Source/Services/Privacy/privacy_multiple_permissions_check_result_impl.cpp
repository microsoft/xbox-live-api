// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_multiple_permissions_check_result_impl.h"

using namespace xbox::services::privacy;

XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL::XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL(
    const xbox::services::privacy::multiple_permissions_check_result& cppObj,
    XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT* cObj
    )
{
    update(cppObj, cObj);
}

void XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL::update(
    const multiple_permissions_check_result& cppObj,
    XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT* cObj
    )
{
    m_xboxUserId = utils_c::to_utf8string(cppObj.xbox_user_id());
    
    auto& cppItems = cppObj.items();
    m_items = std::vector<XSAPI_PRIVACY_PERMISSION_CHECK_RESULT>(cppItems.size());

    unsigned int index = 0;
    for (auto& cppItem : cppItems)
    {
        m_itemsImpls.emplace_back(cppItem, &m_items[index++]);
    }

    cObj->xboxUserId = m_xboxUserId.data();
    cObj->items = m_items.size() > 0 ? &m_items[0] : nullptr;
    cObj->itemsCount = (uint32_t)m_items.size();
}