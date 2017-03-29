// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Handles Users signing in and out and the related Xbox Live Contexts
//
#pragma once

namespace ATG
{
    class UIManager;
    class IPanel;

    class LiveResources
    {
    public:
        LiveResources();

        void Initialize(std::unique_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependantPanel = nullptr);
        void Refresh();

        std::shared_ptr<xbox::services::xbox_live_context> GetLiveContext()     const { return m_xboxLiveContext; }
        uint32_t                                           GetTitleId()         const { return m_titleId; }
        std::wstring                                       GetServiceConfigId() const { return m_scid; }
        Windows::Xbox::System::User^                       GetUser()            const { return m_user; }
        std::wstring                                       GetUserXUID()        const { return m_userXUID; }

    private:
        void OnCurrentUserChanged();
        void SetCurrentUser(Windows::Xbox::System::User^ user);
        void UpdateFirstSignedInUser();

        Windows::Xbox::System::User^                       m_user;
        std::shared_ptr<xbox::services::xbox_live_context> m_xboxLiveContext;

        // Title Info
        uint32_t                                           m_titleId;
        std::wstring                                       m_scid;
        std::wstring                                       m_userXUID;

        // UI Elements
        ATG::Legend*                                       m_gamertag;
        ATG::Image*                                        m_gamerPic;
        ATG::IPanel*                                       m_userDependentPanel;
    };
}