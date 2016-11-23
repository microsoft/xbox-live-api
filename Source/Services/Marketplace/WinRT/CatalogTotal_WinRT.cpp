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
#include "CatalogTotal_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogTotal::CatalogTotal(
    _In_ xbox::services::marketplace::catalog_total cppObj
    ) :
    m_cppObj(std::move(cppObj))
{

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END