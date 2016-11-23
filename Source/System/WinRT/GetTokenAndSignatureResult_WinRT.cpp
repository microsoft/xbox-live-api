//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "GetTokenAndSignatureResult_WinRT.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Data::Json;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN
    
GetTokenAndSignatureResult::GetTokenAndSignatureResult(
    _In_ xbox::services::system::token_and_signature_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
