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
#include "AppConfiguration_WinRT.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

XboxLiveAppConfiguration^ XboxLiveAppConfiguration::SingletonInstance::get()
{
    return ref new XboxLiveAppConfiguration(xbox::services::xbox_live_app_config::get_app_config_singleton());
}

XboxLiveAppConfiguration::XboxLiveAppConfiguration(
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_END
