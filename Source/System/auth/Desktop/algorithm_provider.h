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
#include <string>
#include <Windows.h>
#include <bcrypt.h>
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class algorithm_provider
{
public:
    algorithm_provider(_In_ string_t algorithmId);
    ~algorithm_provider();

    const BCRYPT_ALG_HANDLE GetProviderHandle() const;

private:
    BCRYPT_ALG_HANDLE m_providerHandle;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
