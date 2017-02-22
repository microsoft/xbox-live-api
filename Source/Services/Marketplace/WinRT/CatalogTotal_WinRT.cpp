// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "CatalogTotal_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogTotal::CatalogTotal(
    _In_ xbox::services::marketplace::catalog_total cppObj
    ) :
    m_cppObj(std::move(cppObj))
{

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END