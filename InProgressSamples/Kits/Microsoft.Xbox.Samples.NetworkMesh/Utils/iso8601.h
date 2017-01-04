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

// Functions to convert dates back and forth to the ISO8601 format
// (http://www.iso.org/iso/en/prods-services/popstds/datesandtime.html)
//
// Supported dates are from 1601-01-01 (SYSTEMTIME and FILETIME limitation) to
// 9999-12-31 (ISO8601 limitation)

// Longest form of ISO8601 is 40 chars + 1 for terminating zero
#define ISO8601_MAX_CCH 41

// Iso8601ParsingStage enum
//
// This enumeration is design so values can be compared.
// ISO8601 dates look like this: YYYY-MM-DDThh:mm:ss.sss+/-hh:mm
// So e.g. by parsing the date and then asking if parse stage was
// <IPS_HOUR you can tell if string contained any time at all, or just the date
enum Iso8601ParsingStage
{
    IPS_INVALID = -1,
    IPS_YEAR = 0,
    IPS_MONTH,
    IPS_DAY,
    IPS_HOUR,
    IPS_MINUTE,
    IPS_SECOND,
    IPS_MILLISECOND,
    IPS_TZHOUR,
    IPS_TZMINUTE,
    IPS_TZUTC,
};

HRESULT
FILETIMEToISO8601W(
    __in const FILETIME* pft,
    __in BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    __in size_t cchISO8601,
    __in BOOL fUseShortTimeFormat = FALSE
    );

HRESULT
ISO8601ToFILETIMEW(
    __in PCWSTR pszISO8601,
    __out FILETIME* pft,
    __out Iso8601ParsingStage* pips
    );

HRESULT
SYSTEMTIMEToISO8601ExW(
    __in const SYSTEMTIME* pst,
    __in BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    __in size_t cchISO8601
    );

HRESULT
SYSTEMTIMEToISO8601W(
    __in const SYSTEMTIME* pst,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
   __in  size_t cchISO8601
   );

HRESULT
ISO8601ToSYSTEMTIMEExW(
    __in PCWSTR pszISO8601,
    __out SYSTEMTIME* pst,
    __out Iso8601ParsingStage* pips
    );

HRESULT ISO8601ToSYSTEMTIMEW(
    __in PCWSTR pszISO8601,
    __out SYSTEMTIME* pst
    );
