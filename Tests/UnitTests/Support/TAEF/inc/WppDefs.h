//---------------------------------------------------------------------------
// Microsoft Test Automation Sources
//
// Copyright 2005 Microsoft Corporation. All Rights Reserved.
//
// WppDefs.h
//
// Collection: WDTF
// 
// Environment: User mode
//
// Primary Contact: WDTF Support (WDTFSupp@microsoft.com)
//
// History:
//   [Date]        -   [Status]
//   Jul 11 2005   -   Reviewed
//---------------------------------------------------------------------------
#pragma once


//
// Helpful Macros
//
#ifndef WIDEN2
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#endif


#define WDTF_Guid        (6210f559,c7f7,4d2f,b674,4bc9315cecc7)

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(WDTF, WDTF_Guid, \
        WPP_DEFINE_BIT(WDTFCOMEntryExit) \
        WPP_DEFINE_BIT(WDTFCOMError) \
        WPP_DEFINE_BIT(WDTFEntryExit) \
        WPP_DEFINE_BIT(WDTFAPICall) \
        WPP_DEFINE_BIT(WDTFInfo) \
        WPP_DEFINE_BIT(WDTFWarning) \
        WPP_DEFINE_BIT(WDTFError) \
        \
        WPP_DEFINE_BIT(WDTFNoisyCOMEntryExit) \
        WPP_DEFINE_BIT(WDTFNoisyEntryExit) \
        WPP_DEFINE_BIT(WDTFNoisyAPICall) \
        WPP_DEFINE_BIT(WDTFNoisyInfo) \
        WPP_DEFINE_BIT(WDTFNoisyWarning))



// Define an enum of shift values for bitmasks
#define WPP_DEFINE_CONTROL_GUID(name,guid,bits) bits
#define WPP_DEFINE_BIT(Bit) Bit##__shift,
enum __TracingBitShifts
{
    WPP_CONTROL_GUIDS
};
#undef WPP_DEFINE_BIT
#undef WPP_DEFINE_CONTROL_GUID



// Define an enum of bitmasks corresponding to WPP bits
#define WPP_DEFINE_CONTROL_GUID(name,guid,bits) bits
#define WPP_DEFINE_BIT(Bit) Bit##_Bit = 1 << Bit##__shift,
enum __TracingBits
{
    WPP_CONTROL_GUIDS
};
#undef WPP_DEFINE_BIT
#undef WPP_DEFINE_CONTROL_GUID


#define IS_LEVEL_ENABLED(LEVEL) \
    ((__pCtx->pTracingSettings->IsEnabled(LEVEL##_Bit)) && WPP_LEVEL_ENABLED(LEVEL))


#define INDENT_STR(indent) \
    (__indentSpacer + ((sizeof(__indentSpacer) >= -3+(indent)*5) ? (sizeof(__indentSpacer)+3-(indent)*5) : 0))



// begin_wpp config 
// CUSTOM_TYPE(SYSTEM_POWER_STATE, ItemEnum(_SYSTEM_POWER_STATE) );
// end_wpp



//MACRO: TraceError
//
//begin_wpp config
//USEPREFIX (TraceError, "%!STDPREFIX!%sERROR:", INDENT_STR(__pCtx->CallDepth+1));
//FUNC TraceError{TRACEERRORLEVEL=Error}(COMPNAME, MSG, ...);
//WPP_FLAGS(-public:TraceError);
//end_wpp
#define WPP_TRACEERRORLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_TRACEERRORLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: TraceAPI
//
//begin_wpp config
//USEPREFIX (TraceAPI, "%!STDPREFIX!%sAPI:", INDENT_STR(__pCtx->CallDepth+1));
//FUNC TraceAPI{TRACEAPILEVEL=APICall}(COMPNAME, MSG, ...);
//WPP_FLAGS(-public:TraceAPI);
//end_wpp
#define WPP_TRACEAPILEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_TRACEAPILEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: TraceInfo
//
//begin_wpp config
//USEPREFIX (TraceInfo, "%!STDPREFIX!%sINFO:", INDENT_STR(__pCtx->CallDepth+1));
//FUNC TraceInfo{TRACEINFOLEVEL=Info}(COMPNAME, MSG, ...);
//WPP_FLAGS(-public:TraceInfo);
//end_wpp
#define WPP_TRACEINFOLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_TRACEINFOLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: TraceWarning
//
//begin_wpp config
//USEPREFIX (TraceWarning, "%!STDPREFIX!%sWARN:", INDENT_STR(__pCtx->CallDepth+1));
//FUNC TraceWarning{TRACEWARNLEVEL=Warning}(COMPNAME, MSG, ...);
//WPP_FLAGS(-public:TraceWarning);
//end_wpp
#define WPP_TRACEWARNLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_TRACEWARNLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: TraceCOMEntry
//
//begin_wpp config
//USEPREFIX (TraceCOMEntry, "%!STDPREFIX!%so->this(%p):%!FUNC!(", INDENT_STR(__pCtx->CallDepth), this);
//FUNC TraceCOMEntry{COMENTRYLEVEL=COMEntryExit}(COMPNAME, MSG, ...);
//USESUFFIX (TraceCOMEntry, ")");
//WPP_FLAGS(-public:TraceCOMEntry);
//end_wpp
#define WPP_COMENTRYLEVEL_COMPNAME_PRE(LEVEL, comp) OVERRIDE_TRACING_SETTINGS(&m_TracingSettings);
#define WPP_COMENTRYLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMENTRYLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: COMReturn
//
//begin_wpp config
//USEPREFIX (COMReturn, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT!", INDENT_STR(__pCtx->CallDepth), this, __hResult);
//FUNC COMReturn{COMRETURNLEVEL=COMEntryExit}(COMPNAME, HR);
//WPP_FLAGS(-public:COMReturn);
//end_wpp
//#define WPP_COMRETURNLEVEL_PRE(LEVEL)
#define WPP_COMRETURNLEVEL_COMPNAME_HR_ENABLED(LEVEL, comp, HR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMRETURNLEVEL_COMPNAME_HR_LOGGER(LEVEL, comp, HR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMRETURNLEVEL_COMPNAME_HR_PRE(LEVEL, comp, HR) { \
                                                                HRESULT __hResult = HR;
#define WPP_COMRETURNLEVEL_COMPNAME_HR_POST(LEVEL, comp, HR)    /*TraceMessage()*/; \
                                                                return __hResult; \
                                                            }

//MACRO: COMReturnProperty
//
//begin_wpp config
//USEPREFIX (COMReturnProperty, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT!, ", INDENT_STR(__pCtx->CallDepth), this, __hResult);
//FUNC COMReturnProperty{COMRETURNLEVEL=COMEntryExit}(COMPNAME, HR, MSG, ...);
//WPP_FLAGS(-public:COMReturnProperty);
//end_wpp
//#define WPP_COMRETURNLEVEL_PRE(LEVEL)
#define WPP_COMRETURNPROPERTYLEVEL_COMPNAME_HR_ENABLED(LEVEL, comp, HR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMRETURNPROPERTYLEVEL_COMPNAME_HR_LOGGER(LEVEL, comp, HR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMRETURNPROPERTYLEVEL_COMPNAME_HR_PRE(LEVEL, comp, HR) { \
                                                                        HRESULT __hResult = HR;
#define WPP_COMRETURNPROPERTYLEVEL_COMPNAME_HR_POST(LEVEL, comp, HR)    /*TraceMessage()*/; \
                                                                        return __hResult; \
                                                                    }




//MACRO: TraceMethodEntry
//
//begin_wpp config
//USEPREFIX (TraceMethodEntry, "%!STDPREFIX!%s-->this(%p):%!FUNC!(", INDENT_STR(__pCtx->CallDepth), this);
//FUNC TraceMethodEntry{METHODENTRYLEVEL=EntryExit}(COMPNAME, MSG, ...);
//USESUFFIX (TraceMethodEntry, ")");
//WPP_FLAGS(-public:TraceMethodEntry);
//end_wpp
#define WPP_METHODENTRYLEVEL_COMPNAME_PRE(LEVEL, comp) OVERRIDE_TRACING_SETTINGS(&m_TracingSettings);
#define WPP_METHODENTRYLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_METHODENTRYLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: TraceMethodEntryNoOverride
//
//begin_wpp config
//USEPREFIX (TraceMethodEntryNoOverride, "%!STDPREFIX!%s-->this(%p):%!FUNC!(", INDENT_STR(__pCtx->CallDepth), this);
//FUNC TraceMethodEntryNoOverride{METHODENTRYNOOVERRIDELEVEL=EntryExit}(COMPNAME, MSG, ...);
//USESUFFIX (TraceMethodEntryNoOverride, ")");
//WPP_FLAGS(-public:TraceMethodEntryNoOverride);
//end_wpp
#define WPP_METHODENTRYNOOVERRIDELEVEL_COMPNAME_PRE(LEVEL, comp) USE_DEFAULT_TRACING_SETTINGS;
#define WPP_METHODENTRYNOOVERRIDELEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_METHODENTRYNOOVERRIDELEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: MethodReturn
//
//begin_wpp config
//USEPREFIX (MethodReturn, "%!STDPREFIX!%s<--this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC MethodReturn{RETURNLEVEL=EntryExit}(COMPNAME, RES, MSG, ...);
//WPP_FLAGS(-public:MethodReturn);
//end_wpp
#define WPP_RETURNLEVEL_COMPNAME_RES_ENABLED(LEVEL, comp, RES) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_RETURNLEVEL_COMPNAME_RES_LOGGER(LEVEL, comp, RES) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_RETURNLEVEL_COMPNAME_RES_POST(LEVEL, comp, RES) ;return RES;


//MACRO: MethodReturnHRESULT
//
//begin_wpp config
//USEPREFIX (MethodReturnHRESULT, "%!STDPREFIX!%s<--this(%p):%!FUNC!(): %!HRESULT!", INDENT_STR(__pCtx->CallDepth), this, __hResult);
//FUNC MethodReturnHRESULT{METHODRETURNHRESULTLEVEL=EntryExit}(COMPNAME, HR);
//WPP_FLAGS(-public:MethodReturnHRESULT);
//end_wpp
//#define WPP_COMRETURNLEVEL_PRE(LEVEL)
#define WPP_METHODRETURNHRESULTLEVEL_COMPNAME_HR_ENABLED(LEVEL, comp, HR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_METHODRETURNHRESULTLEVEL_COMPNAME_HR_LOGGER(LEVEL, comp, HR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_METHODRETURNHRESULTLEVEL_COMPNAME_HR_PRE(LEVEL, comp, HR) { \
                                                                          HRESULT __hResult = HR;
#define WPP_METHODRETURNHRESULTLEVEL_COMPNAME_HR_POST(LEVEL, comp, HR)    /*TraceMessage()*/; \
                                                                          return __hResult; \
                                                                      }


//MACRO: MethodReturnVoid
//
//begin_wpp config
//USEPREFIX (MethodReturnVoid, "%!STDPREFIX!%s<--this(%p):%!FUNC!()", INDENT_STR(__pCtx->CallDepth), this);
//FUNC MethodReturnVoid{RETURNVOIDLEVEL=EntryExit}(COMPNAME, ...);
//WPP_FLAGS(-public:MethodReturnVoid);
//end_wpp
#define WPP_RETURNVOIDLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_RETURNVOIDLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_RETURNVOIDLEVEL_COMPNAME_POST(LEVEL, comp) ;return;



//MACRO: MethodReturnBool
//
//begin_wpp config
//USEPREFIX (MethodReturnBool, "%!STDPREFIX!%s<--this(%p):%!FUNC!(): %!bool!", INDENT_STR(__pCtx->CallDepth), this, __boolRetVal);
//FUNC MethodReturnBool{RETURNVOIDLEVEL=EntryExit}(COMPNAME, BOOLRETVAL);
//WPP_FLAGS(-public:MethodReturnBool);
//end_wpp
#define WPP_RETURNVOIDLEVEL_COMPNAME_BOOLRETVAL_ENABLED(LEVEL, comp, boolretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_RETURNVOIDLEVEL_COMPNAME_BOOLRETVAL_LOGGER(LEVEL, comp, boolretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_RETURNVOIDLEVEL_COMPNAME_BOOLRETVAL_PRE(LEVEL, comp, boolretval)    { \
                                                                                    bool __boolRetVal = (boolretval ? true : false);
#define WPP_RETURNVOIDLEVEL_COMPNAME_BOOLRETVAL_POST(LEVEL, comp, boolretval)       /*TraceMessage()*/; \
                                                                                    return __boolRetVal; \
                                                                                }



//MACRO: MethodReturnPtr
//
//begin_wpp config
//USEPREFIX (MethodReturnPtr, "%!STDPREFIX!%s<--this(%p):%!FUNC!(): %p", INDENT_STR(__pCtx->CallDepth), this, __ptrRetVal);
//FUNC MethodReturnPtr{RETURNPTRLEVEL=EntryExit}(COMPNAME, PTRTYPE, PTRRETVAL);
//WPP_FLAGS(-public:MethodReturnPtr);
//end_wpp
#define WPP_RETURNPTRLEVEL_COMPNAME_PTRTYPE_PTRRETVAL_ENABLED(LEVEL, comp, PtrType,  ptrretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_RETURNPTRLEVEL_COMPNAME_PTRTYPE_PTRRETVAL_LOGGER(LEVEL, comp, PtrType, ptrretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_RETURNPTRLEVEL_COMPNAME_PTRTYPE_PTRRETVAL_PRE(LEVEL, comp, PtrType, ptrretval)  { \
                                                                                                PtrType __ptrRetVal = ptrretval;
#define WPP_RETURNPTRLEVEL_COMPNAME_PTRTYPE_PTRRETVAL_POST(LEVEL, comp, PtrType, ptrretval)     /*TraceMessage()*/; \
                                                                                                return __ptrRetVal; \
                                                                                            }


//MACRO: MethodReturnVariant
//
//begin_wpp config
//USEPREFIX (MethodReturnVariant, "%!STDPREFIX!%s<--this(%p):%!FUNC!(): %!VARIANT!", INDENT_STR(__pCtx->CallDepth), this, __vtRetVal);
//FUNC MethodReturnVariant{RETURNVARIANTLEVEL=EntryExit}(COMPNAME, VARIANTRETVAL);
//WPP_FLAGS(-public:MethodReturnVariant);
//end_wpp
#define WPP_RETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_ENABLED(LEVEL, comp, variantretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_RETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_LOGGER(LEVEL, comp, variantretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_RETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_PRE(LEVEL, comp, variantretval)   { \
                                                                                            VARIANT __vtRetVal = variantretval;
#define WPP_RETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_POST(LEVEL, comp, variantretval)      /*TraceMessage()*/; \
                                                                                            return __vtRetVal; \
                                                                                        }

//MACRO: MethodReturnIfNull
//
//begin_wpp config
//USEPREFIX (MethodReturnIfNull, "%!STDPREFIX!%s<-this(%p):%!FUNC!(): E_POINTER %s=NULL, bailing out!", INDENT_STR(__pCtx->CallDepth), this, #PTR);
//FUNC MethodReturnIfNull{METHOD_POINTER_LEVEL=EntryExit}(COMPNAME, PTR);
//WPP_FLAGS(-public:MethodReturnIfNull);
//end_wpp
#define WPP_METHOD_POINTER_LEVEL_COMPNAME_PTR_ENABLED(LEVEL, comp, PTR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_METHOD_POINTER_LEVEL_COMPNAME_PTR_LOGGER(LEVEL, comp, PTR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_METHOD_POINTER_LEVEL_COMPNAME_PTR_PRE(LEVEL, comp, PTR)  if ((PTR) == NULL) \
                                                                     {
#define WPP_METHOD_POINTER_LEVEL_COMPNAME_PTR_POST(LEVEL, comp, PTR)     /*TraceMessage()*/; \
                                                                         return E_POINTER; \
                                                                     }

//MACRO: TraceFunctionEntry
//
//begin_wpp config
//USEPREFIX (TraceFunctionEntry, "%!STDPREFIX!%s-->%!FUNC!(", INDENT_STR(__pCtx->CallDepth));
//FUNC TraceFunctionEntry{FUNCENTRYLEVEL=EntryExit}(COMPNAME, MSG, ...);
//USESUFFIX (TraceFunctionEntry, ")");
//WPP_FLAGS(-public:TraceFunctionEntry);
//end_wpp
#define WPP_FUNCENTRYLEVEL_COMPNAME_PRE(LEVEL, comp) USE_DEFAULT_TRACING_SETTINGS;
#define WPP_FUNCENTRYLEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCENTRYLEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)


//MACRO: FunctionReturn
//
//begin_wpp config
//USEPREFIX (FunctionReturn, "%!STDPREFIX!%s<--%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth));
//FUNC FunctionReturn{FUNCRETURNLEVEL=EntryExit}(COMPNAME, RES, MSG, ...);
//WPP_FLAGS(-public:FunctionReturn);
//end_wpp
#define WPP_FUNCRETURNLEVEL_COMPNAME_RES_ENABLED(LEVEL, comp, RES) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCRETURNLEVEL_COMPNAME_RES_LOGGER(LEVEL, comp, RES) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_FUNCRETURNLEVEL_COMPNAME_RES_POST(LEVEL, comp, RES) ;return RES;



//MACRO: FunctionReturnVoid
//
//begin_wpp config
//USEPREFIX (FunctionReturnVoid, "%!STDPREFIX!%s<--%!FUNC!()", INDENT_STR(__pCtx->CallDepth));
//FUNC FunctionReturnVoid{FUNCRETURNLEVELVOID=EntryExit}(COMPNAME, ...);
//WPP_FLAGS(-public:FunctionReturnVoid);
//end_wpp
#define WPP_FUNCRETURNLEVELVOID_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCRETURNLEVELVOID_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_FUNCRETURNLEVELVOID_COMPNAME_POST(LEVEL, comp) ;return;



//MACRO: FunctionReturnBool
//
//begin_wpp config
//USEPREFIX (FunctionReturnBool, "%!STDPREFIX!%s<--%!FUNC!(): %!bool!", INDENT_STR(__pCtx->CallDepth), __boolretval);
//FUNC FunctionReturnBool{FUNCRETURNLEVELBOOL=EntryExit}(COMPNAME, BOOLRETVAL);
//WPP_FLAGS(-public:FunctionReturnBool);
//end_wpp
#define WPP_FUNCRETURNLEVELBOOL_COMPNAME_BOOLRETVAL_ENABLED(LEVEL, comp, boolretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCRETURNLEVELBOOL_COMPNAME_BOOLRETVAL_LOGGER(LEVEL, comp, boolretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_FUNCRETURNLEVELBOOL_COMPNAME_BOOLRETVAL_PRE(LEVEL, comp, boolretval)    { \
                                                                                        bool __boolretval = (boolretval ? true : false);
#define WPP_FUNCRETURNLEVELBOOL_COMPNAME_BOOLRETVAL_POST(LEVEL, comp, boolretval)       /*TraceMessage()*/; \
                                                                                        return __boolretval; \
                                                                                    }



//MACRO: FunctionReturnPtr
//
//begin_wpp config
//USEPREFIX (FunctionReturnPtr, "%!STDPREFIX!%s<--%!FUNC!(): %p", INDENT_STR(__pCtx->CallDepth), __ptrRetVal);
//FUNC FunctionReturnPtr{FUNCRETURNLEVELPTR=EntryExit}(COMPNAME, PTRTYPE, PTRRETVAL);
//WPP_FLAGS(-public:FunctionReturnPtr);
//end_wpp
#define WPP_FUNCRETURNLEVELPTR_COMPNAME_PTRTYPE_PTRRETVAL_ENABLED(LEVEL, comp, PtrType, ptrretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCRETURNLEVELPTR_COMPNAME_PTRTYPE_PTRRETVAL_LOGGER(LEVEL, comp, PtrType, ptrretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_FUNCRETURNLEVELPTR_COMPNAME_PTRTYPE_PTRRETVAL_PRE(LEVEL, comp, PtrType, ptrretval)  { \
                                                                                                    PtrType __ptrRetVal = ptrretval;
#define WPP_FUNCRETURNLEVELPTR_COMPNAME_PTRTYPE_PTRRETVAL_POST(LEVEL, comp, PtrType, ptrretval)     /*TraceMessage()*/; \
                                                                                                    return __ptrRetVal; \
                                                                                                }


//MACRO: FunctionReturnVariant
//
//begin_wpp config
//USEPREFIX (FunctionReturnVariant, "%!STDPREFIX!%s<--%!FUNC!(): %!VARIANT!", INDENT_STR(__pCtx->CallDepth), __vtRetVal);
//FUNC FunctionReturnVariant{FUNCRETURNVARIANTLEVEL=EntryExit}(COMPNAME, VARIANTRETVAL);
//WPP_FLAGS(-public:FunctionReturnVariant);
//end_wpp
#define WPP_FUNCRETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_ENABLED(LEVEL, comp, variantretval) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_FUNCRETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_LOGGER(LEVEL, comp, variantretval) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_FUNCRETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_PRE(LEVEL, comp, variantretval)   { \
                                                                                                VARIANT __vtRetVal = variantretval;
#define WPP_FUNCRETURNVARIANTLEVEL_COMPNAME_VARIANTRETVAL_POST(LEVEL, comp, variantretval)      /*TraceMessage()*/; \
                                                                                                return __vtRetVal; \
                                                                                            }


//MACRO: COMReportError
//
//begin_wpp config
//USEPREFIX (COMReportError, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT! - Error %S", INDENT_STR(__pCtx->CallDepth), this, __hResult, __pszErrMsg);
//FUNC COMReportError{COMReportError=COMEntryExit}(COMPNAME, HR, ERRMSG);
//WPP_FLAGS(-public:COMReportError);
//end_wpp
#define WPP_COMReportError_COMPNAME_HR_ERRMSG_ENABLED(LEVEL, comp, HR, errmsg) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMReportError_COMPNAME_HR_ERRMSG_LOGGER(LEVEL, comp, HR, errmsg) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMReportError_COMPNAME_HR_ERRMSG_PRE(LEVEL, comp, HR, errmsg)  { \
                                                                                HRESULT __hResult = HR; \
                                                                                CString __strErrMsg(errmsg); \
                                                                                if(HRESULT_FACILITY(__hResult) == FACILITY_WIN32) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" Win32=%d", HRESULT_CODE(__hResult)); \
                                                                                } \
                                                                                else if((__hResult & FACILITY_NT_BIT) != 0) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" NTSTATUS=0x%X", __hResult & ~FACILITY_NT_BIT); \
                                                                                } \
                                                                                else \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" HRESULT=0x%X", __hResult); \
                                                                                } \
                                                                                \
                                                                                PWSTR __pszSysMsg = NULL; \
                                                                                DWORD __dwFmtResult; \
                                                                                __dwFmtResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
                                                                                                              FORMAT_MESSAGE_IGNORE_INSERTS | \
                                                                                                              FORMAT_MESSAGE_FROM_SYSTEM, \
                                                                                                              NULL, \
                                                                                                              __hResult, \
                                                                                                              0, \
                                                                                                              (LPWSTR)&__pszSysMsg, \
                                                                                                              0, \
                                                                                                              NULL); \
                                                                                if(__dwFmtResult != 0 && __hResult != E_FAIL) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" - %s", __pszSysMsg); \
                                                                                } \
                                                                                if(__pszSysMsg != NULL) \
                                                                                { \
                                                                                    LocalFree(__pszSysMsg); \
                                                                                } \
                                                                                LPCOLESTR __pszErrMsg = __strErrMsg;
#define WPP_COMReportError_COMPNAME_HR_ERRMSG_POST(LEVEL, comp, HR, errmsg)     /*TraceMessage()*/; \
                                                                                return AtlReportError(GetObjectCLSID(), \
                                                                                    __pszErrMsg, GUID_NULL, __hResult); \
                                                                            }
 

//MACRO: COMReportErrorIfFailed
//
//begin_wpp config
//USEPREFIX (COMReportErrorIfFailed, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT! - Error %S", INDENT_STR(__pCtx->CallDepth), this, __hResult, __pszErrMsg);
//FUNC COMReportErrorIfFailed{COMReportError=COMEntryExit}(COMPNAME, HR, ERRMSG);
//WPP_FLAGS(-public:COMReportErrorIfFailed);
//end_wpp
#define WPP_COMReportErrorIfFailed_COMPNAME_HR_ERRMSG_ENABLED(LEVEL, comp, HR, errmsg) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMReportErrorIfFailed_COMPNAME_HR_ERRMSG_LOGGER(LEVEL, comp, HR, errmsg) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMReportErrorIfFailed_COMPNAME_HR_ERRMSG_PRE(LEVEL, comp, HR, errmsg)  { \
                                                                                HRESULT __hResult = HR; \
                                                                                if (FAILED(__hResult)) \
                                                                                {\
                                                                                    CString __strErrMsg(errmsg); \
                                                                                    if(HRESULT_FACILITY(__hResult) == FACILITY_WIN32) \
                                                                                    { \
                                                                                        __strErrMsg.AppendFormat(L" Win32=%d", HRESULT_CODE(__hResult)); \
                                                                                    } \
                                                                                    else if((__hResult & FACILITY_NT_BIT) != 0) \
                                                                                    { \
                                                                                        __strErrMsg.AppendFormat(L" NTSTATUS=0x%X", __hResult & ~FACILITY_NT_BIT); \
                                                                                    } \
                                                                                    else \
                                                                                    { \
                                                                                        __strErrMsg.AppendFormat(L" HRESULT=0x%X", __hResult); \
                                                                                    } \
                                                                                    \
                                                                                    PWSTR __pszSysMsg = NULL; \
                                                                                    DWORD __dwFmtResult; \
                                                                                    __dwFmtResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
                                                                                                                  FORMAT_MESSAGE_IGNORE_INSERTS | \
                                                                                                                  FORMAT_MESSAGE_FROM_SYSTEM, \
                                                                                                                  NULL, \
                                                                                                                  __hResult, \
                                                                                                                  0, \
                                                                                                                  (LPWSTR)&__pszSysMsg, \
                                                                                                                  0, \
                                                                                                                  NULL); \
                                                                                    if(__dwFmtResult != 0 && __hResult != E_FAIL) \
                                                                                    { \
                                                                                        __strErrMsg.AppendFormat(L" - %s", __pszSysMsg); \
                                                                                    } \
                                                                                    if(__pszSysMsg != NULL) \
                                                                                    { \
                                                                                        LocalFree(__pszSysMsg); \
                                                                                    } \
                                                                                    LPCOLESTR __pszErrMsg = __strErrMsg;
    #define WPP_COMReportErrorIfFailed_COMPNAME_HR_ERRMSG_POST(LEVEL, comp, HR, errmsg)     /*TraceMessage()*/; \
                                                                                    return AtlReportError(GetObjectCLSID(), \
                                                                                        __pszErrMsg, GUID_NULL, __hResult); \
                                                                                }\
                                                                            }

//MACRO: COMReportErrorAndLog
//
//begin_wpp config
//USEPREFIX (COMReportErrorAndLog, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT! - Error %S", INDENT_STR(__pCtx->CallDepth), this, __hResult, __pszErrMsg);
//FUNC COMReportErrorAndLog{COMReportErrorAndLog=COMEntryExit}(COMPNAME, HR, ERRMSG);
//WPP_FLAGS(-public:COMReportErrorAndLog);
//end_wpp
#define WPP_COMReportErrorAndLog_COMPNAME_HR_ERRMSG_ENABLED(LEVEL, comp, HR, errmsg) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMReportErrorAndLog_COMPNAME_HR_ERRMSG_LOGGER(LEVEL, comp, HR, errmsg) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMReportErrorAndLog_COMPNAME_HR_ERRMSG_PRE(LEVEL, comp, HR, errmsg)  { \
                                                                                HRESULT __hResult = HR; \
                                                                                CString __strErrMsg(errmsg); \
                                                                                if(HRESULT_FACILITY(__hResult) == FACILITY_WIN32) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" Win32=%d", HRESULT_CODE(__hResult)); \
                                                                                } \
                                                                                else if((__hResult & FACILITY_NT_BIT) != 0) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" NTSTATUS=0x%X", __hResult & ~FACILITY_NT_BIT); \
                                                                                } \
                                                                                else \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" HRESULT=0x%X", __hResult); \
                                                                                } \
                                                                                \
                                                                                PWSTR __pszSysMsg = NULL; \
                                                                                DWORD __dwFmtResult; \
                                                                                __dwFmtResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
                                                                                                              FORMAT_MESSAGE_IGNORE_INSERTS | \
                                                                                                              FORMAT_MESSAGE_FROM_SYSTEM, \
                                                                                                              NULL, \
                                                                                                              __hResult, \
                                                                                                              0, \
                                                                                                              (LPWSTR)&__pszSysMsg, \
                                                                                                              0, \
                                                                                                              NULL); \
                                                                                if(__dwFmtResult != 0 && __hResult != E_FAIL) \
                                                                                { \
                                                                                    __strErrMsg.AppendFormat(L" - %s", __pszSysMsg); \
                                                                                } \
                                                                                if(__pszSysMsg != NULL) \
                                                                                { \
                                                                                    LocalFree(__pszSysMsg); \
                                                                                } \
                                                                                LPCOLESTR __pszErrMsg = __strErrMsg;
#define WPP_COMReportErrorAndLog_COMPNAME_HR_ERRMSG_POST(LEVEL, comp, HR, errmsg)     /*TraceMessage()*/; \
	                                                                            m_Log.OutputError(L"%s\n",__pszErrMsg); \
                                                                                return AtlReportError(GetObjectCLSID(), \
                                                                                    __pszErrMsg, GUID_NULL, __hResult); \
                                                                            }





//
//  Return Error macros
//
//  The macros return the HRESULT bit do not report an error threw AtlReportError()
//


//MACRO: COMReturnIfDestroyed
//
//begin_wpp config
//USEPREFIX (COMReturnIfDestroyed, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): E_UNEXPECTED The called object was destroyed when someone called WDTF::Destroy(). Please dispose of this object, instantiate a new WDTF, and try again.", INDENT_STR(__pCtx->CallDepth), this);
//FUNC COMReturnIfDestroyed{COMReturnIfDestroyed=COMEntryExit}(COMPNAME);
//WPP_FLAGS(-public:COMReturnIfDestroyed);
//end_wpp
#define WPP_COMReturnIfDestroyed_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_COMReturnIfDestroyed_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_COMReturnIfDestroyed_COMPNAME_PRE(LEVEL, comp)    if (m_bDestroyed) \
                                                              {
#define WPP_COMReturnIfDestroyed_COMPNAME_POST(LEVEL, comp)       /*TraceMessage()*/; \
                                                                  return AtlReportError(GetObjectCLSID(), \
                                                                            L"The called object was destroyed when someone called WDTF::Destroy(). Please dispose of this object, instantiate a new WDTF, and try again.", GUID_NULL, E_UNEXPECTED); \
                                                              }

//MACRO: COMReturnIfNull
//
//begin_wpp config
//USEPREFIX (COMReturnIfNull, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): E_POINTER %s=NULL, bailing out!", INDENT_STR(__pCtx->CallDepth), this, #PTR);
//FUNC COMReturnIfNull{E_POINTER_LEVEL=COMEntryExit}(COMPNAME, PTR);
//WPP_FLAGS(-public:COMReturnIfNull);
//end_wpp
#define WPP_E_POINTER_LEVEL_COMPNAME_PTR_ENABLED(LEVEL, comp, PTR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_E_POINTER_LEVEL_COMPNAME_PTR_LOGGER(LEVEL, comp, PTR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_E_POINTER_LEVEL_COMPNAME_PTR_PRE(LEVEL, comp, PTR)      if ((PTR) == NULL) \
                                                                   {
#define WPP_E_POINTER_LEVEL_COMPNAME_PTR_POST(LEVEL, comp, PTR)         /*TraceMessage()*/; \
                                                                       return AtlReportError(GetObjectCLSID(), \
                                                                           L"Argument Error " WIDEN(#PTR) L"=NULL, bailing out!", GUID_NULL, E_POINTER); \
                                                                   }


//MACRO: COMReportErrorAndLogIfNull
//
//begin_wpp config
//USEPREFIX (COMReportErrorAndLogIfNull, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): E_POINTER %s=NULL, bailing out!", INDENT_STR(__pCtx->CallDepth), this, #PTR);
//FUNC COMReportErrorAndLogIfNull{E_POINTER_LEVEL=COMEntryExit}(COMPNAME, PTR);
//WPP_FLAGS(-public:COMReturnErrorAndLogIfNull);
//end_wpp
#define WPP_E_POINTER_LEVEL_AND_LOG_COMPNAME_PTR_ENABLED(LEVEL, comp, PTR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_E_POINTER_LEVEL_AND_LOG_COMPNAME_PTR_LOGGER(LEVEL, comp, PTR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_E_POINTER_LEVEL_AND_LOG_COMPNAME_PTR_PRE(LEVEL, comp, PTR)      if ((PTR) == NULL) \
                                                                   {
#define WPP_E_POINTER_LEVEL_AND_LOG_COMPNAME_PTR_POST(LEVEL, comp, PTR)         /*TraceMessage()*/; \
                                                                       LPCOLESTR __pszErrMsg =  L"Argument Error " WIDEN(#PTR) L"=NULL, bailing out!"; \
                                                                       m_Log.OutputError(L"%s\n",__pszErrMsg); \
                                                                       return AtlReportError(GetObjectCLSID(), \
                                                                           __pszErrMsg, GUID_NULL, E_POINTER); \
                                                                   }

//MACRO: COMReturnIfFailed
//
//begin_wpp config
//USEPREFIX (COMReturnIfFailed, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %!HRESULT! Contained method failed: Bailing out!", INDENT_STR(__pCtx->CallDepth), this, __hr);
//FUNC COMReturnIfFailed{PROPAGATE_LEVEL=COMEntryExit}(COMPNAME, HR);
//WPP_FLAGS(-public:COMReturnIfFailed);
//end_wpp
#define WPP_PROPAGATE_LEVEL_COMPNAME_HR_ENABLED(LEVEL, comp, HR) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_PROPAGATE_LEVEL_COMPNAME_HR_LOGGER(LEVEL, comp, HR) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_PROPAGATE_LEVEL_COMPNAME_HR_PRE(LEVEL, comp, HR)      { \
                                                                      HRESULT __hr = HR; \
                                                                      if (FAILED(__hr)) \
                                                                      {
#define WPP_PROPAGATE_LEVEL_COMPNAME_HR_POST(LEVEL, comp, HR)             /*TraceMessage()*/; \
                                                                          return __hr; \
                                                                      } \
                                                                  }

// Return last Win32 error converted to HRESULT
//#define COMReturnLastWin32()                           \
//{                                                      \
//    HRESULT __hr = HRESULT_FROM_WIN32(GetLastError()); \
//    return __hr;                                       \
//}

//MACRO: COMReturnLastWin32
//
//begin_wpp config
//USEPREFIX (COMReturnLastWin32, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC COMReturnLastWin32{LASTWIN32_LEVEL=COMEntryExit}(COMPNAME, ...);
//USESUFFIX (COMReturnLastWin32, "%!HRESULT!", __hr);
//WPP_FLAGS(-public:COMReturnLastWin32);
//end_wpp
#define WPP_LASTWIN32_LEVEL_COMPNAME_ENABLED(LEVEL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LASTWIN32_LEVEL_COMPNAME_LOGGER(LEVEL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LASTWIN32_LEVEL_COMPNAME_PRE(LEVEL, comp)   { \
                                                            HRESULT __hr = HRESULT_FROM_WIN32(GetLastError());
#define WPP_LASTWIN32_LEVEL_COMPNAME_POST(LEVEL, comp)      /*TraceMessage()*/; \
                                                            return __hr; \
                                                        }



// Return NTSTATUS converted to HRESULT if it is not STATUS_SUCCESS
//#define RETURN_IF_NOT_SUCCESS(status)        \
//{                                            \
//    NTSTATUS st = status;                    \
//    if (st != STATUS_SUCCESS)                \
//    {                                        \
//        return HRESULT_FROM_NT(st);          \
//    }                                        \
//}

//MACRO: COMReturnIfNotSuccess
//
//begin_wpp config
//USEPREFIX (COMReturnIfNotSuccess, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC COMReturnIfNotSuccess{NTSTATUS_LEVEL=COMEntryExit}(COMPNAME, status);
//USESUFFIX (COMReturnIfNotSuccess, "%!HRESULT!", HRESULT_FROM_NT(__status));
//WPP_FLAGS(-public:COMReturnIfNotSuccess);
//end_wpp
#define WPP_NTSTATUS_LEVEL_COMPNAME_status_ENABLED(LEVEL, comp, status) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_NTSTATUS_LEVEL_COMPNAME_status_LOGGER(LEVEL, comp, status) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_NTSTATUS_LEVEL_COMPNAME_status_PRE(LEVEL, comp, status) { \
                                                                        NTSTATUS __status = status; \
                                                                        if (__status != STATUS_SUCCESS) \
                                                                        {
#define WPP_NTSTATUS_LEVEL_COMPNAME_status_POST(LEVEL, comp, status)        /*TraceMessage()*/; \
                                                                            return HRESULT_FROM_NT(__status); \
                                                                        } \
                                                                    }



//MACRO: DEFINE_CHECK
//
//begin_wpp config
//FUNC DEFINE_CHECK{DEFINE_CHECK_LEVEL=COMEntryExit}(COMPNAME, decl, init, ret);
//WPP_FLAGS(-public:DEFINE_CHECK);
//end_wpp
#define WPP_DEFINE_CHECK_LEVEL_COMPNAME_decl_init_ret_ENABLED(LEVEL, comp, decl, init, ret) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_DEFINE_CHECK_LEVEL_COMPNAME_decl_init_ret_LOGGER(LEVEL, comp, decl, init, ret) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_DEFINE_CHECK_LEVEL_COMPNAME_decl_init_ret_PRE(LEVEL, comp, decl, init, ret) decl; 
#define WPP_DEFINE_CHECK_LEVEL_COMPNAME_decl_init_ret_POST(LEVEL, comp, decl, init, ret)        /*TraceMessage()*/; \
                                                                        if (init) \
                                                                        { \
                                                                            return ret; \
                                                                        } \


// Define Non-empty debug break for checked builds only

// TODO: Having a debugbreak that happens at regsvr32 time breaks registration
#ifndef NDEBUG
    #define DEBUG_BREAK() do {} while (false)
#else
    #define DEBUG_BREAK() do {} while (false)
#endif


//ASSERT macro with tracing
//MACRO: TraceASSERT
//
//begin_wpp config
//USEPREFIX (TraceASSERT, "%!STDPREFIX!%sWARN: ASSERTION FAILED - expression \"%s\" is false.", INDENT_STR(__pCtx->CallDepth+1), #expr);
//FUNC TraceASSERT{ASSERTLEVEL=Warning}(COMPNAME, expr);
//WPP_FLAGS(-public:TraceASSERT);
//end_wpp
#define WPP_ASSERTLEVEL_COMPNAME_expr_ENABLED(LEVEL, comp, expr) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_ASSERTLEVEL_COMPNAME_expr_LOGGER(LEVEL, comp, expr) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_ASSERTLEVEL_COMPNAME_expr_PRE(LEVEL, comp, expr)    if (!(expr)) \
                                                                {    
#define WPP_ASSERTLEVEL_COMPNAME_expr_POST(LEVEL, comp, expr)       /*TraceMessage()*/; \
                                                                    DEBUG_BREAK(); \
                                                                }


//MACRO: Throw
//
// Trace and throw an exception.
//
//begin_wpp config
//USEPREFIX (Throw, "%!STDPREFIX!%sFATAL ERROR: throwing %s", INDENT_STR(__pCtx->CallDepth+1), #EXCEPTION);
//FUNC Throw{LEVEL=Error}(COMPNAME, EXCEPTION, ...);
//WPP_FLAGS(-public:Throw);
//end_wpp
#define WPP_LEVEL_COMPNAME_EXCEPTION_ENABLED(LEVEL, comp, __exception) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EXCEPTION_LOGGER(LEVEL, comp, __exception) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EXCEPTION_POST(LEVEL, comp, __exception) ; DEBUG_BREAK(); throw __exception;



//MACRO: ThrowEFatal
//
// Trace and throw an EFatal exception.
//
//begin_wpp config
//USEPREFIX (ThrowEFatal, "%!STDPREFIX!%sFATAL ERROR: ", INDENT_STR(__pCtx->CallDepth+1));
//FUNC ThrowEFatal{LEVEL=Error, EFATAL=0, COMPNAME=WDTF}(MSG, ...);
//WPP_FLAGS(-public:ThrowEFatal);
//end_wpp
#define WPP_LEVEL_EFATAL_COMPNAME_ENABLED(LEVEL, EFATAL, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_EFATAL_COMPNAME_LOGGER(LEVEL, EFATAL, comp) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_EFATAL_COMPNAME_POST(LEVEL, EFATAL, comp) ; DEBUG_BREAK(); throw EFatal(__WFILE__, __LINE__, L"See ETL log for details. Please provide this info to WDTF Support.");



//TODO: what help context?
//MACRO: ThrowEInternal
//
// Trace and throw an EInternal exception.
//
//begin_wpp config
//USEPREFIX (ThrowEInternal, "%!STDPREFIX!%sINTERNAL ERROR: \"%S\"", INDENT_STR(__pCtx->CallDepth+1), DESCRIPTION);
//FUNC ThrowEInternal{LEVEL=Error, COMPNAME=WDTF}(DESCRIPTION);
//WPP_FLAGS(-public:ThrowEInternal);
//end_wpp
#define WPP_LEVEL_COMPNAME_DESCRIPTION_ENABLED(LEVEL, comp, descr) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_DESCRIPTION_LOGGER(LEVEL, comp, descr) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_DESCRIPTION_POST(LEVEL, comp, descr) ; DEBUG_BREAK(); throw EInternal(__WFILE__, __LINE__, descr, 0);


// begin_wpp config
// CUSTOM_TYPE(EParse_TCode, ItemEnum(EParse::_TCode) );
// end_wpp

//MACRO: ThrowEParse
//
// Trace and throw an EParse exception.
//
//begin_wpp config
//USEPREFIX (ThrowEParse, "%!STDPREFIX!%sPARSE ERROR: \"%!EParse_TCode!\"", INDENT_STR(__pCtx->CallDepth+1), EParse::TCODE);
//FUNC ThrowEParse{LEVEL=Error, COMPNAME=WDTF}(TCODE);
//WPP_FLAGS(-public:ThrowEParse);
//end_wpp
#define WPP_LEVEL_COMPNAME_TCODE_ENABLED(LEVEL, comp, tcode) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_TCODE_LOGGER(LEVEL, comp, tcode) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_TCODE_POST(LEVEL, comp, tcode) ; throw EParse(__WFILE__, __LINE__, EParse::tcode);




//MACRO: ReportEParse
//
// Handle and trace an EParse exception. Then convert it into an
// IErrorInfo (as E_INVALIDARG) using AtlReportError.
//
//begin_wpp config
//USEPREFIX (ReportEParse, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): %S ", INDENT_STR(__pCtx->CallDepth), this, strFull.c_str());
//FUNC ReportEParse{LEVEL=COMError, COMPNAME=WDTF}(EPARSE);
//USESUFFIX (ReportEParse, "Returning (E_INVALIDARG)");
//WPP_FLAGS(-public:ReportEParse);
//end_wpp
#define WPP_LEVEL_COMPNAME_EPARSE_ENABLED(LEVEL, comp, EPARSE) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EPARSE_LOGGER(LEVEL, comp, EPARSE) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EPARSE_PRE(LEVEL, comp, EPARSE)                             \
{                                                                       \
    wstring strSDEL = EPARSE.GetQuery();                                \
                                                                        \
    wstring strFull = L"Error Parsing Argument: ";                      \
                                                                        \
    strFull += EPARSE.GetDescription(); strFull += L" SDEL(";           \
    strFull += strSDEL.substr(0, EPARSE.GetParsed()).c_str();           \
    strFull += L"<-NEAR HERE->";                                        \
    strFull += strSDEL.substr(EPARSE.GetParsed()).c_str();              \
    strFull += L")";                                                    

#define WPP_LEVEL_COMPNAME_EPARSE_POST(LEVEL, comp, EPARSE)    /*TraceMessage(...)*/;  \
    /*DebugBreak();*/                                                   \
      return AtlReportError(GetObjectCLSID(),                           \
        strFull.c_str(),                                                \
        GUID_NULL, E_INVALIDARG);                                       \
}


//MACRO: ThrowEFieldData
//
// Trace and throw an ThrowEFieldData exception.
//
//begin_wpp config
//USEPREFIX (ThrowEFieldData, "%!STDPREFIX!%sERROR Field Error : ",INDENT_STR(__pCtx->CallDepth+1));
//FUNC ThrowEFieldData{LEVEL=Error, COMPNAME=WDTF}(NODEUNIQUEID,NAMESPACENAME,FIELDNAME,ERRORCODE);
//WPP_FLAGS(-public:ThrowEFieldData);
//end_wpp
#define WPP_LEVEL_COMPNAME_NODEUNIQUEID_NAMESPACENAME_FIELDNAME_ERRORCODE_ENABLED(LEVEL, comp, nodeuniqueid , namespacename,fieldname,err) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_NODEUNIQUEID_NAMESPACENAME_FIELDNAME_ERRORCODE_LOGGER(LEVEL, comp, nodeuniqueid , namespacename,fieldname,err) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_NODEUNIQUEID_NAMESPACENAME_FIELDNAME_ERRORCODE_POST(LEVEL, comp, nodeuniqueid , namespacename,fieldname,err) ; throw EDataField(__WFILE__, __LINE__, (PCWSTR)nodeuniqueid , (PCWSTR)namespacename,(PCWSTR)fieldname,err);


//MACRO: ReportEFieldData
//
// Handle and trace an EDataField exception. Then convert it into an
// IErrorInfo (as E_INVALIDARG) using AtlReportError.
//
//begin_wpp config
//USEPREFIX (ReportEFieldData, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC ReportEFieldData{LEVEL=COMError, COMPNAME=WDTF}(EDATA);
//USESUFFIX (ReportEFieldData, "Returning (E_INVALIDARG)");
//WPP_FLAGS(-public:ReportEFieldData);
//end_wpp
#define WPP_LEVEL_COMPNAME_EDATA_ENABLED(LEVEL, comp, EDATA) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EDATA_LOGGER(LEVEL, comp, EDATA) WPP_LEVEL_LOGGER(comp##LEVEL)
#define WPP_LEVEL_COMPNAME_EDATA_PRE(LEVEL, comp, EDATA)                             \
{                                                                        \
    ULONG __nLastError = EDATA.GetLastError();                           \
    HRESULT __hr = HRESULT_FROM_SETUPAPI(__nLastError);                  \
    CString __strErrMsg;                                                 \
    __strErrMsg.Format(L" %s  : Target=\"%s\" NameSpace=\"%s\" Field Name=\"%s\" Win32 Error Code=%d",   \
        EDATA.GetDescription(),                                          \
        EDATA.GetNodeUniqueID(),                                         \
        EDATA.GetNameSpace(),                                            \
        EDATA.GetFieldName(),                                            \
        HRESULT_CODE(__hr));    \
     PWSTR __pszSysMsg = NULL; \
     DWORD __dwFmtResult; \
     __dwFmtResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
                                    FORMAT_MESSAGE_IGNORE_INSERTS | \
                                    FORMAT_MESSAGE_FROM_SYSTEM, \
                                    NULL, \
                                    __hr, \
                                    0, \
                                    (LPWSTR)&__pszSysMsg, \
                                    0, \
                                    NULL); \
    if(__dwFmtResult != 0 ) \
    { \
        __strErrMsg.AppendFormat(L" - %s", __pszSysMsg); \
    } \
    if(__pszSysMsg != NULL) \
    { \
        LocalFree(__pszSysMsg); \
    } 


#define WPP_LEVEL_COMPNAME_EDATA_POST(LEVEL, comp, EDATA)    /*TraceMessage(...)*/;  \
    /*DebugBreak();*/                                                   \
      return AtlReportError(GetObjectCLSID(),                           \
        __strErrMsg, GUID_NULL, __hr);                   \
}



//MACRO: ReportEMemory
//
// Handle and trace a std::bad_alloc exception. Then convert it into an
// IErrorInfo (as E_OUTOFMEMORY) using AtlReportError.
//
//begin_wpp config
//USEPREFIX (ReportEMemory, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC ReportEMemory{LEVEL=COMError, MEM=0, COMPNAME=WDTF}(...);
//USESUFFIX (ReportEMemory, "Out Of Memory Error! Returning (E_OUTOFMEMORY)");
//WPP_FLAGS(-public:ReportEMemory);
//end_wpp
#define WPP_LEVEL_MEM_COMPNAME_ENABLED(LEVEL, MEM, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_MEM_COMPNAME_LOGGER(LEVEL, MEM, comp) WPP_LEVEL_LOGGER(comp##LEVEL)

#define WPP_LEVEL_MEM_COMPNAME_PRE(LEVEL, MEM, comp) 
#define WPP_LEVEL_MEM_COMPNAME_POST(LEVEL, MEM, comp)      /*TraceMessage(...)*/;                               \
                                                DebugBreak();                                                   \
                                                return AtlReportError(GetObjectCLSID(),                         \
                                                    L"Out Of Memory Error! Perhaps try the operation again?",   \
                                                    GUID_NULL, E_OUTOFMEMORY);



//MACRO: ReportEFail
//
// Handle and trace the '...' exception. Then convert it into an
// IErrorInfo (as E_FAIL) using AtlReportError.
//
//
//begin_wpp config
//USEPREFIX (ReportEFail, "%!STDPREFIX!%so<-this(%p):%!FUNC!(): ", INDENT_STR(__pCtx->CallDepth), this);
//FUNC ReportEFail{LEVEL=COMError, FAIL=0, COMPNAME=WDTF}(...);
//USESUFFIX (ReportEFail, "General Failure! Returning (E_FAIL)");
//WPP_FLAGS(-public:ReportEFail);
//end_wpp
#define WPP_LEVEL_FAIL_COMPNAME_ENABLED(LEVEL, GEN, comp) IS_LEVEL_ENABLED(comp##LEVEL)
#define WPP_LEVEL_FAIL_COMPNAME_LOGGER(LEVEL, GEN, comp) WPP_LEVEL_LOGGER(comp##LEVEL)

#define WPP_LEVEL_FAIL_COMPNAME_PRE(LEVEL, GEN, comp) 
#define WPP_LEVEL_FAIL_COMPNAME_POST(LEVEL, GEN, comp)     /*TraceMessage(...)*/;                       \
                                                return AtlReportError(GetObjectCLSID(),                 \
                                                    L"General Failure! See the WPP log for more info.", \
                                                    GUID_NULL, E_FAIL);





// begin_wpp config
// CUSTOM_TYPE(Devnode_RelationName, ItemEnum(TDevnode_RelationName) );
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(Devnode_FieldName, ItemEnum(TDevnode_FieldName) );
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(System_RelationName, ItemEnum(TSystem_RelationName) );
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(System_FieldName, ItemEnum(TSystem_FieldName) );
// end_wpp


// begin_wpp config
// CUSTOM_TYPE(Fake_RelationName, ItemEnum(TFake_RelationName) );
// end_wpp


