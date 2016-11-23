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
#include "xsapi/system.h"
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "SignInStatus_WinRT.h"


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>
/// Represents the result of the sign in operation.
/// </summary>
public ref class SignInResult sealed
{
public:

    /// <summary>
    /// The status of sign in operation.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Status, status, Microsoft::Xbox::Services::System::SignInStatus);

internal:
    SignInResult(
        _In_ xbox::services::system::sign_in_result cppObj
        ) : m_cppObj(cppObj)
    {}
    

private:
    xbox::services::system::sign_in_result m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
