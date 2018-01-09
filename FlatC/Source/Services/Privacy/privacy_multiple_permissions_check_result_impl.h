// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/privacy_c.h"
#include "privacy_permission_check_result_impl.h"

class XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL
{
public:
    XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL() {}

    XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL(
        const xbox::services::privacy::multiple_permissions_check_result& cppObj,
        XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT* cObj);

    void update(const xbox::services::privacy::multiple_permissions_check_result& cppObj, XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT* cObj);

private:
    std::vector<XSAPI_PRIVACY_PERMISSION_CHECK_RESULT> m_items;
    std::vector<XSAPI_PRIVACY_PERMISSION_CHECK_RESULT_IMPL> m_itemsImpls;
    std::string m_xboxUserId;
};