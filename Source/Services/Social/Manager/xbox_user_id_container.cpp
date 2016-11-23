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
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

xbox_user_id_container::xbox_user_id_container()
{
    initialize_char_arr(m_xboxUserId);
}

xbox_user_id_container::xbox_user_id_container(
    _In_ const char_t* xboxUserId
    )
{
    utils::char_t_copy(m_xboxUserId, ARRAYSIZE(m_xboxUserId), xboxUserId);
}

const char_t*
xbox_user_id_container::xbox_user_id() const
{
    return m_xboxUserId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END