// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#ifdef _WIN32
    #ifndef _WIN32_WINNT_WIN10
    #define _WIN32_WINNT_WIN10 0x0A00
    #endif
#endif //#ifdef _WIN32

#include <iostream>
#include <vector>
#include <assert.h>
#include <httpClient/httpClient.h>
#if HC_PLATFORM != HC_PLATFORM_UWP
    #include <Xal/xal.h>
    #if HC_PLATFORM != HC_PLATFORM_GDK
    #include <Xal/xal_platform.h>
    #endif
#endif

#include <stdlib.h>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <mutex>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <thread>
#include <cinttypes>
#include <unordered_map>

#if HC_PLATFORM != HC_PLATFORM_IOS
#include <xsapi-cpp/services.h>
#endif

#include <xsapi-c/services_c.h>

#include "pal.h"
#include "utils.h"
#include "mem_hook.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#if XSAPI_BUILT_FROM_SOURCE
#undef RAPIDJSON_NAMESPACE
#undef RAPIDJSON_NAMESPACE_BEGIN
#undef RAPIDJSON_NAMESPACE_END
#endif
#include "../../../External/rapidjson/include/rapidjson/document.h"

#include "../Include/multidevice.h"
#include "../Include/api_explorer.h"
#include "../Include/runner.h"
#include "../APIs/apis.h"

#if HC_PLATFORM_IS_MICROSOFT
    #define SPRINTF(buffer, size, format, ...) sprintf_s(buffer, size, format, __VA_ARGS__)
#else
    #define SPRINTF(buffer, size, format, ...) snprintf(buffer, size, format, __VA_ARGS__)
#endif
