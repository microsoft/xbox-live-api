// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_permission_check_result_impl.h"

using namespace xbox::services::privacy;

XSAPI_PRIVACY_PERMISSION_CHECK_RESULT_IMPL::XSAPI_PRIVACY_PERMISSION_CHECK_RESULT_IMPL(
    const permission_check_result& cppObj,
    XSAPI_PRIVACY_PERMISSION_CHECK_RESULT* cObj
    )
{
    update(cppObj, cObj);
}

void XSAPI_PRIVACY_PERMISSION_CHECK_RESULT_IMPL::update(
    const permission_check_result& cppObj,
    XSAPI_PRIVACY_PERMISSION_CHECK_RESULT* cObj
    )
{
    m_permissionRequested = utils_c::to_utf8string(cppObj.permission_requested());

    auto& denyReasons = cppObj.deny_reasons();
    m_items = std::vector<XSAPI_PRIVACY_PERMISSION_DENY_REASON>(denyReasons.size());

    unsigned int index = 0;
    for (auto& cppReason : denyReasons)
    {
        m_itemsImpls.emplace_back(cppReason, &m_items[index++]);
    }

    cObj->isAllowed = cppObj.is_allowed();
    cObj->permissionRequested = m_permissionRequested.data();
    cObj->denyReasons = m_items.size() > 0 ? &m_items[0] : nullptr;
    cObj->denyReasonsCount = (uint32_t)m_items.size();
}