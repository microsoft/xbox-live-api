#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#include <windows.h>

#if HC_PLATFORM_IS_MICROSOFT

#define XBL_DLLEXPORT __declspec(dllexport)
#define XBL_CALLING_CONV __stdcall

#else

#define XBL_DLLEXPORT
#define XBL_CALLING_CONV

#endif