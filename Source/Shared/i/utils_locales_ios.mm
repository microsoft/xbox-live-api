//
//  utils_locales_ios.cpp
//  XboxLiveServices
//
//  Created by blake on 5/13/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#include "pch.h"
#include "utils.h"

using namespace xbox::services;

std::vector<string_t>
utils::get_locale_list()
{
    NSString *local = [NSLocale preferredLanguages][0];
    string_t localeStr([local UTF8String]);
    std::vector<string_t> localeList;
    localeList.push_back(localeStr);
    return localeList;
}