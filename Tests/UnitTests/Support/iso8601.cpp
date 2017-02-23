// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Functions to convert dates back and forth to the ISO8601 format
// http://www.iso.org/iso/en/prods-services/popstds/datesandtime.html

#include "pch.h"
#include "ISO8601.h"
#include <intsafe.h>

#define ISO8601_MAX_USED_CCH 26 // Max amount of characters when generating ISO 8601 strings: YYYY-MM-DDThh:mm:ss.ssssZ + terminating zero


// This table defines different "types" of characters for use as the columns
// of the state table:
// 0 - invalid character
// 1 - number
// 2 - '-'
// 3 - date-time separator ('T', 't' and ' ')
// 4 - ':'
// 5 - UTC zone ('Z' and 'z')
// 6 - '+'
// 7 - second-fraction separator ('.' and ',')

static const unsigned char iso8601chartable[256] =
{
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 (00 - 0f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1 (10 - 1f)
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 2, 7, 0, // 2 (20 - 2f)
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, // 3 (30 - 3f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4 (40 - 4f)
    0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, // 5 (50 - 5f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6 (60 - 6f)
    0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, // 7 (70 - 7f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8 (80 - 8f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9 (90 - 9f)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a (a0 - af)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b (b0 - bf)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // c (c0 - cf)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // d (d0 - df)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // e (e0 - ef)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // f (f0 - ff)
};

// Parsing state table is made up of WORD values with this meaning:
//
// |    action     | |  next state   |
// |_|_|_|_|_|_|_|_| |_|_|_|_|_|_|_|_|
//   HIBYTE            LOBYTE

#define STENTRY WORD
#define S(a,b) MAKEWORD(b,a)
#define ACTION(a) HIBYTE(a)
#define STATE(a) LOBYTE(a)

// Actions
#define _OK_  0x00 // valid character
#define _NXT  0x01 // end of a segment, move to next
#define _TZM  0x02 // starting '-' time zone offset
#define _TZP  0x03 // starting '+' time zone offset
#define _MSC  0x04 // millisecond digit
#define _TZU  0x05 // time zone UTC
#define _ERR  0x80 // error, invalid character
#define ___ERROR____ MAKEWORD(0x00, _ERR)

// State table
#define STATE_TABLE_DIM 8
static const STENTRY iso8601StateTable[][STATE_TABLE_DIM] =
{
   //  unknown       number        '-'           'T'           ':'           'Z'           '+'           '.'
    ___ERROR____, S(_OK_,0x01), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x00 year
    ___ERROR____, S(_OK_,0x02), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x01
    ___ERROR____, S(_OK_,0x03), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x02
    ___ERROR____, S(_NXT,0x04), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x03
    ___ERROR____, S(_OK_,0x06), S(_OK_,0x05), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x04 month
    ___ERROR____, S(_OK_,0x06), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x05
    ___ERROR____, S(_NXT,0x07), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x06
    ___ERROR____, S(_OK_,0x09), S(_OK_,0x08), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x07 day
    ___ERROR____, S(_OK_,0x09), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x08
    ___ERROR____, S(_NXT,0x0a), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x09
    ___ERROR____, S(_OK_,0x0c), ___ERROR____, S(_OK_,0x0b), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x0a hour
    ___ERROR____, S(_OK_,0x0c), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x0b
    ___ERROR____, S(_NXT,0x0d), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x0c
    ___ERROR____, S(_OK_,0x0f), S(_TZM,0x15), ___ERROR____, S(_OK_,0x0e), S(_TZU,0x1b), S(_TZP,0x15), ___ERROR____, //0x0d min
    ___ERROR____, S(_OK_,0x0f), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x0e
    ___ERROR____, S(_NXT,0x10), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x0f
    ___ERROR____, S(_OK_,0x12), S(_TZM,0x15), ___ERROR____, S(_OK_,0x11), S(_TZU,0x1b), S(_TZP,0x15), ___ERROR____, //0x10 sec
    ___ERROR____, S(_OK_,0x12), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x11
    ___ERROR____, S(_NXT,0x13), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x12
    ___ERROR____, ___ERROR____, S(_TZM,0x15), ___ERROR____, ___ERROR____, S(_TZU,0x1b), S(_TZP,0x15), S(_OK_,0x14), //0x13 '.' or 'Z' or '+/-'
    ___ERROR____, S(_MSC,0x14), S(_TZM,0x15), ___ERROR____, ___ERROR____, S(_TZU,0x1b), S(_TZP,0x15), ___ERROR____, //0x14 fragment of a second
    ___ERROR____, S(_OK_,0x16), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x15 TZ offset - hour
    ___ERROR____, S(_NXT,0x17), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x16
    ___ERROR____, S(_OK_,0x19), ___ERROR____, ___ERROR____, S(_OK_,0x18), ___ERROR____, ___ERROR____, ___ERROR____, //0x17 TZ offset - minute
    ___ERROR____, S(_OK_,0x19), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x18
    ___ERROR____, S(_NXT,0x1a), ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x19
    ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x1a offset done - parsing done
    ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, ___ERROR____, //0x1b UTC zone done - parsing done
};


#define IsLeapYear(YEARS) (                        \
    (((YEARS) % 400 == 0) ||                       \
     ((YEARS) % 100 != 0) && ((YEARS) % 4 == 0)) ? \
        TRUE                                       \
    :                                              \
        FALSE                                      \
    )

static HRESULT _GetNumDaysForYearMonth(
    WORD wYear,
    WORD wMonth,
    __out WORD *pwDays)
{
    HRESULT hr = S_OK;

    if (NULL == pwDays ||
        wMonth < 1 || wMonth > 12 ||
        wYear < 1601 || wYear > 9999)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        static const WORD s_wNumDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        *pwDays = s_wNumDays[wMonth - 1];
        // Check for leap year
        if ((wMonth == 2) && IsLeapYear(wYear))
        {
            (*pwDays)++;
        }
    }

    return hr;
}

#define ONE_SECOND 10000000ui64 // number of 100-nanosecond intervals in a second
#define ONE_MINUTE (ONE_SECOND * 60) // number of 100-nanosecond intervals in a minute
#define ONE_HOUR (ONE_MINUTE * 60) // number of 100-nanosecond intervals in an hour

typedef union tagTU
{
    FILETIME ft;
    ULARGE_INTEGER ui;
} TU;

static HRESULT _AddTZOffset(
    Iso8601ParsingStage ips,
    WORD wValue,
    int iTZDirection,
    _Inout_ SYSTEMTIME *pSysTime)
{
    HRESULT hr = S_OK;
    _ASSERTE((ips == IPS_TZHOUR && wValue < 24) || (ips == IPS_TZMINUTE && wValue < 60));
    _ASSERTE(iTZDirection == 1 || iTZDirection == -1);

    // Convert SYSTEMTIME to FILETIME to perform date-time arithmetic
    TU ftTime = {0};
    if (SystemTimeToFileTime(pSysTime, &ftTime.ft))
    {
        ULARGE_INTEGER ulOffset;
        ulOffset.QuadPart = (ips == IPS_TZHOUR) ? ONE_HOUR * wValue : ONE_MINUTE * wValue;
        if (iTZDirection > 0)
        {
            ftTime.ui.QuadPart += ulOffset.QuadPart;
        }
        else
        {
            ftTime.ui.QuadPart -= ulOffset.QuadPart;
        }
        if (!FileTimeToSystemTime(&ftTime.ft, pSysTime))
        {
            hr = HRESULT_FROM_WIN32( GetLastError() );
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
    }

    return hr;
}

static HRESULT _CheckValueAndAddToSysTime(
    Iso8601ParsingStage ips,
    WORD wValue,
    WORD wMSDigits,
    int iTZDirection,
    _Inout_ SYSTEMTIME *pSysTime)
{
    HRESULT hr = S_OK;
    WORD wHiLimit = 0;
    WORD wLoLimit = 0;
    WORD *pDateWord = NULL;

    _ASSERTE((iTZDirection == 0 && ips != IPS_TZHOUR && ips != IPS_TZMINUTE) ||
        (iTZDirection != 0 && (ips == IPS_TZHOUR || ips == IPS_TZMINUTE)));
    _ASSERTE((wMSDigits == 0 && ips != IPS_MILLISECOND) ||
        (wMSDigits > 0 && wMSDigits < 4 && ips == IPS_MILLISECOND));

    switch (ips)
    {
    case IPS_YEAR:
        wLoLimit = 1601;
        wHiLimit = 9999;
        pDateWord  = (WORD *) &(pSysTime->wYear);
        break;
    case IPS_MONTH:
        wLoLimit = 1;
        wHiLimit = 12;
        pDateWord  = (WORD *) &(pSysTime->wMonth);
        break;
    case IPS_DAY:
        wLoLimit = 1;
        hr = _GetNumDaysForYearMonth(pSysTime->wYear, pSysTime->wMonth, &wHiLimit);
        _ASSERTE(SUCCEEDED(hr)); // internal call, should never fail
        pDateWord  = (WORD *) &(pSysTime->wDay);
        break;
    case IPS_HOUR:
        wLoLimit = 0;
        wHiLimit = 23;
        pDateWord  = (WORD *) &(pSysTime->wHour);
        break;
    case IPS_MINUTE:
        wLoLimit = 0;
        wHiLimit = 59;
        pDateWord  = (WORD *) &(pSysTime->wMinute);
        break;
    case IPS_SECOND:
        wLoLimit = 0;
        wHiLimit = 59;
        pDateWord  = (WORD *) &(pSysTime->wSecond);
        break;
    case IPS_MILLISECOND:
        wLoLimit = 0;
        wHiLimit = 999;
        while (wMSDigits++ < 3)
        {
            wValue *= 10;
        }
        pDateWord  = (WORD *) &(pSysTime->wMilliseconds);
        break;
    case IPS_TZHOUR:
        wLoLimit = 0;
        wHiLimit = 23; // valid offsets appear to be -12 to +14, but we'll allow any valid hour value
        pDateWord  = (WORD *) &(pSysTime->wHour);
        break;
    case IPS_TZMINUTE:
        wLoLimit = 0;
        wHiLimit = 59;
        pDateWord  = (WORD *) &(pSysTime->wMinute);
        break;
    default:
        hr = E_UNEXPECTED;
        break;
    }

    if (SUCCEEDED(hr))
    {
        _ASSERTE(NULL != pDateWord);
        if (wValue < wLoLimit || wValue > wHiLimit)
        {
            hr = E_ABORT;
        }
        else
        {
            _ASSERTE(*pDateWord == 0 || ips >= IPS_MILLISECOND);
            if (ips != IPS_TZHOUR && ips != IPS_TZMINUTE)
            {
                *pDateWord = wValue;
            }
            else
            {
                _ASSERTE(iTZDirection == 1 || iTZDirection == -1);
                // handle offset rollover - can affect entire date
                hr = _AddTZOffset(ips, wValue, iTZDirection, pSysTime);
            }
        }
    }

    return hr;
}

static HRESULT _iso8601ToSysTime(
    const char *pszisoDate,
    __out SYSTEMTIME *pSysTime,
    __out Iso8601ParsingStage *pips)
{
    HRESULT hr = S_OK;

    if (NULL == pszisoDate ||
        NULL == pSysTime ||
        NULL == pips ||
        *pszisoDate == '\0')
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        ZeroMemory(pSysTime, sizeof(SYSTEMTIME));
        *pips = IPS_INVALID;

        BYTE action = 0;
        BYTE state = 0;
        WORD DateWord = 0;
        Iso8601ParsingStage ips = IPS_YEAR;
        int iTZDirection = 0;
        WORD wMSDigits = 0;

        // Main state machine loop
        while(*pszisoDate && SUCCEEDED(hr))
        {
            unsigned char code = iso8601chartable[*pszisoDate];

            // Prevent overflows - sanity check
            if ((code >= STATE_TABLE_DIM) ||
                (state >= ARRAYSIZE(iso8601StateTable)))
            {
                hr = E_UNEXPECTED;
                break;
            }

            STENTRY stValue = iso8601StateTable[state][code];
            state = STATE(stValue);
            action = ACTION(stValue);

            switch(action)
            {
            case _OK_: // input OK, valid character
            case _NXT: // finish piece and advance to next stage, valid character
                {
                    if (code == 1)
                    {
                        DateWord = (DateWord * 10) + (*pszisoDate - '0');
                    }

                    if (action == _NXT)
                    {
                        hr = _CheckValueAndAddToSysTime(ips, DateWord, 0 /*wMSDigits*/, iTZDirection, pSysTime);
                        DateWord = 0;
                        if (SUCCEEDED(hr))
                        {
                            *pips = ips;
                            ips = Iso8601ParsingStage(int(ips) + 1);
                        }
                    }
                }
                break;
            case _TZM: // start '-' timezone offset
                iTZDirection = 1;
                DateWord = 0;
                *pips = ips;
                ips = IPS_TZHOUR;
                break;
            case _TZP: // start '+' timezone offset
                iTZDirection = -1;
                DateWord = 0;
                *pips = ips;
                ips = IPS_TZHOUR;
                break;
            case _MSC:  // process millisecond digit
                _ASSERTE(code == 1 && ips == IPS_MILLISECOND);
                wMSDigits++;
                if (wMSDigits < 4) 
                {
                    DateWord = (DateWord * 10) + (*pszisoDate - '0');

                    hr = _CheckValueAndAddToSysTime(ips, DateWord, wMSDigits, iTZDirection, pSysTime);
                    if (SUCCEEDED(hr))
                    {
                        *pips = IPS_MILLISECOND;
                    }
                }
                break;
            case _TZU: // 'Z' for UTC zone
                *pips = IPS_TZUTC;
                ips = IPS_TZUTC;
                break;
            case _ERR: // error, invalid character
                _ASSERTE(state == 0x00);
                hr = E_ABORT;
                break;
            default:
                hr = E_UNEXPECTED;
                break;
            }

            pszisoDate++;
        }

        if (SUCCEEDED(hr) && *pszisoDate == '\0')
        {
            if (action != _NXT && action != _MSC && action != _TZU)
            {
                hr = E_ABORT;
            }
        }

        if (hr != S_OK && hr != E_ABORT)
        {
            *pips = IPS_INVALID;
        }
    }

    _ASSERTE(hr != E_UNEXPECTED);
    return hr;
}

static HRESULT _iso8601ToFileTime(
    const char *pszisoDate,
    __out FILETIME *pftTime,
    __out Iso8601ParsingStage *pips)
{
    HRESULT hr = S_OK;
    SYSTEMTIME stTime = {0};

    hr = _iso8601ToSysTime(pszisoDate, &stTime, pips);
    if (SUCCEEDED(hr))
    {
        if (!SystemTimeToFileTime(&stTime, pftTime))
        {
            hr = E_FAIL;
        }
    }

    return hr;
}

static HRESULT _SysTimeToiso8601(
    _In_ SYSTEMTIME *pstTime,
    _In_ BOOL fGeneratePartial,
    __out_ecount(cch) char *pszBuf,
    _In_ ULONG cch,
    _In_ BOOL fUseShortTimeFormat = FALSE
    )
{
    HRESULT hr = S_OK;

    if (NULL == pstTime ||
        NULL == pszBuf ||
        cch < ISO8601_MAX_USED_CCH)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        if (pstTime->wYear < 1601 || // FILETIME cannot handle less
        pstTime->wYear > 9999 || // ISO8601 has four digits for a year
        pstTime->wMonth > 12 ||
        pstTime->wDay > 31 ||
        pstTime->wHour > 24 ||
        pstTime->wMinute > 59 ||
        pstTime->wSecond > 59 ||
        pstTime->wMilliseconds > 999)
        {
            hr = E_INVALIDARG;
        }

        if (SUCCEEDED(hr))
        {
            WORD wDays = 0;
            hr = _GetNumDaysForYearMonth(pstTime->wYear, pstTime->wMonth, &wDays);
            if (SUCCEEDED(hr) && pstTime->wDay > wDays)
            {
                hr = E_INVALIDARG;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        pszBuf[0] = static_cast<char>((pstTime->wYear / 1000) + '0');
        pszBuf[1] = static_cast<char>(((pstTime->wYear / 100) % 10) + '0');
        pszBuf[2] = static_cast<char>(((pstTime->wYear / 10) % 10) + '0');
        pszBuf[3] = static_cast<char>(((pstTime->wYear) % 10) + '0');
        if (pstTime->wMonth > 0 ||
            !fGeneratePartial)
        {
            pszBuf[4] = '-';
            pszBuf[5] = static_cast<char>((pstTime->wMonth / 10) + '0');
            pszBuf[6] = static_cast<char>((pstTime->wMonth % 10) + '0');
            if (pstTime->wDay > 0 ||
                !fGeneratePartial)
            {
                pszBuf[7] = '-';
                pszBuf[8] = static_cast<char>((pstTime->wDay / 10) + '0');
                pszBuf[9] = static_cast<char>((pstTime->wDay % 10) + '0');
                if (pstTime->wHour != 0 ||
                    pstTime->wMinute != 0 ||
                    pstTime->wSecond != 0 ||
                    pstTime->wMilliseconds != 0 ||
                    !fGeneratePartial)
                {
                    pszBuf[10] = 'T';
                    pszBuf[11] = static_cast<char>(pstTime->wHour / 10 + '0');
                    pszBuf[12] = static_cast<char>((pstTime->wHour % 10) + '0');
                    pszBuf[13] = ':';
                    pszBuf[14] = static_cast<char>(pstTime->wMinute / 10 + '0');
                    pszBuf[15] = static_cast<char>((pstTime->wMinute % 10) + '0');
                    pszBuf[16] = ':';
                    pszBuf[17] = static_cast<char>(pstTime->wSecond / 10 + '0');
                    pszBuf[18] = static_cast<char>((pstTime->wSecond % 10) + '0');
                    if ( !fUseShortTimeFormat && ( pstTime->wMilliseconds != 0 ) )
                    {
                        // YYYY-MM-DDThh:mm:ss.ssssZ
                        pszBuf[19] = '.';
                        pszBuf[20] = static_cast<char>(pstTime->wMilliseconds / 100 + '0');
                        pszBuf[21] = static_cast<char>(((pstTime->wMilliseconds / 10) % 10) + '0');
                        pszBuf[22] = static_cast<char>((pstTime->wMilliseconds % 10) + '0');

                        // pad the last digit of millisecond with 0
                        pszBuf[23] = '0';
                        pszBuf[24] = 'Z';
                        pszBuf[25] = 0;
                    }
                    else
                    {
                        // YYYY-MM-DDThh:mm:ssZ
                        pszBuf[19] = 'Z';
                        pszBuf[20] = 0;
                    }
                }
                else
                {
                    // YYYY-MM-DD
                    pszBuf[10] = 0;
                }
            }
            else
            {
                // YYYY-MM
                pszBuf[7] = 0;
            }
        }
        else
        {
            // YYYY
            pszBuf[4] = 0;
        }
    }

    return hr;
}

static HRESULT _FileTimeToiso8601(
    const FILETIME *pftTime,
    BOOL fGeneratePartial,
    __out_ecount(cch) char *pszBuf,
    ULONG cch,
    BOOL fUseShortTimeFormat
    )
{
    HRESULT hr = S_OK;
    SYSTEMTIME stTime = {0};

    if (NULL == pftTime)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        if (FileTimeToSystemTime( pftTime, &stTime))
        {
            hr = _SysTimeToiso8601( &stTime, fGeneratePartial, pszBuf, cch, fUseShortTimeFormat );
        }
        else
        {
            hr = E_FAIL;
        }
    }

    return hr;
}

HRESULT FILETIMEToISO8601W(
    _In_ const FILETIME* pft,
    _In_ BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    _In_ size_t cchISO8601,
    _In_ BOOL fUseShortTimeFormat
    )
{
    HRESULT hr = S_OK;

    CHAR aszISO8601[ISO8601_MAX_USED_CCH];
    hr = _FileTimeToiso8601( pft, fGeneratePartial, aszISO8601, ARRAYSIZE(aszISO8601), fUseShortTimeFormat );
    if (SUCCEEDED(hr))
    {
        int cchSHAnsiToUnicode;
        hr = SizeTToInt(cchISO8601, &cchSHAnsiToUnicode);
        if (SUCCEEDED(hr))
        {
            hr = ( ISO8601_MAX_USED_CCH >= ::MultiByteToWideChar(
                    CP_ACP,
                    0,
                    aszISO8601,
                    -1,
                    pszISO8601, cchSHAnsiToUnicode) ) ? S_OK : E_UNEXPECTED;
            _ASSERTE(SUCCEEDED(hr));
        }
    }

    return hr;
}

HRESULT ISO8601ToFILETIMEW(
    _In_ PCWSTR pszISO8601,
    __out FILETIME* pft,
    __out Iso8601ParsingStage *pips)
{
    HRESULT hr = S_OK;

    CHAR aszISO8601[ISO8601_MAX_CCH];

    if( !::WideCharToMultiByte(
        CP_ACP,
        0,
        pszISO8601,
        -1,
        aszISO8601,
        ISO8601_MAX_CCH,
        NULL,
        NULL ) )
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
    }

    if (SUCCEEDED(hr))
    {
        hr = _iso8601ToFileTime(aszISO8601, pft, pips);
    }

    return hr;
}

HRESULT SYSTEMTIMEToISO8601ExW(
    _In_ const SYSTEMTIME* pst,
    _In_ BOOL fGeneratePartial,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    _In_ size_t cchISO8601)
{
    HRESULT hr = S_OK;

    SYSTEMTIME st = *pst;
    CHAR aszISO8601[ISO8601_MAX_USED_CCH];
    hr = _SysTimeToiso8601(&st, fGeneratePartial, aszISO8601, ARRAYSIZE(aszISO8601));
    if (SUCCEEDED(hr))
    {
        int cchSHAnsiToUnicode;
        hr = SizeTToInt(cchISO8601, &cchSHAnsiToUnicode);
        if (SUCCEEDED(hr))
        {
            hr = ( ISO8601_MAX_USED_CCH >= ::MultiByteToWideChar(
                    CP_ACP,
                    0,
                    aszISO8601,
                    -1,
                    pszISO8601,
                    cchSHAnsiToUnicode ) ) ? S_OK : E_UNEXPECTED;
            _ASSERTE(SUCCEEDED(hr));
        }
    }

    return hr;
}

HRESULT SYSTEMTIMEToISO8601W(
    _In_ const SYSTEMTIME* pst,
    __out_ecount(cchISO8601) PWSTR pszISO8601,
    _In_ size_t cchISO8601)
{
    return SYSTEMTIMEToISO8601ExW(pst, FALSE /*fGeneratePartial*/, pszISO8601, cchISO8601);
}

HRESULT ISO8601ToSYSTEMTIMEExW(
    _In_ PCWSTR pszISO8601,
    __out SYSTEMTIME* pst,
    __out Iso8601ParsingStage *pips)
{
    HRESULT hr = S_OK;

    CHAR aszISO8601[ISO8601_MAX_CCH];

    if( !::WideCharToMultiByte(
        CP_ACP,
        0,
        pszISO8601,
        -1,
        aszISO8601,
        ISO8601_MAX_CCH,
        NULL,
        NULL ) )
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
    }

    if (SUCCEEDED(hr))
    {
        hr = _iso8601ToSysTime(aszISO8601, pst, pips);
    }

    return hr;
}

HRESULT ISO8601ToSYSTEMTIMEW(
    _In_ PCWSTR pszISO8601,
    __out SYSTEMTIME* pst)
{
    HRESULT hr = S_OK;
    Iso8601ParsingStage ips = IPS_INVALID;

    hr = ISO8601ToSYSTEMTIMEExW(pszISO8601, pst, &ips);

    // Fix up less-than-full-date
    if (SUCCEEDED(hr) && ips < IPS_DAY)
    {
        if (ips < IPS_DAY)
        {
            pst->wDay = 1;
        }
        if (ips < IPS_MONTH)
        {
            pst->wMonth = 1;
        }
        hr = S_OK;
    }

    return hr;
}
