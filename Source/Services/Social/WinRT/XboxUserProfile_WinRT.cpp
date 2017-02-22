// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "XboxUserProfile_winrt.h"

using namespace pplx;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::social;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

XboxUserProfile::XboxUserProfile(
    _In_ xbox_user_profile cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END