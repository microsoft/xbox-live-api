// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
