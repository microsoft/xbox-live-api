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
#include "GameServerImageSet_WinRT.h"

using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameServerImageSet::GameServerImageSet(
    _In_ game_server_image_set cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_tagsMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& tag : m_cppObj.tags())
    {
        m_tagsMap->Insert(ref new Platform::String(tag.first.c_str()), ref new Platform::String(tag.second.c_str()));
    }
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ 
GameServerImageSet::Tags::get()
{
    return m_tagsMap->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
