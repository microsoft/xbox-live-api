// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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