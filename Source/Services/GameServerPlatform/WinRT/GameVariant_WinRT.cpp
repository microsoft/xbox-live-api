// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameVariant_WinRT.h"

using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameVariant::GameVariant(
    _In_ game_variant cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
