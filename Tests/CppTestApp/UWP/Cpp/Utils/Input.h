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
#include <Xinput.h>

namespace Sample
{
    private ref class Input sealed
    {
    internal:
        Input();

        void RegisterKey(_In_ Windows::System::VirtualKey key, _In_ uint32_t keyEnum);
        void ClearKeys();
        void ClearKeyHash();

        bool IsKeyDown(_In_ uint32_t keyEnum);
        void OnKeyDown(_In_ Windows::UI::Core::CoreWindow^ window, _In_ Windows::UI::Core::KeyEventArgs^ args);
        void OnKeyUp(_In_ Windows::UI::Core::CoreWindow^ window, _In_ Windows::UI::Core::KeyEventArgs^ args);
        const std::unordered_map<uint32_t, bool>& GetKeys();

        void Update();  // polls input

    private:
        std::unordered_map<uint32_t, bool> m_keyHash;   // maps an enumerated key state
        std::unordered_map<Windows::System::VirtualKey, uint32_t> m_registeredKeys;
        Concurrency::critical_section m_lock;
    };
}