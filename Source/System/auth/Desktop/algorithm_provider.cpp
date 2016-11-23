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
#include "algorithm_provider.h"
#include <string>
#include <vector>
#include <Windows.h>
#include <bcrypt.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

algorithm_provider::algorithm_provider(
    _In_ string_t algorithmId
    )
{
    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &m_providerHandle,
        algorithmId.c_str(),
        NULL,
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("BCryptOpenAlgorithmProvider failed");
    }
}

algorithm_provider::~algorithm_provider()
{
    BCryptCloseAlgorithmProvider(m_providerHandle, 0);
}

const BCRYPT_ALG_HANDLE algorithm_provider::GetProviderHandle() const
{
    return m_providerHandle;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
