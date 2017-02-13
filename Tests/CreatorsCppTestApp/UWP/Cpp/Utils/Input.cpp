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
#include "Input.h"
using namespace Sample;
using namespace Concurrency;

Input::Input()
{
}

void 
Input::OnKeyDown(
    _In_ Windows::UI::Core::CoreWindow^ window,
    _In_ Windows::UI::Core::KeyEventArgs^ args
    )
{
    critical_section::scoped_lock lock(m_lock);

    Windows::System::VirtualKey key = args->VirtualKey;
    if (m_registeredKeys.count(key))
    {
        uint32_t registeredKeyEnum = m_registeredKeys[key];
        m_keyHash[registeredKeyEnum] = true;
    }
}

void
Input::OnKeyUp(
    _In_ Windows::UI::Core::CoreWindow^ window,
    _In_ Windows::UI::Core::KeyEventArgs^ args
)
{
    critical_section::scoped_lock lock(m_lock);

    Windows::System::VirtualKey key = args->VirtualKey;
    if (m_registeredKeys.count(key))
    {
        uint32_t registeredKeyEnum = m_registeredKeys[key];
        m_keyHash[registeredKeyEnum] = false;
    }
}

void
Input::RegisterKey(
    _In_ Windows::System::VirtualKey key,
    _In_ uint32_t keyEnum
    )
{
    critical_section::scoped_lock lock(m_lock);

    m_registeredKeys[key] = keyEnum;
}

void 
Input::ClearKeys()
{
    critical_section::scoped_lock lock(m_lock);

    m_registeredKeys.clear();
    m_keyHash.clear();
}

void
Input::ClearKeyHash()
{
    critical_section::scoped_lock lock(m_lock);
    m_keyHash.clear();
}

bool 
Input::IsKeyDown(
    _In_ uint32_t keyEnum
    )
{
    critical_section::scoped_lock lock(m_lock);

    return m_keyHash[keyEnum];
}

const std::unordered_map<uint32_t, bool>&
Input::GetKeys()
{
    critical_section::scoped_lock lock(m_lock);

    return m_keyHash;
}

void 
Input::Update()
{
    // will poll for xinput when availible
}