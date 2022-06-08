// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#define INJECTION_FEATURE_USER 0x0001
#define INJECTION_FEATURE_HTTP 0x0002

extern "C"
{
STDAPI_(void) XblEnableFaultInjection(_In_ uint64_t featureId) XBL_NOEXCEPT;
STDAPI_(bool) XblShouldFaultInject(_In_ uint64_t featureId) XBL_NOEXCEPT;
STDAPI_(uint64_t) XblGetFaultCounter() XBL_NOEXCEPT;
STDAPI_(void) XblSetFaultInjectOptions(int64_t failFreq, uint64_t freqChangeSpeed, int64_t freqChangeAmount) XBL_NOEXCEPT;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

struct fault_injection
{
    uint64_t m_enabledFeatureMask{ 0 };
    uint64_t m_failCounter{ 0 };
    uint64_t m_failTotalCounter{ 0 };
    int64_t m_failFreq{ 3 };

    // Alter the fail freq every so often so it doesn't just fall into testing a repeated calling pattern
    uint64_t m_freqChangeCounter{ 0 };
    uint64_t m_freqChangeSpeed{ 7 };
    int64_t m_freqChangeAmount{ 1 };
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
