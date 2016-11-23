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
#include "pch.h"
#include "xsapi/system.h"
#include "VerifyStringResultCode_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>
/// Contains information about the results of a string verification.
/// </summary>
public ref class VerifyStringResult sealed
{
public:
    /// <summary>
    /// The result code for the string verification.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ResultCode, result_code, VerifyStringResultCode);

    /// <summary>
    /// FirstOffendingSubstring contains the first offending substring found if the
    /// result code is VerifyStringResultCode::Offensive.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(FirstOffendingSubstring, first_offending_substring);

internal:
    VerifyStringResult(
        _In_ xbox::services::system::verify_string_result cppObj
        ):
        m_cppObj(std::move(cppObj))
    {}

private:
    xbox::services::system::verify_string_result m_cppObj;

};
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
