// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

HRESULT XalGetTitleId(
    _Out_ uint32_t* titleId
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(titleId);
    *titleId = MOCK_TITLEID;
    return S_OK;
}

size_t XalGetSandboxSize() noexcept
{
    return strlen(MOCK_SANDBOX) + 1;
}

HRESULT XalGetSandbox(
    _In_ size_t sandboxSize,
    _Out_writes_(sandboxSize) char* sandbox,
    _Out_opt_ size_t* sandboxUsed
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(sandbox);

    size_t requiredSize{ strlen(MOCK_SANDBOX) + 1 };
    RETURN_HR_INVALIDARGUMENT_IF(sandboxSize < requiredSize);

    memcpy(sandbox, MOCK_SANDBOX, requiredSize);
    if (sandboxUsed)
    {
        *sandboxUsed = requiredSize;
    }

    return S_OK;
}