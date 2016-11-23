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
#include "ContextualSearchBroadcast_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

ContextualSearchBroadcast::ContextualSearchBroadcast(
    _In_ xbox::services::contextual_search::contextual_search_broadcast cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_currentStats = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& stat : m_cppObj.current_stats())
    {
        m_currentStats->Insert(ref new Platform::String(stat.first.c_str()), ref new Platform::String(stat.second.c_str()));
    }
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ 
ContextualSearchBroadcast::CurrentStats::get()
{
    return m_currentStats->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
