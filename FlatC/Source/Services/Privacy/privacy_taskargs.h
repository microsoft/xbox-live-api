// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "taskargs.h"
#include "xsapi/privacy_c.h"
#include "privacy_permission_check_result_impl.h"
#include "privacy_multiple_permissions_check_result_impl.h"

struct privacy_taskargs
{
    XSAPI_XBOX_LIVE_CONTEXT* pXboxLiveContext;
};

struct privacy_user_list_taskargs : public privacy_taskargs, public taskargs
{
    std::wstring subpathName;
    std::vector<PCSTR> xboxUserIdPointers;
    std::vector<std::string> xboxUserIds;
};

struct privacy_check_permission_taskargs : public privacy_taskargs, public taskargs_with_payload<XSAPI_PRIVACY_PERMISSION_CHECK_RESULT>
{
    std::string permissionId;
    std::string xboxUserId;
    XSAPI_PRIVACY_PERMISSION_CHECK_RESULT_IMPL payloadImpl;
};

struct privacy_check_multiple_permissions_taskargs : public privacy_taskargs, public taskargs
{
    std::vector<string_t> permissionIds;
    std::vector<string_t> xboxUserIds;
    std::vector<XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT> permissions;
    std::vector<XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT_IMPL> permissionsImpls;
};