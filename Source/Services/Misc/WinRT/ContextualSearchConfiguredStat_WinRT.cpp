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
#include "ContextualSearchConfiguredStat_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

ContextualSearchConfiguredStat::ContextualSearchConfiguredStat(
    _In_ xbox::services::contextual_search::contextual_search_configured_stat cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_valueToDisplayName = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& value : m_cppObj.value_to_display_name_map())
    {
        m_valueToDisplayName->Insert(ref new Platform::String(value.first.c_str()), ref new Platform::String(value.second.c_str()));
    }
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ 
ContextualSearchConfiguredStat::ValueToDisplayName::get()
{
    return m_valueToDisplayName->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END

