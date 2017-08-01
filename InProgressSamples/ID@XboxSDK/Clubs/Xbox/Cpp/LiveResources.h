// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

namespace ATG
{
    class UIManager;
    class IPanel;

    typedef int32_t function_context;

    class LiveResources
    {
    public:
        LiveResources();

        void Initialize(std::shared_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependantPanel = nullptr, ATG::IPanel *nouserDependantPanel = nullptr);
        void SignIn();
        void Refresh();

        std::shared_ptr<xbox::services::xbox_live_context> GetLiveContext()     const { return m_xboxLiveContext; }
        uint32_t                                           GetTitleId()         const { return m_titleId; }
        std::wstring                                       GetServiceConfigId() const { return m_scid; }

        std::shared_ptr<xbox::services::system::xbox_live_user> GetUser()       const { return m_user; }

        function_context add_signin_handler(_In_ std::function<void(std::shared_ptr<xbox::services::system::xbox_live_user>, xbox::services::system::sign_in_status)> handler);
        void remove_signin_handler(_In_ function_context context);

    private:
        void _Raise_service_call_routed_event(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ xbox::services::system::sign_in_status result);
        void HandleSignInResult(xbox::services::xbox_live_result<xbox::services::system::sign_in_result> &t);
        void UpdateCurrentUser();

        std::shared_ptr<xbox::services::system::xbox_live_user> m_user;
        std::shared_ptr<xbox::services::xbox_live_context> m_xboxLiveContext;

        // Title Info
        uint32_t                                           m_titleId;
        std::wstring                                       m_scid;

        // UI Elements
        ATG::Legend*                                       m_gamertag;
        ATG::Image*                                        m_gamerPic;
        ATG::IPanel*                                       m_userDependentPanel;
        ATG::IPanel*                                       m_nouserDependentPanel;
        ATG::TextLabel*                                    m_sandboxLabel;
        ATG::TextLabel*                                    m_titleIdLabel;
        ATG::TextLabel*                                    m_scidLabel;
        ATG::TextLabel*                                    m_signInErrorLabel;

        std::mutex m_writeLock;
        std::unordered_map<function_context, std::function<void(std::shared_ptr<xbox::services::system::xbox_live_user>, xbox::services::system::sign_in_status)>> m_signinRoutedHandlers;
        function_context m_signinRoutedHandlersCounter;
    };
}