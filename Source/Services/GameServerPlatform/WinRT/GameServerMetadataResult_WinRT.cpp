// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameServerMetadataResult_WinRT.h"

using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameServerMetadataResult::GameServerMetadataResult(
    _In_ xbox::services::game_server_platform::game_server_metadata_result cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
    m_gameVariants = ref new Vector<GameVariant^>();
    for (const auto& i : m_cppObj.game_variants())
    {
        m_gameVariants->Append(ref new GameVariant(i));
    }

    m_gameServerImageSets = ref new Vector<GameServerImageSet^>();
    for (const auto& i : m_cppObj.game_server_image_sets())
    {
        m_gameServerImageSets->Append(ref new GameServerImageSet(i));
    }
}

Windows::Foundation::Collections::IVectorView<GameVariant^>^ 
GameServerMetadataResult::GameVariants::get()
{
    return m_gameVariants->GetView();
}

Windows::Foundation::Collections::IVectorView<GameServerImageSet^>^ 
GameServerMetadataResult::GameServerImageSets::get()
{
    return m_gameServerImageSets->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
