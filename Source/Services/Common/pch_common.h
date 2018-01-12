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
#include <vector>
#include <memory>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>

#include "xsapi/types.h"
#include "httpClient/types.h"
#include "httpClient/httpClient.h"
#include "httpClient/task.h"

#if XSAPI_CPP && UWP_API // TODO eventually extend where these are included
#include "xsapi-c/types_c.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/xbox_live_global_c.h"
#include "utils_c.h"
#endif

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library

#include "xsapi/errors.h"
#include "utils.h"
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
