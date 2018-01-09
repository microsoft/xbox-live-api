// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#pragma warning(disable: 4503) // C4503: decorated name length exceeded, name was truncated  
#pragma warning(disable: 4242) 

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

// STL includes
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <codecvt>
#include <iomanip>
#include <ctime>

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

#if _MSC_VER <= 1800
typedef std::chrono::system_clock chrono_clock_t;
#else
typedef std::chrono::steady_clock chrono_clock_t;
#endif

#include "xsapi/services.h"

// libHttpClient
#include "httpClient/httpClient.h"
#include "httpClient/task.h"
#include "httpClient/trace.h"

#include "xsapi/types_c.h"
#include "xsapi/errors_c.h"
#include "xsapi/xbox_live_global_c.h"
#include "utils.h"
#include "singleton.h"

HC_DECLARE_TRACE_AREA(XSAPI_C_TRACE);

#define CATCH_RETURN() CATCH_RETURN_IMPL(__FILE__, __LINE__)

#define CATCH_RETURN_IMPL(file, line) \
    catch (std::bad_alloc const& e) { return utils::std_bad_alloc_to_result(e, file, line); } \
    catch (std::exception const& e) { return utils::std_exception_to_result(e, file, line); } \
    catch (...) { return utils::unknown_exception_to_result(file, line); }

#define CATCH_RETURN_WITH(errCode) CATCH_RETURN_IMPL_WITH(__FILE__, __LINE__, errCode)

#define CATCH_RETURN_IMPL_WITH(file, line, errCode) \
    catch (std::bad_alloc const& e) { utils::std_bad_alloc_to_result(e, file, line); return errCode; } \
    catch (std::exception const& e) { utils::std_exception_to_result(e, file, line); return errCode; } \
    catch (...) { utils::unknown_exception_to_result(file, line); return errCode; }