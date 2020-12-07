// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"

using namespace xbox::services;

xsapi_internal_vector<xsapi_internal_string>
utils::get_locale_list()
{
    NSString *local = [NSLocale preferredLanguages][0];
    string_t localeStr([local UTF8String]);
    xsapi_internal_vector<xsapi_internal_string> localeList;
    localeList.push_back(utils::internal_string_from_string_t(localeStr));
    return localeList;
}
