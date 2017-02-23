// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    _In_ const FILETIME* pft,
    _In_ BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    _In_ size_t cchISO8601,
    _In_ BOOL fUseShortTimeFormat = FALSE
    );

HRESULT
ISO8601ToFILETIMEW(
    _In_ PCWSTR pszISO8601,
    __out FILETIME* pft,
    __out Iso8601ParsingStage* pips
    );

HRESULT
SYSTEMTIMEToISO8601ExW(
    _In_ const SYSTEMTIME* pst,
    _In_ BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    _In_ size_t cchISO8601
    );

HRESULT
SYSTEMTIMEToISO8601W(
    _In_ const SYSTEMTIME* pst,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
   _In_  size_t cchISO8601
   );

HRESULT
ISO8601ToSYSTEMTIMEExW(
    _In_ PCWSTR pszISO8601,
    __out SYSTEMTIME* pst,
    __out Iso8601ParsingStage* pips
    );

HRESULT ISO8601ToSYSTEMTIMEW(
    _In_ PCWSTR pszISO8601,
    __out SYSTEMTIME* pst
    );
