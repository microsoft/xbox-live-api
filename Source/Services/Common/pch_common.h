// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#pragma warning(disable: 4503) // C4503: decorated name length exceeded, name was truncated  
#pragma warning(disable: 4242) 
#pragma warning(disable: 4634) // C4634: Discarding XML document comment for invalid target.

#ifdef _WIN32
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>
//#include <winapifamily.h>
#else
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <boost/uuid/uuid.hpp>
#endif
#if XSAPI_A
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
#include <assert.h>

#include "xsapi/types.h"
#include "xsapi/mem.h"
#include "xsapi-c/pal.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/xbox_live_global_c.h"

#include "httpClient/pal.h"
#include "httpClient/httpClient.h"
#include "httpClient/async.h"
#include "httpClient/asyncProvider.h"
#include "httpClient/asyncQueue.h"

#include "http_headers.h"
#include "cpprest/json.h"
#include "cpprest/http_msg.h"
#include "cpprest/uri.h"

#include "xsapi/errors.h"
#include "utils.h"
#include "xbox_live_async.h"
#include "Logger/Log.h"

#include "shared_macros.h"
#if UWP_API
#include <collection.h>
#endif

#ifndef _WIN32
#define UNREFERENCED_PARAMETER(args)
#endif

#ifndef max
#define max(x,y) std::max(x,y)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])
#endif

#if _DEBUG && UNIT_TEST_SERVICES
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif