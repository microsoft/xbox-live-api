// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <wmistr.h>
#include <evntrace.h>
#include "evntprov.h"
//
//  Initial Defs
//
#if !defined(ETW_INLINE)
#define ETW_INLINE DECLSPEC_NOINLINE __inline
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//
// Allow disabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION
#if  !defined(McGenDebug)
#define McGenDebug(a,b)
#endif 


#if !defined(MCGEN_TRACE_CONTEXT_DEF)
#define MCGEN_TRACE_CONTEXT_DEF
typedef struct _MCGEN_TRACE_CONTEXT
{
    TRACEHANDLE            RegistrationHandle;
    TRACEHANDLE            Logger;
    ULONGLONG              MatchAnyKeyword;
    ULONGLONG              MatchAllKeyword;
    ULONG                  Flags;
    ULONG                  IsEnabled;
    UCHAR                  Level; 
    UCHAR                  Reserve;
    USHORT                 EnableBitsCount;
    PULONG                 EnableBitMask;
    const ULONGLONG*       EnableKeyWords;
    const UCHAR*           EnableLevel;
} MCGEN_TRACE_CONTEXT, *PMCGEN_TRACE_CONTEXT;
#endif

#if !defined(MCGEN_LEVEL_KEYWORD_ENABLED_DEF)
#define MCGEN_LEVEL_KEYWORD_ENABLED_DEF
FORCEINLINE
BOOLEAN
McGenLevelKeywordEnabled(
    _In_ PMCGEN_TRACE_CONTEXT EnableInfo,
    _In_ UCHAR Level,
    _In_ ULONGLONG Keyword
    )
{
    //
    // Check if the event Level is lower than the level at which
    // the channel is enabled.
    // If the event Level is 0 or the channel is enabled at level 0,
    // all levels are enabled.
    //

    if ((Level <= EnableInfo->Level) || // This also covers the case of Level == 0.
        (EnableInfo->Level == 0)) {

        //
        // Check if Keyword is enabled
        //

        if ((Keyword == (ULONGLONG)0) ||
            ((Keyword & EnableInfo->MatchAnyKeyword) &&
             ((Keyword & EnableInfo->MatchAllKeyword) == EnableInfo->MatchAllKeyword))) {
            return TRUE;
        }
    }

    return FALSE;

}
#endif

#if !defined(MCGEN_EVENT_ENABLED_DEF)
#define MCGEN_EVENT_ENABLED_DEF
FORCEINLINE
BOOLEAN
McGenEventEnabled(
    _In_ PMCGEN_TRACE_CONTEXT EnableInfo,
    _In_ PCEVENT_DESCRIPTOR EventDescriptor
    )
{

    return McGenLevelKeywordEnabled(EnableInfo, EventDescriptor->Level, EventDescriptor->Keyword);

}
#endif


//
// EnableCheckMacro
//
#ifndef MCGEN_ENABLE_CHECK
#define MCGEN_ENABLE_CHECK(Context, Descriptor) (Context.IsEnabled &&  McGenEventEnabled(&Context, &Descriptor))
#endif

#if !defined(MCGEN_CONTROL_CALLBACK)
#define MCGEN_CONTROL_CALLBACK

DECLSPEC_NOINLINE __inline
VOID
__stdcall
McGenControlCallbackV2(
    _In_ LPCGUID SourceId,
    _In_ ULONG ControlCode,
    _In_ UCHAR Level,
    _In_ ULONGLONG MatchAnyKeyword,
    _In_ ULONGLONG MatchAllKeyword,
    _In_opt_ PEVENT_FILTER_DESCRIPTOR FilterData,
    _Inout_opt_ PVOID CallbackContext
    )
/*++

Routine Description:

    This is the notification callback for Windows Vista and later.

Arguments:

    SourceId - The GUID that identifies the session that enabled the provider. 

    ControlCode - The parameter indicates whether the provider 
                  is being enabled or disabled.

    Level - The level at which the event is enabled.

    MatchAnyKeyword - The bitmask of keywords that the provider uses to 
                      determine the category of events that it writes.

    MatchAllKeyword - This bitmask additionally restricts the category 
                      of events that the provider writes. 

    FilterData - The provider-defined data.

    CallbackContext - The context of the callback that is defined when the provider 
                      called EtwRegister to register itself.

Remarks:

    ETW calls this function to notify provider of enable/disable

--*/
{
    PMCGEN_TRACE_CONTEXT Ctx = (PMCGEN_TRACE_CONTEXT)CallbackContext;
    ULONG Ix;
#ifndef MCGEN_PRIVATE_ENABLE_CALLBACK_V2
    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(FilterData);
#endif

    if (Ctx == NULL) {
        return;
    }

    switch (ControlCode) {

        case EVENT_CONTROL_CODE_ENABLE_PROVIDER:
            Ctx->Level = Level;
            Ctx->MatchAnyKeyword = MatchAnyKeyword;
            Ctx->MatchAllKeyword = MatchAllKeyword;
            Ctx->IsEnabled = EVENT_CONTROL_CODE_ENABLE_PROVIDER;

            for (Ix = 0; Ix < Ctx->EnableBitsCount; Ix += 1) {
                if (McGenLevelKeywordEnabled(Ctx, Ctx->EnableLevel[Ix], Ctx->EnableKeyWords[Ix]) != FALSE) {
                    Ctx->EnableBitMask[Ix >> 5] |= (1 << (Ix % 32));
                } else {
                    Ctx->EnableBitMask[Ix >> 5] &= ~(1 << (Ix % 32));
                }
            }
            break;

        case EVENT_CONTROL_CODE_DISABLE_PROVIDER:
            Ctx->IsEnabled = EVENT_CONTROL_CODE_DISABLE_PROVIDER;
            Ctx->Level = 0;
            Ctx->MatchAnyKeyword = 0;
            Ctx->MatchAllKeyword = 0;
            if (Ctx->EnableBitsCount > 0) {
                RtlZeroMemory(Ctx->EnableBitMask, (((Ctx->EnableBitsCount - 1) / 32) + 1) * sizeof(ULONG));
            }
            break;
 
        default:
            break;
    }

#ifdef MCGEN_PRIVATE_ENABLE_CALLBACK_V2
    //
    // Call user defined callback
    //
    MCGEN_PRIVATE_ENABLE_CALLBACK_V2(
        SourceId,
        ControlCode,
        Level,
        MatchAnyKeyword,
        MatchAllKeyword,
        FilterData,
        CallbackContext
        );
#endif
   
    return;
}

#endif
#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION
//+
// Provider Microsoft.Xbox.Services.Tracing Event Count 4
//+
EXTERN_C __declspec(selectany) const GUID ProviderGuid = {0x9594a560, 0xe985, 0x4ee6, {0xb0, 0xb5, 0x0d, 0xac, 0x4f, 0x92, 0x41, 0x44}};

//
// Event Descriptors
//
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR XSAPI_Verbose = {0x1, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0};
#define XSAPI_Verbose_value 0x1
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR XSAPI_Info = {0x2, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0};
#define XSAPI_Info_value 0x2
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR XSAPI_Warn = {0x3, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0};
#define XSAPI_Warn_value 0x3
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR XSAPI_Error = {0x4, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0};
#define XSAPI_Error_value 0x4

//
// Note on Generate Code from Manifest for Windows Vista and above
//
//Structures :  are handled as a size and pointer pairs. The macro for the event will have an extra 
//parameter for the size in bytes of the structure. Make sure that your structures have no extra padding.
//
//Strings: There are several cases that can be described in the manifest. For array of variable length 
//strings, the generated code will take the count of characters for the whole array as an input parameter. 
//
//SID No support for array of SIDs, the macro will take a pointer to the SID and use appropriate 
//GetLengthSid function to get the length.
//

//
// Allow disabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION

//
// Globals 
//


//
// Event Enablement Bits
//

EXTERN_C __declspec(selectany) DECLSPEC_CACHEALIGN ULONG Microsoft_Xbox_Services_TracingEnableBits[1];
EXTERN_C __declspec(selectany) const ULONGLONG Microsoft_Xbox_Services_TracingKeywords[4] = {0x0, 0x0, 0x0, 0x0};
EXTERN_C __declspec(selectany) const UCHAR Microsoft_Xbox_Services_TracingLevels[4] = {5, 4, 3, 2};
EXTERN_C __declspec(selectany) MCGEN_TRACE_CONTEXT ProviderGuid_Context = {0, 0, 0, 0, 0, 0, 0, 0, 4, Microsoft_Xbox_Services_TracingEnableBits, Microsoft_Xbox_Services_TracingKeywords, Microsoft_Xbox_Services_TracingLevels};

EXTERN_C __declspec(selectany) REGHANDLE Microsoft_Xbox_Services_TracingHandle = (REGHANDLE)0;

#if !defined(McGenEventRegisterUnregister)
#define McGenEventRegisterUnregister
#pragma warning(push)
#pragma warning(disable:6103)
DECLSPEC_NOINLINE __inline
ULONG __stdcall
McGenEventRegister(
    _In_ LPCGUID ProviderId,
    _In_opt_ PENABLECALLBACK EnableCallback,
    _In_opt_ PVOID CallbackContext,
    _Inout_ PREGHANDLE RegHandle
    )
/*++

Routine Description:

    This function registers the provider with ETW USER mode.

Arguments:
    ProviderId - Provider ID to be register with ETW.

    EnableCallback - Callback to be used.

    CallbackContext - Context for this provider.

    RegHandle - Pointer to registration handle.

Remarks:

    If the handle != NULL will return ERROR_SUCCESS

--*/
{
    ULONG Error;


    if (*RegHandle) {
        //
        // already registered
        //
        return ERROR_SUCCESS;
    }

    Error = EventRegister( ProviderId, EnableCallback, CallbackContext, RegHandle); 

    return Error;
}
#pragma warning(pop)


DECLSPEC_NOINLINE __inline
ULONG __stdcall
McGenEventUnregister(_Inout_ PREGHANDLE RegHandle)
/*++

Routine Description:

    Unregister from ETW USER mode

Arguments:
            RegHandle this is the pointer to the provider context
Remarks:
            If provider has not been registered, RegHandle == NULL,
            return ERROR_SUCCESS
--*/
{
    ULONG Error;


    if(!(*RegHandle)) {
        //
        // Provider has not registerd
        //
        return ERROR_SUCCESS;
    }

    Error = EventUnregister(*RegHandle); 
    *RegHandle = (REGHANDLE)0;
    
    return Error;
}
#endif
//
// Register with ETW Vista +
//
#ifndef EventRegisterMicrosoft_Xbox_Services_Tracing
#define EventRegisterMicrosoft_Xbox_Services_Tracing() McGenEventRegister(&ProviderGuid, McGenControlCallbackV2, &ProviderGuid_Context, &Microsoft_Xbox_Services_TracingHandle) 
#endif

//
// UnRegister with ETW
//
#ifndef EventUnregisterMicrosoft_Xbox_Services_Tracing
#define EventUnregisterMicrosoft_Xbox_Services_Tracing() McGenEventUnregister(&Microsoft_Xbox_Services_TracingHandle) 
#endif

//
// Enablement check macro for XSAPI_Verbose
//

#define EventEnabledXSAPI_Verbose() ((Microsoft_Xbox_Services_TracingEnableBits[0] & 0x00000001) != 0)

//
// Event Macro for XSAPI_Verbose
//
#define EventWriteXSAPI_Verbose(Catalog, Message)\
        EventEnabledXSAPI_Verbose() ?\
        Template_ss(Microsoft_Xbox_Services_TracingHandle, &XSAPI_Verbose, Catalog, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for XSAPI_Info
//

#define EventEnabledXSAPI_Info() ((Microsoft_Xbox_Services_TracingEnableBits[0] & 0x00000002) != 0)

//
// Event Macro for XSAPI_Info
//
#define EventWriteXSAPI_Info(Catalog, Message)\
        EventEnabledXSAPI_Info() ?\
        Template_ss(Microsoft_Xbox_Services_TracingHandle, &XSAPI_Info, Catalog, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for XSAPI_Warn
//

#define EventEnabledXSAPI_Warn() ((Microsoft_Xbox_Services_TracingEnableBits[0] & 0x00000004) != 0)

//
// Event Macro for XSAPI_Warn
//
#define EventWriteXSAPI_Warn(Catalog, Message)\
        EventEnabledXSAPI_Warn() ?\
        Template_ss(Microsoft_Xbox_Services_TracingHandle, &XSAPI_Warn, Catalog, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for XSAPI_Error
//

#define EventEnabledXSAPI_Error() ((Microsoft_Xbox_Services_TracingEnableBits[0] & 0x00000008) != 0)

//
// Event Macro for XSAPI_Error
//
#define EventWriteXSAPI_Error(Catalog, Message)\
        EventEnabledXSAPI_Error() ?\
        Template_ss(Microsoft_Xbox_Services_TracingHandle, &XSAPI_Error, Catalog, Message)\
        : ERROR_SUCCESS\

#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION


//
// Allow Diasabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION

//
// Template Functions 
//
//
//Template from manifest : tidMessage
//
#ifndef Template_ss_def
#define Template_ss_def
ETW_INLINE
ULONG
Template_ss(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_opt_ LPCSTR  _Arg0,
    _In_opt_ LPCSTR  _Arg1
    )
{
#define ARGUMENT_COUNT_ss 2

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_ss];

    EventDataDescCreate(&EventData[0], 
                        (_Arg0 != NULL) ? _Arg0 : "NULL",
                        (_Arg0 != NULL) ? (ULONG)((strlen(_Arg0) + 1) * sizeof(CHAR)) : (ULONG)sizeof("NULL"));

    EventDataDescCreate(&EventData[1], 
                        (_Arg1 != NULL) ? _Arg1 : "NULL",
                        (_Arg1 != NULL) ? (ULONG)((strlen(_Arg1) + 1) * sizeof(CHAR)) : (ULONG)sizeof("NULL"));

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_ss, EventData);
}
#endif

#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION

#if defined(__cplusplus)
};
#endif

