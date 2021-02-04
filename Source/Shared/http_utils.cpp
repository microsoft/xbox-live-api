/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Utilities
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "pch.h"

#include <array>

#if defined(_WIN32)
#if HC_PLATFORM != HC_PLATFORM_XDK
#include <winhttp.h>
#endif
#else // _WIN32
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
// TODO 1808 #include <boost/date_time/posix_time/posix_time.hpp>
// #include <boost/date_time/posix_time/posix_time_io.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif // _WIN32

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 26444 ) // ignore various unnamed objects
#pragma warning( disable : 26498 ) // ignore eof warning 
#pragma warning( disable : 26812 )  // enum instead of enum class
#endif

// Could use C++ standard library if not __GLIBCXX__,
// For testing purposes we just the handwritten on all platforms.
#if defined(CPPREST_STDLIB_UNICODE_CONVERSIONS)
#include <codecvt>
#endif

namespace xbox
{
namespace services
{
namespace detail
{
    xsapi_internal_string _to_base64(const unsigned char *ptr, size_t size);
    std::vector<unsigned char> _from_base64(const xsapi_internal_string& str);

    struct _triple_byte
    {
        unsigned char _1_1 : 2;
        unsigned char _0 : 6;
        unsigned char _2_1 : 4;
        unsigned char _1_2 : 4;
        unsigned char _3 : 6;
        unsigned char _2_2 : 2;
    };

    static const char* _base64_enctbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const std::array<unsigned char, 128> _base64_dectbl =
    { { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
       255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
       255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
        52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 254, 255, 255,
       255,  0,    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
        15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
       255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255 } };

    xsapi_internal_string _to_base64(const unsigned char *ptr, size_t size)
    {
        xsapi_internal_string result;

        for (; size >= 3; )
        {
            const _triple_byte* record = reinterpret_cast<const _triple_byte*>(ptr);
            unsigned char idx0 = record->_0;
            unsigned char idx1 = (record->_1_1 << 4) | record->_1_2;
            unsigned char idx2 = (record->_2_1 << 2) | record->_2_2;
            unsigned char idx3 = record->_3;
            result.push_back(char(_base64_enctbl[idx0]));
            result.push_back(char(_base64_enctbl[idx1]));
            result.push_back(char(_base64_enctbl[idx2]));
            result.push_back(char(_base64_enctbl[idx3]));
            size -= 3;
            ptr += 3;
        }
        switch (size)
        {
        case 1:
        {
            const _triple_byte* record = reinterpret_cast<const _triple_byte*>(ptr);
            unsigned char idx0 = record->_0;
            unsigned char idx1 = (record->_1_1 << 4);
            result.push_back(char(_base64_enctbl[idx0]));
            result.push_back(char(_base64_enctbl[idx1]));
            result.push_back('=');
            result.push_back('=');
            break;
        }
        case 2:
        {
            const _triple_byte* record = reinterpret_cast<const _triple_byte*>(ptr);
            unsigned char idx0 = record->_0;
            unsigned char idx1 = (record->_1_1 << 4) | record->_1_2;
            unsigned char idx2 = (record->_2_1 << 2);
            result.push_back(char(_base64_enctbl[idx0]));
            result.push_back(char(_base64_enctbl[idx1]));
            result.push_back(char(_base64_enctbl[idx2]));
            result.push_back('=');
            break;
        }
        }
        return result;
    }

//
// A note on the implementation of BASE64 encoding and decoding:
//
// This is a fairly basic and naive implementation; there is probably a lot of room for
// performance improvement, as well as for adding options such as support for URI-safe base64,
// ignoring CRLF, relaxed validation rules, etc. The decoder is currently pretty strict.
//

#ifdef __GNUC__
// gcc is concerned about the bitfield uses in the code, something we simply need to ignore.
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    std::vector<unsigned char> _from_base64(const xsapi_internal_string& input)
    {
        std::vector<unsigned char> result;

        if (input.empty())
            return result;

        size_t padding = 0;

        // Validation
        {
            auto size = input.size();

            if ((size % 4) != 0)
            {
                throw std::runtime_error("length of base64 string is not an even multiple of 4");
            }

            for (auto iter = input.begin(); iter != input.end(); ++iter, --size)
            {
                const size_t ch_sz = static_cast<size_t>(*iter);
                if (ch_sz >= _base64_dectbl.size() || _base64_dectbl[ch_sz] == 255)
                {
                    throw std::runtime_error("invalid character found in base64 string");
                }
                if (_base64_dectbl[ch_sz] == 254)
                {
                    padding++;
                    // padding only at the end
                    if (size > 2)
                    {
                        throw std::runtime_error("invalid padding character found in base64 string");
                    }
                    if (size == 2)
                    {
                        const size_t ch2_sz = static_cast<size_t>(*(iter + 1));
                        if (ch2_sz >= _base64_dectbl.size() || _base64_dectbl[ch2_sz] != 254)
                        {
                            throw std::runtime_error("invalid padding character found in base64 string");
                        }
                    }
                }
            }
        }


        auto size = input.size();
        const char* ptr = &input[0];

        auto outsz = (size / 4) * 3;
        outsz -= padding;

        result.resize(outsz);

        size_t idx = 0;
        for (; size > 4; ++idx)
        {
            unsigned char target[3];
            memset(target, 0, sizeof(target));
            _triple_byte* record = reinterpret_cast<_triple_byte*>(target);

            unsigned char val0 = _base64_dectbl[ptr[0]];
            unsigned char val1 = _base64_dectbl[ptr[1]];
            unsigned char val2 = _base64_dectbl[ptr[2]];
            unsigned char val3 = _base64_dectbl[ptr[3]];

            record->_0 = val0;
            record->_1_1 = val1 >> 4;
            result[idx] = target[0];

            record->_1_2 = val1 & 0xF;
            record->_2_1 = val2 >> 2;
            result[++idx] = target[1];

            record->_2_2 = val2 & 0x3;
            record->_3 = val3 & 0x3F;
            result[++idx] = target[2];

            ptr += 4;
            size -= 4;
        }

        // Handle the last four bytes separately, to avoid having the conditional statements
        // in all the iterations (a performance issue).

        {
            unsigned char target[3];
            memset(target, 0, sizeof(target));
            _triple_byte* record = reinterpret_cast<_triple_byte*>(target);

            DISABLE_WARNING_PUSH;
            SUPPRESS_WARNING_EXPRESSION_NOT_TRUE;
            unsigned char val0 = _base64_dectbl[ptr[0]];
            DISABLE_WARNING_POP;
            unsigned char val1 = _base64_dectbl[ptr[1]];
            unsigned char val2 = _base64_dectbl[ptr[2]];
            unsigned char val3 = _base64_dectbl[ptr[3]];

            record->_0 = val0;
            record->_1_1 = val1 >> 4;
            result[idx] = target[0];

            record->_1_2 = val1 & 0xF;
            if (val2 != 254)
            {
                record->_2_1 = val2 >> 2;
                result[++idx] = target[1];
            }
            else
            {
                // There shouldn't be any information (ones) in the unused bits,
                if (record->_1_2 != 0)
                {
                    throw std::runtime_error("Invalid end of base64 string");
                }
                return result;
            }

            record->_2_2 = val2 & 0x3;
            if (val3 != 254)
            {
                record->_3 = val3 & 0x3F;
                result[++idx] = target[2];
            }
            else
            {
                // There shouldn't be any information (ones) in the unused bits.
                if (record->_2_2 != 0)
                {
                    throw std::runtime_error("Invalid end of base64 string");
                }
                return result;
            }
        }

        return result;
    }
}

#ifndef _WIN32
    datetime datetime::timeval_to_datetime(const timeval& time)
    {
        const uint64_t epoch_offset = 11644473600LL; // diff between windows and unix epochs (seconds)
        uint64_t result = epoch_offset + time.tv_sec;
        result *= _secondTicks; // convert to 10e-7
        result += time.tv_usec * 10; // convert and add microseconds, 10e-6 to 10e-7
        return datetime(result);
    }
#endif

    static bool is_digit(char c) { return c >= '0' && c <= '9'; }

    datetime datetime::utc_now()
    {
#ifdef _WIN32
        ULARGE_INTEGER largeInt;
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);

        largeInt.LowPart = fileTime.dwLowDateTime;
        largeInt.HighPart = fileTime.dwHighDateTime;

        return datetime(largeInt.QuadPart);
#else //LINUX
        timeval time{};
        gettimeofday(&time, nullptr);
        return timeval_to_datetime(time);
#endif
    }

    xsapi_internal_string datetime::to_string(date_format format) const
    {
#ifdef _WIN32
        int status;

        ULARGE_INTEGER largeInt;
        largeInt.QuadPart = m_interval;

        FILETIME ft;
        ft.dwHighDateTime = largeInt.HighPart;
        ft.dwLowDateTime = largeInt.LowPart;

        SYSTEMTIME systemTime;
        if (!FileTimeToSystemTime((const FILETIME*)&ft, &systemTime))
        {
            throw details::create_system_error(GetLastError());
        }

        xsapi_internal_wostringstream outStream;
        outStream.imbue(std::locale::classic());

        if (format == RFC_1123)
        {
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
            TCHAR dateStr[18] = { 0 };
            status = GetDateFormat(LOCALE_INVARIANT, 0, &systemTime, __TEXT("ddd',' dd MMM yyyy"), dateStr, sizeof(dateStr) / sizeof(TCHAR));
#else
            wchar_t dateStr[18] = { 0 };
            status = GetDateFormatEx(LOCALE_NAME_INVARIANT, 0, &systemTime, L"ddd',' dd MMM yyyy", dateStr, sizeof(dateStr) / sizeof(wchar_t), NULL);
#endif // _WIN32_WINNT < _WIN32_WINNT_VISTA
            if (status == 0)
            {
                throw details::create_system_error(GetLastError());
            }

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
            TCHAR timeStr[10] = { 0 };
            status = GetTimeFormat(LOCALE_INVARIANT, TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT, &systemTime, __TEXT("HH':'mm':'ss"), timeStr, sizeof(timeStr) / sizeof(TCHAR));
#else
            wchar_t timeStr[10] = { 0 };
            status = GetTimeFormatEx(LOCALE_NAME_INVARIANT, TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT, &systemTime, L"HH':'mm':'ss", timeStr, sizeof(timeStr) / sizeof(wchar_t));
#endif // _WIN32_WINNT < _WIN32_WINNT_VISTA
            if (status == 0)
            {
                throw details::create_system_error(GetLastError());
            }

            outStream << dateStr << " " << timeStr << " " << "GMT";
        }
        else if (format == ISO_8601)
        {
            const size_t buffSize = 64;
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
            TCHAR dateStr[buffSize] = { 0 };
            status = GetDateFormat(LOCALE_INVARIANT, 0, &systemTime, __TEXT("yyyy-MM-dd"), dateStr, buffSize);
#else
            wchar_t dateStr[buffSize] = { 0 };
            status = GetDateFormatEx(LOCALE_NAME_INVARIANT, 0, &systemTime, L"yyyy-MM-dd", dateStr, buffSize, NULL);
#endif // _WIN32_WINNT < _WIN32_WINNT_VISTA
            if (status == 0)
            {
                throw details::create_system_error(GetLastError());
            }

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
            TCHAR timeStr[buffSize] = { 0 };
            status = GetTimeFormat(LOCALE_INVARIANT, TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT, &systemTime, __TEXT("HH':'mm':'ss"), timeStr, buffSize);
#else
            wchar_t timeStr[buffSize] = { 0 };
            status = GetTimeFormatEx(LOCALE_NAME_INVARIANT, TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT, &systemTime, L"HH':'mm':'ss", timeStr, buffSize);
#endif // _WIN32_WINNT < _WIN32_WINNT_VISTA
            if (status == 0)
            {
                throw details::create_system_error(GetLastError());
            }

            outStream << dateStr << "T" << timeStr;
            uint64_t frac_sec = largeInt.QuadPart % _secondTicks;
            if (frac_sec > 0)
            {
                // Append fractional second, which is a 7-digit value with no trailing zeros
                // This way, '1200' becomes '00012'
                char buf[9] = { 0 };
                sprintf_s(buf, sizeof(buf), ".%07ld", (long int)frac_sec);
                // trim trailing zeros
                for (int i = 7; buf[i] == '0'; i--) buf[i] = '\0';
                outStream << buf;
            }
            outStream << "Z";
        }

        auto result = convert::utf16_to_utf8_internal(outStream.str());
        return result;
#else //LINUX
        uint64_t input = m_interval;
        uint64_t frac_sec = input % _secondTicks;
        input /= _secondTicks; // convert to seconds
        time_t time = (time_t)input - (time_t)11644473600LL;// diff between windows and unix epochs (seconds)

        struct tm datetime;
        gmtime_r(&time, &datetime);

        const int max_dt_length = 64;
        char output[max_dt_length + 1] = { 0 };

        if (format != RFC_1123 && frac_sec > 0)
        {
            // Append fractional second, which is a 7-digit value with no trailing zeros
            // This way, '1200' becomes '00012'
            char buf[9] = { 0 };
            snprintf(buf, sizeof(buf), ".%07ld", (long int)frac_sec);
            // trim trailing zeros
            for (int i = 7; buf[i] == '0'; i--) buf[i] = '\0';
            // format the datetime into a separate buffer
            char datetime_str[max_dt_length + 1] = { 0 };
            strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%dT%H:%M:%S", &datetime);
            // now print this buffer into the output buffer
            snprintf(output, sizeof(output), "%s%sZ", datetime_str, buf);
        }
        else
        {
            strftime(output, sizeof(output),
                format == RFC_1123 ? "%a, %d %b %Y %H:%M:%S GMT" : "%Y-%m-%dT%H:%M:%SZ",
                &datetime);
        }

        return xsapi_internal_string(output);
#endif
    }

    // Take a string that represents a fractional second and return the number of ticks
    // This is equivalent to doing atof on the string and multiplying by 10000000,
    // but does not lose precision
    template<typename StringIterator>
    uint64_t timeticks_from_second(StringIterator begin, StringIterator end)
    {
        int size = (int)(end - begin);
        _ASSERTE(begin[0] == U('.'));
        uint64_t ufrac_second = 0;
        for (int i = 1; i <= 7; ++i)
        {
            ufrac_second *= 10;
            int add = i < size ? begin[i] - U('0') : 0;
            ufrac_second += add;
        }
        return ufrac_second;
    }

    void extract_fractional_second(const xsapi_internal_string& dateString, xsapi_internal_string& resultString, uint64_t& ufrac_second)
    {
        resultString = dateString;
        // First, the string must be strictly longer than 2 characters, and the trailing character must be 'Z'
        if (resultString.size() > 2 && resultString[resultString.size() - 1] == U('Z'))
        {
            // Second, find the last non-digit by scanning the string backwards
            auto last_non_digit = std::find_if_not(resultString.rbegin() + 1, resultString.rend(), is_digit);
            if (last_non_digit < resultString.rend() - 1)
            {
                // Finally, make sure the last non-digit is a dot:
                auto last_dot = last_non_digit.base() - 1;
                if (*last_dot == U('.'))
                {
                    // Got it! Now extract the fractional second
                    auto last_before_Z = std::end(resultString) - 1;
                    ufrac_second = timeticks_from_second(last_dot, last_before_Z);
                    // And erase it from the string
                    resultString.erase(last_dot, last_before_Z);
                }
            }
        }
    }

#ifdef _WIN32
    bool datetime::system_type_to_datetime(void* pvsysTime, uint64_t seconds, datetime* pdt)
    {
        SYSTEMTIME* psysTime = (SYSTEMTIME*)pvsysTime;
        FILETIME fileTime;

        if (SystemTimeToFileTime(psysTime, &fileTime))
        {
            ULARGE_INTEGER largeInt;
            largeInt.LowPart = fileTime.dwLowDateTime;
            largeInt.HighPart = fileTime.dwHighDateTime;

            // Add hundredths of nanoseconds
            largeInt.QuadPart += seconds;

            *pdt = datetime(largeInt.QuadPart);
            return true;
        }
        return false;
    }
#endif

    datetime datetime::from_string(const xsapi_internal_string& dateString, date_format format)
    {
        // avoid floating point math to preserve precision
        uint64_t ufrac_second = 0;

#ifdef _WIN32
        datetime result;
        if (format == RFC_1123)
        {
            SYSTEMTIME sysTime = { 0 };

            xsapi_internal_string month(3, '\0');
            xsapi_internal_string unused(3, '\0');

            const char* formatString = "%3c, %2d %3c %4d %2d:%2d:%2d %3c";
            auto n = sscanf_s(dateString.c_str(), formatString,
                unused.data(), unused.size(),
                &sysTime.wDay,
                month.data(), month.size(),
                &sysTime.wYear,
                &sysTime.wHour,
                &sysTime.wMinute,
                &sysTime.wSecond,
                unused.data(), unused.size());

            if (n == 8)
            {
                xsapi_internal_string monthnames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
                auto loc = std::find_if(monthnames, monthnames + 12, [&month](const xsapi_internal_string& m) { return m == month; });

                if (loc != monthnames + 12)
                {
                    sysTime.wMonth = (short)((loc - monthnames) + 1);
                    if (system_type_to_datetime(&sysTime, ufrac_second, &result))
                    {
                        return result;
                    }
                }
            }
        }
        else if (format == ISO_8601)
        {
            // Unlike FILETIME, SYSTEMTIME does not have enough precision to hold seconds in 100 nanosecond
            // increments. Therefore, start with seconds and milliseconds set to 0, then add them separately

            // Try to extract the fractional second from the timestamp
            xsapi_internal_string input;
            extract_fractional_second(dateString, input, ufrac_second);
            {
                SYSTEMTIME sysTime = { 0 };
                const char* formatString = "%4d-%2d-%2dT%2d:%2d:%2dZ";
                auto n = sscanf_s(input.c_str(), formatString,
                    &sysTime.wYear,
                    &sysTime.wMonth,
                    &sysTime.wDay,
                    &sysTime.wHour,
                    &sysTime.wMinute,
                    &sysTime.wSecond);

                if (n == 3 || n == 6)
                {
                    if (system_type_to_datetime(&sysTime, ufrac_second, &result))
                    {
                        return result;
                    }
                }
            }
            {
                SYSTEMTIME sysTime = { 0 };
                DWORD date = 0;

                const char* formatString = "%8dT%2d:%2d:%2dZ";
                auto n = sscanf_s(input.c_str(), formatString,
                    &date,
                    &sysTime.wHour,
                    &sysTime.wMinute,
                    &sysTime.wSecond);

                if (n == 1 || n == 4)
                {
                    sysTime.wDay = date % 100;
                    date /= 100;
                    sysTime.wMonth = date % 100;
                    date /= 100;
                    sysTime.wYear = (WORD)date;

                    if (system_type_to_datetime(&sysTime, ufrac_second, &result))
                    {
                        return result;
                    }
                }
            }
            {
                SYSTEMTIME sysTime = { 0 };
                GetSystemTime(&sysTime);    // Fill date portion with today's information
                sysTime.wSecond = 0;
                sysTime.wMilliseconds = 0;

                const char* formatString = "%2d:%2d:%2dZ";
                auto n = sscanf_s(input.c_str(), formatString,
                    &sysTime.wHour,
                    &sysTime.wMinute,
                    &sysTime.wSecond);

                if (n == 3)
                {
                    if (system_type_to_datetime(&sysTime, ufrac_second, &result))
                    {
                        return result;
                    }
                }
            }
        }

        return datetime();
#else
        xsapi_internal_string input(dateString);

        struct tm output = tm();

        if (format == RFC_1123)
        {
            strptime(input.data(), "%a, %d %b %Y %H:%M:%S GMT", &output);
        }
        else
        {
            // Try to extract the fractional second from the timestamp
            xsapi_internal_string input;
            extract_fractional_second(dateString, input, ufrac_second);

            auto result = strptime(input.data(), "%Y-%m-%dT%H:%M:%SZ", &output);

            if (result == nullptr)
            {
                result = strptime(input.data(), "%Y%m%dT%H:%M:%SZ", &output);
            }
            if (result == nullptr)
            {
                // Fill the date portion with the epoch,
                // strptime will do the rest
                memset(&output, 0, sizeof(struct tm));
                output.tm_year = 70;
                output.tm_mon = 1;
                output.tm_mday = 1;
                result = strptime(input.data(), "%H:%M:%SZ", &output);
            }
            if (result == nullptr)
            {
                result = strptime(input.data(), "%Y-%m-%d", &output);
            }
            if (result == nullptr)
            {
                result = strptime(input.data(), "%Y%m%d", &output);
            }
            if (result == nullptr)
            {
                return datetime();
            }
        }

#if (defined(ANDROID) || defined(__ANDROID__))
        // HACK: The (nonportable?) POSIX function timegm is not available in
        //       bionic. As a workaround[1][2], we set the C library timezone to
        //       UTC, call mktime, then set the timezone back. However, the C
        //       environment is fundamentally a shared global resource and thread-
        //       unsafe. We can protect our usage here, however any other code might
        //       manipulate the environment at the same time.
        //
        // [1] http://linux.die.net/man/3/timegm
        // [2] http://www.gnu.org/software/libc/manual/html_node/Broken_002ddown-Time.html
        time_t time;

        static std::mutex env_var_lock;
        {
            std::lock_guard<std::mutex> lock(env_var_lock);
            xsapi_internal_string prev_env;
            auto prev_env_cstr = getenv("TZ");
            if (prev_env_cstr != nullptr)
            {
                prev_env = prev_env_cstr;
            }
            setenv("TZ", "UTC", 1);

            time = mktime(&output);

            if (prev_env_cstr)
            {
                setenv("TZ", prev_env.c_str(), 1);
            }
            else
            {
                unsetenv("TZ");
            }
            tzset();
        }
#else
        time_t time = timegm(&output);
#endif
        struct timeval tv = timeval();
        tv.tv_sec = time;
        auto result = timeval_to_datetime(tv);

        // fractional seconds are already in correct format so just add them.
        result = result + ufrac_second;
        return result;
#endif
    }

#define LOW_3BITS 0x7
#define LOW_4BITS 0xF
#define LOW_5BITS 0x1F
#define LOW_6BITS 0x3F
#define BIT4 0x8
#define BIT5 0x10
#define BIT6 0x20
#define BIT7 0x40
#define BIT8 0x80
#define L_SURROGATE_START 0xDC00
#define L_SURROGATE_END 0xDFFF
#define H_SURROGATE_START 0xD800
#define H_SURROGATE_END 0xDBFF
#define SURROGATE_PAIR_START 0x10000

    xsapi_internal_wstring convert::utf8_to_utf16(const xsapi_internal_string &s)
    {
#if defined(CPPREST_STDLIB_UNICODE_CONVERSIONS)
        std::wstring_convert<std::codecvt_utf8_utf16<utf16char>, utf16char> conversion;
        return conversion.from_bytes(s);
#else
        xsapi_internal_wstring dest;
        // Save repeated heap allocations, use less than source string size assuming some
        // of the characters are not just ASCII and collapse.
        dest.reserve(static_cast<size_t>(static_cast<double>(s.size()) * .70));

        for (auto src = s.begin(); src != s.end(); ++src)
        {
            if ((*src & BIT8) == 0) // single byte character, 0x0 to 0x7F
            {
                dest.push_back(xsapi_internal_wstring::value_type(*src));
            }
            else
            {
                unsigned char numContBytes = 0;
                uint32_t codePoint;
                if ((*src & BIT7) == 0)
                {
                    throw std::range_error("UTF-8 string character can never start with 10xxxxxx");
                }
                else if ((*src & BIT6) == 0) // 2 byte character, 0x80 to 0x7FF
                {
                    codePoint = *src & LOW_5BITS;
                    numContBytes = 1;
                }
                else if ((*src & BIT5) == 0) // 3 byte character, 0x800 to 0xFFFF
                {
                    codePoint = *src & LOW_4BITS;
                    numContBytes = 2;
                }
                else if ((*src & BIT4) == 0) // 4 byte character, 0x10000 to 0x10FFFF
                {
                    codePoint = *src & LOW_3BITS;
                    numContBytes = 3;
                }
                else
                {
                    throw std::range_error("UTF-8 string has invalid Unicode code point");
                }

                for (unsigned char i = 0; i < numContBytes; ++i)
                {
                    if (++src == s.end())
                    {
                        throw std::range_error("UTF-8 string is missing bytes in character");
                    }
                    if ((*src & BIT8) == 0 || (*src & BIT7) != 0)
                    {
                        throw std::range_error("UTF-8 continuation byte is missing leading byte");
                    }
                    codePoint <<= 6;
                    codePoint |= *src & LOW_6BITS;
                }

                if (codePoint >= SURROGATE_PAIR_START)
                {
                    // In UTF-16 U+10000 to U+10FFFF are represented as two 16-bit code units, surrogate pairs.
                    //  - 0x10000 is subtracted from the code point
                    //  - high surrogate is 0xD800 added to the top ten bits
                    //  - low surrogate is 0xDC00 added to the low ten bits
                    codePoint -= SURROGATE_PAIR_START;
                    dest.push_back(xsapi_internal_wstring::value_type((codePoint >> 10) | H_SURROGATE_START));
                    dest.push_back(xsapi_internal_wstring::value_type((codePoint & 0x3FF) | L_SURROGATE_START));
                }
                else
                {
                    // In UTF-16 U+0000 to U+D7FF and U+E000 to U+FFFF are represented exactly as the Unicode code point value.
                    // U+D800 to U+DFFF are not valid characters, for simplicity we assume they are not present but will encode
                    // them if encountered.
                    dest.push_back(xsapi_internal_wstring::value_type(codePoint));
                }
            }
        }
        return dest;
#endif
    }

    xsapi_internal_string convert::to_utf8string(xsapi_internal_string value) { return value; }

    xsapi_internal_string convert::to_utf8string(const xsapi_internal_wstring &value) { return utf16_to_utf8_internal(value); }

    std::string convert::utf16_to_utf8(const std::wstring &w)
    {
#if defined(CPPREST_STDLIB_UNICODE_CONVERSIONS)
        std::wstring_convert<std::codecvt_utf8_utf16<utf16char>, utf16char> conversion;
        return conversion.to_bytes(w);
#else
        std::string dest;
        dest.reserve(w.size());
        for (auto src = w.begin(); src != w.end(); ++src)
        {
            // Check for high surrogate.
            if (*src >= H_SURROGATE_START && *src <= H_SURROGATE_END)
            {
                const auto highSurrogate = *src++;
                if (src == w.end())
                {
                    throw std::range_error("UTF-16 string is missing low surrogate");
                }
                const auto lowSurrogate = *src;
                if (lowSurrogate < L_SURROGATE_START || lowSurrogate > L_SURROGATE_END)
                {
                    throw std::range_error("UTF-16 string has invalid low surrogate");
                }

                // To get from surrogate pair to Unicode code point:
                // - subract 0xD800 from high surrogate, this forms top ten bits
                // - subract 0xDC00 from low surrogate, this forms low ten bits
                // - add 0x10000
                // Leaves a code point in U+10000 to U+10FFFF range.
                uint32_t codePoint = highSurrogate - H_SURROGATE_START;
                codePoint <<= 10;
                codePoint |= lowSurrogate - L_SURROGATE_START;
                codePoint += SURROGATE_PAIR_START;

                // 4 bytes need using 21 bits
                dest.push_back(char((codePoint >> 18) | 0xF0));                 // leading 3 bits
                dest.push_back(char(((codePoint >> 12) & LOW_6BITS) | BIT8));   // next 6 bits
                dest.push_back(char(((codePoint >> 6) & LOW_6BITS) | BIT8));    // next 6 bits
                dest.push_back(char((codePoint & LOW_6BITS) | BIT8));           // trailing 6 bits
            }
            else
            {
                if (*src <= 0x7F) // single byte character
                {
                    dest.push_back(static_cast<char>(*src));
                }
                else if (*src <= 0x7FF) // 2 bytes needed (11 bits used)
                {
                    dest.push_back(char((*src >> 6) | 0xC0));               // leading 5 bits
                    dest.push_back(char((*src & LOW_6BITS) | BIT8));        // trailing 6 bits
                }
                else // 3 bytes needed (16 bits used)
                {
                    dest.push_back(char((*src >> 12) | 0xE0));              // leading 4 bits
                    dest.push_back(char(((*src >> 6) & LOW_6BITS) | BIT8)); // middle 6 bits
                    dest.push_back(char((*src & LOW_6BITS) | BIT8));        // trailing 6 bits
                }
            }
        }

        return dest;
#endif
    }

    xsapi_internal_string convert::utf16_to_utf8_internal(const xsapi_internal_wstring &w)
    {
#if defined(CPPREST_STDLIB_UNICODE_CONVERSIONS)
        std::wstring_convert<std::codecvt_utf8_utf16<utf16char>, utf16char> conversion;
        return conversion.to_bytes(w);
#else
        xsapi_internal_string dest;
        dest.reserve(w.size());
        for (auto src = w.begin(); src != w.end(); ++src)
        {
            // Check for high surrogate.
            if (*src >= H_SURROGATE_START && *src <= H_SURROGATE_END)
            {
                const auto highSurrogate = *src++;
                if (src == w.end())
                {
                    throw std::range_error("UTF-16 string is missing low surrogate");
                }
                const auto lowSurrogate = *src;
                if (lowSurrogate < L_SURROGATE_START || lowSurrogate > L_SURROGATE_END)
                {
                    throw std::range_error("UTF-16 string has invalid low surrogate");
                }

                // To get from surrogate pair to Unicode code point:
                // - subract 0xD800 from high surrogate, this forms top ten bits
                // - subract 0xDC00 from low surrogate, this forms low ten bits
                // - add 0x10000
                // Leaves a code point in U+10000 to U+10FFFF range.
                uint32_t codePoint = highSurrogate - H_SURROGATE_START;
                codePoint <<= 10;
                codePoint |= lowSurrogate - L_SURROGATE_START;
                codePoint += SURROGATE_PAIR_START;

                // 4 bytes need using 21 bits
                dest.push_back(char((codePoint >> 18) | 0xF0));                 // leading 3 bits
                dest.push_back(char(((codePoint >> 12) & LOW_6BITS) | BIT8));   // next 6 bits
                dest.push_back(char(((codePoint >> 6) & LOW_6BITS) | BIT8));    // next 6 bits
                dest.push_back(char((codePoint & LOW_6BITS) | BIT8));           // trailing 6 bits
            }
            else
            {
                if (*src <= 0x7F) // single byte character
                {
                    dest.push_back(static_cast<char>(*src));
                }
                else if (*src <= 0x7FF) // 2 bytes needed (11 bits used)
                {
                    dest.push_back(char((*src >> 6) | 0xC0));               // leading 5 bits
                    dest.push_back(char((*src & LOW_6BITS) | BIT8));        // trailing 6 bits
                }
                else // 3 bytes needed (16 bits used)
                {
                    dest.push_back(char((*src >> 12) | 0xE0));              // leading 4 bits
                    dest.push_back(char(((*src >> 6) & LOW_6BITS) | BIT8)); // middle 6 bits
                    dest.push_back(char((*src & LOW_6BITS) | BIT8));        // trailing 6 bits
                }
            }
        }

        return dest;
#endif
    }

    xsapi_internal_string convert::to_base64(const Vector<unsigned char>& input)
    {
        if (input.size() == 0)
        {
            // return empty string
            return String();
        }

        return detail::_to_base64(&input[0], input.size());
    }

    std::vector<unsigned char> convert::from_base64(const xsapi_internal_string& str)
    {
        return detail::_from_base64(str);
    }

    namespace details
    {

        const std::error_category & platform_category()
        {
#ifdef _WIN32
            return windows_category();
#else
            return linux_category();
#endif
        }

#ifdef _WIN32

        // Remove once VS 2013 is no longer supported.
#if _MSC_VER < 1900
        static details::windows_category_impl instance;
#endif
        const std::error_category & windows_category()
        {
#if _MSC_VER >= 1900
            static details::windows_category_impl instance;
#endif
            return instance;
        }

        std::string windows_category_impl::message(int errorCode) const CPPREST_NOEXCEPT
        {
#if 0 // this returns a non-mem hooked string which can't be changed so commenting it out since its not really needed
            const size_t buffer_size = 4096;
            DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
            LPCVOID lpSource = NULL;

#if !defined(__cplusplus_winrt)
            if (errorCode >= 12000)
            {
                dwFlags = FORMAT_MESSAGE_FROM_HMODULE;
                lpSource = GetModuleHandleA("winhttp.dll"); // this handle DOES NOT need to be freed
            }
#endif

            std::wstring buffer;
            buffer.resize(buffer_size);

            const auto result = ::FormatMessageW(
                dwFlags,
                lpSource,
                errorCode,
                0,
                &buffer[0],
                buffer_size,
                NULL);
            if (result == 0)
            {
                std::ostringstream os;
                os << "Unable to get an error message for error code: " << errorCode << ".";
                return os.str();
            }
            return convert::to_utf8string(buffer);
#else
            UNREFERENCED_PARAMETER(errorCode);
            return std::string();
#endif
        }

        std::error_condition windows_category_impl::default_error_condition(int errorCode) const CPPREST_NOEXCEPT
        {
            // First see if the STL implementation can handle the mapping for common cases.
            const std::error_condition errCondition = std::system_category().default_error_condition(errorCode);
            const std::string errConditionMsg = errCondition.message();
            if (_stricmp(errConditionMsg.c_str(), "unknown error") != 0)
            {
                return errCondition;
            }

            switch (errorCode)
            {
#ifndef __cplusplus_winrt
            case ERROR_WINHTTP_TIMEOUT:
                return std::errc::timed_out;
            case ERROR_WINHTTP_CANNOT_CONNECT:
                return std::errc::host_unreachable;
            case ERROR_WINHTTP_CONNECTION_ERROR:
                return std::errc::connection_aborted;
#endif
            case INET_E_RESOURCE_NOT_FOUND:
            case INET_E_CANNOT_CONNECT:
                return std::errc::host_unreachable;
            case INET_E_CONNECTION_TIMEOUT:
                return std::errc::timed_out;
            case INET_E_DOWNLOAD_FAILURE:
                return std::errc::connection_aborted;
            default:
                break;
            }

            return std::error_condition(errorCode, *this);
        }

#else

        const std::error_category & linux_category()
        {
            // On Linux we are using boost error codes which have the exact same
            // mapping and are equivalent with std::generic_category error codes.
            return std::generic_category();
        }

#endif
    }


}

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 26444 ) // ignore various unnamed objects
#pragma warning( disable : 26812 )  // enum instead of enum class
#endif

namespace services
{
    namespace details
    {
        xsapi_internal_string uri_components::join()
        {
            // canonicalize components first

            // convert scheme to lowercase
            std::transform(m_scheme.begin(), m_scheme.end(), m_scheme.begin(), [](char c) {
                return (char)tolower(c);
                });

            // convert host to lowercase
            std::transform(m_host.begin(), m_host.end(), m_host.begin(), [](char c) {
                return (char)tolower(c);
                });

            // canonicalize the path to have a leading slash if it's a full uri
            if (!m_host.empty() && m_path.empty())
            {
                m_path = "/";
            }
            else if (!m_host.empty() && m_path[0] != '/')
            {
                m_path.insert(m_path.begin(), 1, '/');
            }

            xsapi_internal_string ret;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800))
            if (!m_scheme.empty())
            {
                ret.append(m_scheme).append({ ':' });
            }

            if (!m_host.empty())
            {
                ret.append("//");

                if (!m_user_info.empty())
                {
                    ret.append(m_user_info).append({ '@' });
                }

                ret.append(m_host);

                if (m_port > 0)
                {
                    char buf[16] = { 0 };
                    sprintf_s(buf, sizeof(buf), ":%d", m_port);
                    ret.append(buf);
                }
            }

            if (!m_path.empty())
            {
                // only add the leading slash when the host is present
                if (!m_host.empty() && m_path.front() != '/')
                {
                    ret.append({ '/' });
                }

                ret.append(m_path);
            }

            if (!m_query.empty())
            {
                ret.append({ '?' }).append(m_query);
            }

            if (!m_fragment.empty())
            {
                ret.append({ '#' }).append(m_fragment);
            }

            return ret;
#else
            xsapi_internal_ostringstream_t os;
            os.imbue(std::locale::classic());

            if (!m_scheme.empty())
            {
                os << m_scheme << ':';
            }

            if (!m_host.empty())
            {
                os << "//";

                if (!m_user_info.empty())
                {
                    os << m_user_info << '@';
                }

                os << m_host;

                if (m_port > 0)
                {
                    os << ':' << m_port;
                }
            }

            if (!m_path.empty())
            {
                // only add the leading slash when the host is present
                if (!m_host.empty() && m_path.front() != '/')
                {
                    os << '/';
                }
                os << m_path;
            }

            if (!m_query.empty())
            {
                os << '?' << m_query;
            }

            if (!m_fragment.empty())
            {
                os << '#' << m_fragment;
            }

            return os.str();
#endif
        }

        namespace uri_parser
        {

            bool validate(const xsapi_internal_string& encoded_string)
            {
                const char* scheme_begin = nullptr;
                const char* scheme_end = nullptr;
                const char* uinfo_begin = nullptr;
                const char* uinfo_end = nullptr;
                const char* host_begin = nullptr;
                const char* host_end = nullptr;
                int port_ptr = 0;
                const char* path_begin = nullptr;
                const char* path_end = nullptr;
                const char* query_begin = nullptr;
                const char* query_end = nullptr;
                const char* fragment_begin = nullptr;
                const char* fragment_end = nullptr;

                // perform a parse, but don't copy out the data
                return inner_parse(
                    encoded_string.c_str(),
                    &scheme_begin,
                    &scheme_end,
                    &uinfo_begin,
                    &uinfo_end,
                    &host_begin,
                    &host_end,
                    &port_ptr,
                    &path_begin,
                    &path_end,
                    &query_begin,
                    &query_end,
                    &fragment_begin,
                    &fragment_end);
            }

            bool parse(const xsapi_internal_string& encoded_string, uri_components& components)
            {
                const char* scheme_begin = nullptr;
                const char* scheme_end = nullptr;
                const char* host_begin = nullptr;
                const char* host_end = nullptr;
                const char* uinfo_begin = nullptr;
                const char* uinfo_end = nullptr;
                int port_ptr = 0;
                const char* path_begin = nullptr;
                const char* path_end = nullptr;
                const char* query_begin = nullptr;
                const char* query_end = nullptr;
                const char* fragment_begin = nullptr;
                const char* fragment_end = nullptr;

                if (inner_parse(
                    encoded_string.c_str(),
                    &scheme_begin,
                    &scheme_end,
                    &uinfo_begin,
                    &uinfo_end,
                    &host_begin,
                    &host_end,
                    &port_ptr,
                    &path_begin,
                    &path_end,
                    &query_begin,
                    &query_end,
                    &fragment_begin,
                    &fragment_end))
                {
                    if (scheme_begin)
                    {
                        components.m_scheme.assign(scheme_begin, scheme_end);

                        // convert scheme to lowercase
                        std::transform(components.m_scheme.begin(), components.m_scheme.end(), components.m_scheme.begin(), [](char c) {
                            return (char)tolower(c);
                            });
                    }
                    else
                    {
                        components.m_scheme.clear();
                    }

                    if (uinfo_begin)
                    {
                        components.m_user_info.assign(uinfo_begin, uinfo_end);
                    }

                    if (host_begin)
                    {
                        components.m_host.assign(host_begin, host_end);

                        // convert host to lowercase
                        std::transform(components.m_host.begin(), components.m_host.end(), components.m_host.begin(), [](char c) {
                            return (char)tolower(c);
                            });
                    }
                    else
                    {
                        components.m_host.clear();
                    }

                    if (port_ptr)
                    {
                        components.m_port = port_ptr;
                    }
                    else
                    {
                        components.m_port = 0;
                    }

                    if (path_begin)
                    {
                        components.m_path.assign(path_begin, path_end);
                    }
                    else
                    {
                        // default path to begin with a slash for easy comparison
                        components.m_path = "/";
                    }

                    if (query_begin)
                    {
                        components.m_query.assign(query_begin, query_end);
                    }
                    else
                    {
                        components.m_query.clear();
                    }

                    if (fragment_begin)
                    {
                        components.m_fragment.assign(fragment_begin, fragment_end);
                    }
                    else
                    {
                        components.m_fragment.clear();
                    }

                    return true;
                }
                else
                {
                    return false;
                }
            }

            bool inner_parse(
                const char* encoded,
                const char** scheme_begin, const char** scheme_end,
                const char** uinfo_begin, const char** uinfo_end,
                const char** host_begin, const char** host_end,
                _Out_ int* port,
                const char** path_begin, const char** path_end,
                const char** query_begin, const char** query_end,
                const char** fragment_begin, const char** fragment_end)
            {
                *scheme_begin = nullptr;
                *scheme_end = nullptr;
                *uinfo_begin = nullptr;
                *uinfo_end = nullptr;
                *host_begin = nullptr;
                *host_end = nullptr;
                *port = 0;
                *path_begin = nullptr;
                *path_end = nullptr;
                *query_begin = nullptr;
                *query_end = nullptr;
                *fragment_begin = nullptr;
                *fragment_end = nullptr;

                const char* p = encoded;

                // IMPORTANT -- A uri may either be an absolute uri, or an relative-reference
                // Absolute: 'http://host.com'
                // Relative-Reference: '//:host.com', '/path1/path2?query', './path1:path2'
                // A Relative-Reference can be disambiguated by parsing for a ':' before the first slash

                bool is_relative_reference = true;
                const char* p2 = p;
                for (; *p2 != '/' && *p2 != '\0'; p2++)
                {
                    if (*p2 == ':')
                    {
                        // found a colon, the first portion is a scheme
                        is_relative_reference = false;
                        break;
                    }
                }

                if (!is_relative_reference)
                {
                    // the first character of a scheme must be a letter
                    if (!isalpha(*p))
                    {
                        return false;
                    }

                    // start parsing the scheme, it's always delimited by a colon (must be present)
                    *scheme_begin = p++;
                    for (; *p != ':'; p++)
                    {
                        if (!is_scheme_character(*p))
                        {
                            return false;
                        }
                    }
                    *scheme_end = p;

                    // skip over the colon
                    p++;
                }

                // if we see two slashes next, then we're going to parse the authority portion
                // later on we'll break up the authority into the port and host
                const char* authority_begin = nullptr;
                const char* authority_end = nullptr;
                if (*p == '/' && p[1] == '/')
                {
                    // skip over the slashes
                    p += 2;
                    authority_begin = p;

                    // the authority is delimited by a slash (resource), question-mark (query) or octothorpe (fragment)
                    // or by EOS. The authority could be empty ('file:///C:\file_name.txt')
                    for (; *p != '/' && *p != '?' && *p != '#' && *p != '\0'; p++)
                    {
                        // We're NOT currently supporting IPv6, IPvFuture or username/password in authority
                        if (!is_authority_character(*p))
                        {
                            return false;
                        }
                    }
                    authority_end = p;

                    // now lets see if we have a port specified -- by working back from the end
                    if (authority_begin != authority_end)
                    {
                        // the port is made up of all digits
                        const char* port_begin = authority_end - 1;
                        for (; isdigit(*port_begin) && port_begin != authority_begin; port_begin--)
                        {
                        }

                        if (*port_begin == ':')
                        {
                            // has a port
                            *host_begin = authority_begin;
                            *host_end = port_begin;

                            //skip the colon
                            port_begin++;

                            *port = convert::scan_string<int>(xsapi_internal_string(port_begin, authority_end), std::locale::classic());
                        }
                        else
                        {
                            // no port
                            *host_begin = authority_begin;
                            *host_end = authority_end;
                        }

                        // look for a user_info component
                        const char* u_end = *host_begin;
                        for (; is_user_info_character(*u_end) && u_end != *host_end; u_end++)
                        {
                        }

                        if (*u_end == '@')
                        {
                            *host_begin = u_end + 1;
                            *uinfo_begin = authority_begin;
                            *uinfo_end = u_end;
                        }
                        else
                        {
                            uinfo_end = uinfo_begin = nullptr;
                        }
                    }
                }

                // if we see a path character or a slash, then the
                // if we see a slash, or any other legal path character, parse the path next
                if (*p == '/' || is_path_character(*p))
                {
                    *path_begin = p;

                    // the path is delimited by a question-mark (query) or octothorpe (fragment) or by EOS
                    for (; *p != '?' && *p != '#' && *p != '\0'; p++)
                    {
                        if (!is_path_character(*p))
                        {
                            return false;
                        }
                    }
                    *path_end = p;
                }

                // if we see a ?, then the query is next
                if (*p == '?')
                {
                    // skip over the question mark
                    p++;
                    *query_begin = p;

                    // the query is delimited by a '#' (fragment) or EOS
                    for (; *p != '#' && *p != '\0'; p++)
                    {
                        if (!is_query_character(*p))
                        {
                            return false;
                        }
                    }
                    *query_end = p;
                }

                // if we see a #, then the fragment is next
                if (*p == '#')
                {
                    // skip over the hash mark
                    p++;
                    *fragment_begin = p;

                    // the fragment is delimited by EOS
                    for (; *p != '\0'; p++)
                    {
                        if (!is_fragment_character(*p))
                        {
                            return false;
                        }
                    }
                    *fragment_end = p;
                }

                return true;
            }

        }


    }

    using namespace details;

    uri::uri(const details::uri_components& components) : m_components(components)
    {
        m_uri = m_components.join();
        if (!details::uri_parser::validate(m_uri))
        {
            throw uri_exception("provided uri is invalid: " + convert::to_utf8string(m_uri));
        }
    }

    uri::uri(const xsapi_internal_string& uri_string)
    {
        if (!details::uri_parser::parse(uri_string, m_components))
        {
            throw uri_exception("provided uri is invalid: " + convert::to_utf8string(uri_string));
        }
        m_uri = m_components.join();
    }

    uri::uri(const char* uri_string) : m_uri(uri_string)
    {
        if (!details::uri_parser::parse(uri_string, m_components))
        {
            throw uri_exception("provided uri is invalid: " + convert::to_utf8string(uri_string));
        }
        m_uri = m_components.join();
    }

    xsapi_internal_string uri::encode_impl(const xsapi_internal_string& utf8raw, const std::function<bool(int)>& should_encode)
    {
        const char* const hex = "0123456789ABCDEF";
        xsapi_internal_string encoded;
        for (auto iter = utf8raw.begin(); iter != utf8raw.end(); ++iter)
        {
            // for utf8 encoded string, char ASCII can be greater than 127.
            int ch = static_cast<unsigned char>(*iter);
            // ch should be same under both utf8 and utf16.
            if (should_encode(ch))
            {
                encoded.push_back('%');
                encoded.push_back(hex[(ch >> 4) & 0xF]);
                encoded.push_back(hex[ch & 0xF]);
            }
            else
            {
                // ASCII don't need to be encoded, which should be same on both utf8 and utf16.
                encoded.push_back((char)ch);
            }
        }
        return encoded;
    }

    /// <summary>
    /// Encodes a string by converting all characters except for RFC 3986 unreserved characters to their
    /// hexadecimal representation.
    /// </summary>
    xsapi_internal_string uri::encode_data_string(const xsapi_internal_string& raw)
    {
        return uri::encode_impl(raw, [](int ch) -> bool
            {
                return !uri_parser::is_unreserved(ch);
            });
    }

    xsapi_internal_string uri::encode_uri(const xsapi_internal_string& raw, uri::components::component component)
    {
        // Note: we also encode the '+' character because some non-standard implementations
        // encode the space character as a '+' instead of %20. To better interoperate we encode
        // '+' to avoid any confusion and be mistaken as a space.
        switch (component)
        {
        case components::user_info:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    return !uri_parser::is_user_info_character(ch)
                        || ch == '%' || ch == '+';
                });
        case components::host:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    // No encoding of ASCII characters in host name (RFC 3986 3.2.2)
                    return ch > 127;
                });
        case components::path:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    return !uri_parser::is_path_character(ch)
                        || ch == '%' || ch == '+';
                });
        case components::query:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    return !uri_parser::is_query_character(ch)
                        || ch == '%' || ch == '+';
                });
        case components::fragment:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    return !uri_parser::is_fragment_character(ch)
                        || ch == '%' || ch == '+';
                });
        case components::full_uri:
        default:
            return uri::encode_impl(raw, [](int ch) -> bool
                {
                    return !uri_parser::is_unreserved(ch) && !uri_parser::is_reserved(ch);
                });
        };
    }

    /// <summary>
    /// Helper function to convert a hex character digit to a decimal character value.
    /// Throws an exception if not a valid hex digit.
    /// </summary>
    static int hex_char_digit_to_decimal_char(int hex)
    {
        int decimal;
        if (hex >= '0' && hex <= '9')
        {
            decimal = hex - '0';
        }
        else if (hex >= 'A' && hex <= 'F')
        {
            decimal = 10 + (hex - 'A');
        }
        else if (hex >= 'a' && hex <= 'f')
        {
            decimal = 10 + (hex - 'a');
        }
        else
        {
            throw uri_exception("Invalid hexadecimal digit");
        }
        return decimal;
    }

    xsapi_internal_string uri::decode(const xsapi_internal_string& encoded)
    {
        xsapi_internal_string utf8raw;
        for (auto iter = encoded.begin(); iter != encoded.end(); ++iter)
        {
            if (*iter == '%')
            {
                if (++iter == encoded.end())
                {
                    throw uri_exception("Invalid URI string, two hexadecimal digits must follow '%'");
                }
                int decimal_value = hex_char_digit_to_decimal_char(static_cast<int>(*iter)) << 4;
                if (++iter == encoded.end())
                {
                    throw uri_exception("Invalid URI string, two hexadecimal digits must follow '%'");
                }
                decimal_value += hex_char_digit_to_decimal_char(static_cast<int>(*iter));

                utf8raw.push_back(static_cast<char>(decimal_value));
            }
            else
            {
                // encoded string has to be ASCII.
                utf8raw.push_back(reinterpret_cast<const char&>(*iter));
            }
        }
        return utf8raw;
    }

    std::vector<xsapi_internal_string> uri::split_path(const xsapi_internal_string& path)
    {
        std::vector<xsapi_internal_string> results;
        xsapi_internal_istringstream iss(path);
        iss.imbue(std::locale::classic());
        xsapi_internal_string s;

        while (std::getline(iss, s, '/'))
        {
            if (!s.empty())
            {
                results.push_back(s);
            }
        }

        return results;
    }

    std::map<xsapi_internal_string, xsapi_internal_string> uri::split_query(const xsapi_internal_string& query)
    {
        std::map<xsapi_internal_string, xsapi_internal_string> results;

        // Split into key value pairs separated by '&'.
        size_t prev_amp_index = 0;
        while (prev_amp_index != xsapi_internal_string::npos)
        {
            size_t amp_index = query.find_first_of('&', prev_amp_index);
            if (amp_index == xsapi_internal_string::npos)
                amp_index = query.find_first_of(';', prev_amp_index);

            xsapi_internal_string key_value_pair = query.substr(
                prev_amp_index,
                amp_index == xsapi_internal_string::npos ? query.size() - prev_amp_index : amp_index - prev_amp_index);
            prev_amp_index = amp_index == xsapi_internal_string::npos ? xsapi_internal_string::npos : amp_index + 1;

            size_t equals_index = key_value_pair.find_first_of('=');
            if (equals_index == xsapi_internal_string::npos)
            {
                continue;
            }
            else if (equals_index == 0)
            {
                xsapi_internal_string value(key_value_pair.begin() + equals_index + 1, key_value_pair.end());
                results[""] = value;
            }
            else
            {
                xsapi_internal_string key(key_value_pair.begin(), key_value_pair.begin() + equals_index);
                xsapi_internal_string value(key_value_pair.begin() + equals_index + 1, key_value_pair.end());
                results[key] = value;
            }
        }

        return results;
    }

    bool uri::validate(const xsapi_internal_string& uri_string)
    {
        return uri_parser::validate(uri_string);
    }

    uri uri::authority() const
    {
        return uri_builder().set_scheme(this->scheme()).set_host(this->host()).set_port(this->port()).set_user_info(this->user_info()).to_uri();
    }

    uri uri::resource() const
    {
        return uri_builder().set_path(this->path()).set_query(this->query()).set_fragment(this->fragment()).to_uri();
    }

    bool uri::operator == (const uri& other) const
    {
        // Each individual URI component must be decoded before performing comparison.
        // TFS # 375865

        if (this->is_empty() && other.is_empty())
        {
            return true;
        }
        else if (this->is_empty() || other.is_empty())
        {
            return false;
        }
        else if (this->scheme() != other.scheme())
        {
            // scheme is canonicalized to lowercase
            return false;
        }
        else if (uri::decode(this->user_info()) != uri::decode(other.user_info()))
        {
            return false;
        }
        else if (uri::decode(this->host()) != uri::decode(other.host()))
        {
            // host is canonicalized to lowercase
            return false;
        }
        else if (this->port() != other.port())
        {
            return false;
        }
        else if (uri::decode(this->path()) != uri::decode(other.path()))
        {
            return false;
        }
        else if (uri::decode(this->query()) != uri::decode(other.query()))
        {
            return false;
        }
        else if (uri::decode(this->fragment()) != uri::decode(other.fragment()))
        {
            return false;
        }

        return true;
    }


    uri_builder& uri_builder::append_path(const xsapi_internal_string& path, bool is_encode)
    {
        if (path.empty() || path == "/")
        {
            return *this;
        }

        auto encoded_path = is_encode ? uri::encode_uri(path, uri::components::path) : path;
        auto thisPath = this->path();
        if (thisPath.empty() || thisPath == "/")
        {
            if (encoded_path.front() != '/')
            {
                set_path("/" + encoded_path);
            }
            else
            {
                set_path(encoded_path);
            }
        }
        else if (thisPath.back() == '/' && encoded_path.front() == '/')
        {
            thisPath.pop_back();
            set_path(thisPath + encoded_path);
        }
        else if (thisPath.back() != '/' && encoded_path.front() != '/')
        {
            set_path(thisPath + "/" + encoded_path);
        }
        else
        {
            // Only one slash.
            set_path(thisPath + encoded_path);
        }
        return *this;
    }

    uri_builder& uri_builder::append_query(const xsapi_internal_string& query, bool is_encode)
    {
        if (query.empty())
        {
            return *this;
        }

        auto encoded_query = is_encode ? uri::encode_uri(query, uri::components::query) : query;
        auto thisQuery = this->query();
        if (thisQuery.empty())
        {
            this->set_query(encoded_query);
        }
        else if (thisQuery.back() == '&' && encoded_query.front() == '&')
        {
            thisQuery.pop_back();
            this->set_query(thisQuery + encoded_query);
        }
        else if (thisQuery.back() != '&' && encoded_query.front() != '&')
        {
            this->set_query(thisQuery + "&" + encoded_query);
        }
        else
        {
            // Only one ampersand.
            this->set_query(thisQuery + encoded_query);
        }
        return *this;
    }

    uri_builder& uri_builder::append(const xbox::services::uri& relative_uri)
    {
        append_path(relative_uri.path());
        append_query(relative_uri.query());
        this->set_fragment(this->fragment() + relative_uri.fragment());
        return *this;
    }

    xsapi_internal_string uri_builder::to_string()
    {
        return to_uri().to_string();
    }

    uri uri_builder::to_uri()
    {
        return uri(m_uri);
    }

    bool uri_builder::is_valid()
    {
        return uri::validate(m_uri.join());
    }

}
}
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif
