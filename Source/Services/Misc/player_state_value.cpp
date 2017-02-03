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
#include "xsapi/player_state_writer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_BEGIN

player_state_value::player_state_value() :
    m_nonStringData{}
{
}

player_state_value::player_state_value(
    _In_ string_t value
    ) :
    m_stringData(std::move(value))
{
}

player_state_value::player_state_value(
    _In_ double value
    )
{
    m_nonStringData.numberType = value;
}

player_state_value::player_state_value(
    _In_ int64_t value
    )
{
    m_nonStringData.integerType = value;
}

player_state_value::player_state_value(
    _In_ bool value
    )
{
    m_nonStringData.boolType = value;
}

string_t
player_state_value::as_string() const
{
    return m_stringData;
}

bool
player_state_value::as_bool() const
{
    return m_nonStringData.boolType;
}

double
player_state_value::as_number() const
{
    return m_nonStringData.numberType;
}

int64_t
player_state_value::as_integer() const
{
    return m_nonStringData.integerType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_END