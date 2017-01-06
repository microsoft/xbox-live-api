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
#include "token_error.h"
#include "utils.h"

#include <string>
#include <sstream>
#include <vector>
#include <cstdint>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

token_error::token_error() :
    m_xerr(0)
{
}

token_error::token_error(
    _In_ string_t identity,
    _In_ uint32_t xerr,
    _In_ string_t redirect
    ) :
    m_identity(std::move(identity)),
    m_xerr(xerr),
    m_redirect(std::move(redirect))
{
}

#ifndef DEFAULT_MOVE_ENABLED
token_error::token_error(token_error&& other)
{
    *this = std::move(other);
}

token_error& token_error::operator=(token_error&& other)
{
    if (this != &other)
    {
        m_identity = std::move(other.m_identity);
        m_xerr = other.m_xerr;
        m_redirect = std::move(other.m_redirect);
    }

    return *this;
}
#endif

const string_t& 
token_error::identity() const
{
    return m_identity;
}

uint32_t 
token_error::xerr() const
{
    return m_xerr;
}

const string_t& 
token_error::redirect() const
{
    return m_redirect;
}

token_error
token_error::deserialize(
    _In_ const web::json::value& json
    )
{
    return token_error(
        utils::extract_json_string(json, _T("Identity"), true),
        static_cast<uint32_t>(utils::extract_json_int(json, _T("XErr"), true)),
        utils::extract_json_string(json, _T("Redirect")));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END