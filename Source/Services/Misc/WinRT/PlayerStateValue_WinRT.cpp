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
#include "PlayerStateValue_WinRT.h"

using namespace xbox::services::player_state;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_BEGIN

PlayerStateValue::PlayerStateValue()
{
}

PlayerStateValue::PlayerStateValue(
    _In_ player_state_value cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

void
PlayerStateValue::SetStringValue(_In_ Platform::String^ value)
{
    m_cppObj = player_state_value(string_t(value->Data()));
}

void
PlayerStateValue::SetNumberValue(_In_ double value)
{
    m_cppObj = player_state_value(value);
}

void
PlayerStateValue::SetIntegerValue(_In_ int64_t value)
{
    m_cppObj = player_state_value(value);
}

void
PlayerStateValue::SetBoolValue(_In_ bool value)
{
    m_cppObj = player_state_value(value);
}

const xbox::services::player_state::player_state_value&
PlayerStateValue::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_END