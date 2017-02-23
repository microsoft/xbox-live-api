// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "CatalogItemAvailability_WinRT.h"
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogItemAvailability::CatalogItemAvailability(
    _In_ xbox::services::marketplace::catalog_item_availability cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_acceptablePaymentInstrumentTypes = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.acceptable_payment_Instrument_types())->GetView();
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ 
CatalogItemAvailability::AcceptablePaymentInstrumentTypes::get()
{
    return m_acceptablePaymentInstrumentTypes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END