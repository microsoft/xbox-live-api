// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "httpClient/config.h"

#if HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM_IS_APPLE
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wc++14-extensions"
#pragma clang diagnostic ignored "-Wreorder"
#endif

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning(disable : 4355 4628 4266 4555 4263 4264 4868 4062 4826 4244 4654)
#pragma warning(disable : 4619 4616 4350 4351 4472 4640 5038 4471)
#pragma warning(disable : 4061 4265 4365 4571 4623 4625 4626 4668 4710 4711 4746 4774 4820 4987 4996 5026 5027 5031 5032 5039 5037 5045)

#pragma warning(disable: 4503) // C4503: decorated name length exceeded, name was truncated  
#pragma warning(disable: 4242) 
#pragma warning(disable: 4634) // C4634: Discarding XML document comment for invalid target.
#pragma warning(disable: 26812)  // enum instead of enum class

#if _DEBUG && XSAPI_UNIT_TESTS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

#else // HC_PLATFORM_IS_MICROSOFT

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>

#endif // HC_PLATFORM_IS_MICROSOFT

#if HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM_IS_APPLE
#define _NOEXCEPT noexcept
#endif

// STL includes
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <queue>
#include <vector>
#include <memory>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <limits>
#include <list>
#include <assert.h>
#include <array>
#if HC_PLATFORM_IS_MICROSOFT
#include <objbase.h>
#endif

#include "httpClient/pal.h"
#include "httpClient/httpClient.h"
#include "XAsync.h"
#include "XAsyncProvider.h"
#include "XTaskQueue.h"

#include "xsapi-c/types_c.h"
#include "internal_types.h"
#include "xsapi-c/pal.h"
#include "internal_mem.h"

#include <Xal/xal.h>
#include "http_headers.h"
#if HC_PLATFORM != HC_PLATFORM_GDK
#include "cpprest/http_msg.h"
#endif
#include "http_utils.h"
#if HC_PLATFORM == HC_PLATFORM_GDK
#include "HookedUri/uri.h"
#endif

#undef max // needed for the version of RapidJson we're using to avoid warnings
#undef min

#define RAPIDJSON_NAMESPACE xbox::services::rapidjson
#define RAPIDJSON_NAMESPACE_BEGIN namespace xbox { namespace services { namespace rapidjson {
#define RAPIDJSON_NAMESPACE_END } } }
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "async_helpers.h"
#include "xsapi_utils.h"
#include "xsapi_json_utils.h"
#include "public_utils_legacy.h"
#include "ref_counter.h"
#include "internal_errors.h"
#include "Logger/log.h"
#include "xbox_live_app_config_internal.h"
#include "user.h"
#include "http_call_wrapper_internal.h"
#include "global_state.h"
#include "enum_traits.h"
#include "xsapi-c/xbox_live_context_c.h"

#include "shared_macros.h"
#if HC_PLATFORM == HC_PLATFORM_UWP
#include <collection.h>
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])
#endif

#if HC_PLATFORM_IS_MICROSOFT
    #define SPRINTF(buffer, size, format, ...) sprintf_s(buffer, size, format, __VA_ARGS__)
#else
    #define SPRINTF(buffer, size, format, ...) sprintf(buffer, format, __VA_ARGS__)
#endif
