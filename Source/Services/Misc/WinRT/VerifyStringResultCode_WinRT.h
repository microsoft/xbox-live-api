// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>Enumeration values that indicate the result code from string verification.</summary>
public enum class VerifyStringResultCode
{
    /// <summary>No issues were found with the string.</summary>
    Success = xbox::services::system::verify_string_result_code::success,

    /// <summary>The string contains offensive content.</summary>
    Offensive = xbox::services::system::verify_string_result_code::offensive,

    /// <summary>The string is too long to verify.</summary>
    TooLong = xbox::services::system::verify_string_result_code::too_long,

    /// <summary>An unknown error was encountered during string verification.</summary>
    UnknownError = xbox::services::system::verify_string_result_code::unknown_error
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
