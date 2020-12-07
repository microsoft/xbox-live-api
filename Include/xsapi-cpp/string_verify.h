// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#include "xsapi-c/string_verify_c.h"

namespace xbox {
namespace services {
    class xbox_live_context;
namespace system {
/// <summary>Enumeration values that indicate the result code from string verification.
/// These values are defined on the service side and should not be modified.
/// </summary>
enum class verify_string_result_code
{
    /// <summary>No issues were found with the string.</summary>
    success = (int)XblVerifyStringResultCode::Success,

    /// <summary>The string contains offensive content.</summary>
    offensive = (int)XblVerifyStringResultCode::Offensive,

    /// <summary>The string is too long to verify.</summary>
    too_long = (int)XblVerifyStringResultCode::TooLong,

    /// <summary>An unknown error was encountered during string verification.</summary>
    unknown_error = (int) XblVerifyStringResultCode::UnknownError
};

/// <summary>
/// Contains information about the results of a string verification.
/// </summary>
class verify_string_result
{
public:
    /// <summary>
    /// The result code for the string verification.
    /// </summary>
    inline verify_string_result_code result_code() const;

    /// <summary>
    /// first_offending_substring() contains the first offending substring if the
    /// result code is verify_string_result_code::offensive.
    /// </summary>
    inline const string_t& first_offending_substring() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline verify_string_result();

    /// <summary>
    /// Internal function
    /// </summary>
    inline verify_string_result(
        XblVerifyStringResultCode resultCode,
        const char* firstOffendingSubstring
    );

private:
    verify_string_result_code m_resultCode;
    string_t m_first_offending_substring;
};


/// <summary>
/// Provides methods to validate a string for use with Xbox live.
/// </summary>
class string_service
{
public:

    /// <summary>
    /// Verifies if a string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringToVerify">The string to verify.</param>
    /// <returns>
    /// A verify_string_result object which indicates if the string contains unacceptable text.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// 
    /// Calls V2 GET /system/strings/validate
    /// </remarks>
    inline pplx::task<xbox_live_result<verify_string_result>> verify_string(_In_ const string_t& stringToVerify);

    /// <summary>
    /// Verifies a collection of strings to see if each string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringsToVerify">The collection of strings to verify.</param>
    /// <returns>
    /// A collection of verify_string_result objects which indicate if the strings contain unacceptable text.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// 
    /// Calls V2 GET /system/strings/validate
    /// </remarks>
    inline pplx::task<xbox_live_result<std::vector<verify_string_result>>> verify_strings(_In_ const std::vector<string_t>& stringsToVerify);

    inline string_service(const string_service& other);
    inline string_service& operator=(string_service other);
    inline ~string_service();

private:
    inline string_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext;
    friend xbox_live_context;
};
}}}

#include "impl/string_verify.hpp"

