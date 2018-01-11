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

#include "MainPage.g.h"

namespace Social_CppCx_140
{
    class Scenarios
    {
    public:
        void Active_RealTimeActivity(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_GetUserProfileAsync(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_GetSocialRelationshipsAsync(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_GetStat(_In_ MainPage^ ui, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_WriteStat(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_GetLeaderboard(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);
        void Scenario_DeleteStat(_In_ MainPage^ ui, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext);

    private:
        Windows::Foundation::EventRegistrationToken m_devicePresenceChangeEventToken;
        Windows::Foundation::EventRegistrationToken m_titlePresenceChangeEventToken;
        static void OnTitlePresenceChange(_In_ MainPage^ ui, _In_ Microsoft::Xbox::Services::Presence::TitlePresenceChangeEventArgs^ args);
        static void OnDevicePresenceChange(_In_ MainPage^ ui, _In_ Microsoft::Xbox::Services::Presence::DevicePresenceChangeEventArgs^ args);
    };
}
