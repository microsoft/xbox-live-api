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
#include "MultiplayerSessionArbitrationServer_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace xbox::services;
using namespace xbox::services::tournaments;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionArbitrationServer::MultiplayerSessionArbitrationServer(
    _In_ xbox::services::multiplayer::multiplayer_session_arbitration_server cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_results = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<TournamentTeamResult>(m_cppObj.results());
}

Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^
MultiplayerSessionArbitrationServer::Results::get()
{
    return m_results->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END