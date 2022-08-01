// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "fault_injection.h"

#if _DEBUG // fault injection only enabled on debug bits so as not to impact retail titles
static fault_injection g_faultInjection; // not hanging this off of global state since it needs to keep between XblCleanups so long test runs are useful
#endif

STDAPI_(void) XblEnableFaultInjection(_In_ uint64_t featureId) XBL_NOEXCEPT
try
{
#if _DEBUG // fault injection only enabled on debug bits so as not to impact retail titles
    g_faultInjection.m_enabledFeatureMask |= featureId;
#else 
    UNREFERENCED_PARAMETER(featureId);
#endif
}
CATCH_RETURN_WITH(;)

STDAPI_(uint64_t) XblGetFaultCounter() XBL_NOEXCEPT
try
{
#if _DEBUG // testing feature only enabled on debug bits so as not to impact retail titles
    return g_faultInjection.m_failTotalCounter;
#else
    return 0;
#endif
}
CATCH_RETURN_WITH(0)

STDAPI_(void) XblSetFaultInjectOptions(int64_t failFreq, uint64_t freqChangeSpeed, int64_t freqChangeAmount) XBL_NOEXCEPT
try
{
#if _DEBUG // testing feature only enabled on debug bits so as not to impact retail titles
    g_faultInjection.m_failFreq = failFreq;
    g_faultInjection.m_freqChangeSpeed = freqChangeSpeed;
    g_faultInjection.m_freqChangeAmount = freqChangeAmount;
#else 
    UNREFERENCED_PARAMETER(failFreq);
    UNREFERENCED_PARAMETER(freqChangeSpeed);
    UNREFERENCED_PARAMETER(freqChangeAmount);
#endif
}
CATCH_RETURN_WITH(;)

STDAPI_(bool) XblShouldFaultInject(_In_ uint64_t featureId) XBL_NOEXCEPT
try
{
#if _DEBUG // testing feature only enabled on debug bits so as not to impact retail titles
    if ((g_faultInjection.m_enabledFeatureMask & featureId) == featureId)
    {
        g_faultInjection.m_freqChangeCounter++;
        g_faultInjection.m_freqChangeCounter %= g_faultInjection.m_freqChangeSpeed;
        if (g_faultInjection.m_freqChangeCounter == 0)
        {
            // Alter the fail freq every so often so it doesn't just fall into testing a repeated calling pattern
            g_faultInjection.m_failFreq += g_faultInjection.m_freqChangeAmount;
            g_faultInjection.m_failFreq = std::max<int64_t>(0, g_faultInjection.m_failFreq);
        }

        g_faultInjection.m_failTotalCounter++;
        g_faultInjection.m_failCounter++;
        g_faultInjection.m_failCounter %= g_faultInjection.m_failFreq;
        return (g_faultInjection.m_failCounter == 0);
    }
#else
    UNREFERENCED_PARAMETER(featureId);
#endif

    return false;
}
CATCH_RETURN()

