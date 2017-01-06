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
#include "xsapi/player_state_writer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_BEGIN

public ref class PlayerStateValue sealed
{
public:
    PlayerStateValue();
    void SetStringValue(_In_ Platform::String^ value);
    void SetNumberValue(_In_ double value);
    void SetIntegerValue(_In_ int64_t value);
    void SetBoolValue(_In_ bool value);

    DEFINE_PROP_GET_STR_OBJ(AsString, as_string);
    DEFINE_PROP_GET_OBJ(AsBool, as_bool, bool);
    DEFINE_PROP_GET_OBJ(AsNumber, as_number, double);
    DEFINE_PROP_GET_OBJ(AsInteger, as_integer, uint64_t);

internal:
    PlayerStateValue(_In_ xbox::services::player_state::player_state_value cppObj);
    const xbox::services::player_state::player_state_value& GetCppObj() const;

private:
    xbox::services::player_state::player_state_value m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_END