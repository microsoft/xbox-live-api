// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

struct _case_insensitive_cmp
{
    bool operator()(const xsapi_internal_string &str1, const xsapi_internal_string &str2) const
    {
        return _stricmp(str1.c_str(), str2.c_str()) < 0;
    }
};

typedef xsapi_internal_unordered_map<xsapi_internal_string, xsapi_internal_string, _case_insensitive_cmp> http_headers;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END