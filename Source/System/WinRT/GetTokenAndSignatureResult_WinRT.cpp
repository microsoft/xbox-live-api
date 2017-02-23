// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
