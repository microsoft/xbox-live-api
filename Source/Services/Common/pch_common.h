//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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
#if XSAPI_A
#define _NOEXCEPT noexcept
#endif

// TODO: dealing with inline "xbox::services" on many of the signatures here, is this safe/the right move?
#ifdef XBOX_LIVE_NAMESPACE
#undef XBOX_LIVE_NAMESPACE
#endif
#if BEAM_API
#define XBOX_LIVE_NAMESPACE xbox::services::beam
#else
#define XBOX_LIVE_NAMESPACE xbox::services
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

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library

#include "shared_macros.h"
#if UWP_API
#include <collection.h>
#endif

#include "xsapi/errors.h"
#include "utils.h"
#include "Logger/Log.h"


#ifndef _WIN32
#define UNREFERENCED_PARAMETER(args)
#endif

#ifndef max
#define max(x,y) std::max(x,y)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])
#endif