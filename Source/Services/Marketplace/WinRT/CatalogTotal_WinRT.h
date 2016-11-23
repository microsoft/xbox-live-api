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

