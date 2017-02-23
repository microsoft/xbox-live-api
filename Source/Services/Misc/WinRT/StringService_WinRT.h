// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/system.h"
#include "VerifyStringResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>
/// Provides methods to validate a string for use with Xbox Live.
/// </summary>
public ref class StringService sealed
{
public:
    /// <summary>
    /// Verifies if a string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringToVerify">The string to verify.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The result is a VerifyStringResult object which indicates if the string contains unacceptable text.
    /// </returns>
    /// <remarks>Calls V2 GET /system/strings/validate</remarks>
    Windows::Foundation::IAsyncOperation<VerifyStringResult^>^ VerifyStringAsync(
        _In_ Platform::String^ stringToVerify
        );

    /// <summary>
    /// Verifies a collection of strings to see if each string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringsToVerify">The collection of strings to verify.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The result is a collection of VerifyStringResult objects which indicate if the strings contain unacceptable text.
    /// </returns>
    /// <remarks>Calls V2 GET /system/strings/validate</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<VerifyStringResult^>^>^ VerifyStringsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ stringsToVerify
        );

internal:
    StringService(
        xbox::services::system::string_service cppObj
        );

private:
    xbox::services::system::string_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
