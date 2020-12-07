// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "local_storage.h"
#include <WinBase.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

HRESULT LocalStorage::SetStoragePath(
    _In_opt_z_ const char* path
)
{
    // On Win32, if the default storage handlers are being used, the client must
    // specify a base storage path. If custom storage handlers have been specified,
    // the path will just be ignored.
    if (m_writeHandler == DefaultWrite)
    {
        if (path)
        {
            m_path = path;
            return S_OK;
        }
        else
        {
            return E_INVALIDARG;
        }
    }
    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END