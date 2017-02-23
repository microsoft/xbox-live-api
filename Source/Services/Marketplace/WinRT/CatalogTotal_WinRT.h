// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/marketplace.h"
#include "CatalogTotal_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

private ref class CatalogTotal sealed
{
public:
    DEFINE_PROP_GET_OBJ(Total, total, uint32);

internal:
    CatalogTotal(
        _In_ xbox::services::marketplace::catalog_total cppObj
        );

private:
    xbox::services::marketplace::catalog_total m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

