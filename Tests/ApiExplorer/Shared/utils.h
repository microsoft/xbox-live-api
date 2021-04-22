// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#if HC_PLATFORM != HC_PLATFORM_UWP
#include <Xal/xal.h>
#endif

// internal test-only APIs
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN                     namespace xbox { namespace services {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END                       }}
#include "../../../Source/Shared/fault_injection.h"

#define VERIFY_IS_TRUE(x, y) if (SUCCEEDED(hr)) { hr = VerifyIsTrue(x, y); }
#define ENSURE_IS_TRUE(x, y) if (FAILED(VerifyIsTrue(x, y))) return LuaReturnHR(L, E_ABORT);

std::string ConvertHRtoString(HRESULT hr);
std::string ConvertHR(HRESULT hr);
std::string ReadFile(std::string fileName);
uint64_t ConvertStringToUInt64(std::string str);
HRESULT CreateQueueIfNeeded();
HRESULT VerifyIsTrue(bool condition, const char* pszCondition);
HRESULT ConvertXboxLiveErrorCodeToHresult(_In_ const std::error_code& errCode);
