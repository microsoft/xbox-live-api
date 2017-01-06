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
#include "xsapi/system.h"
#include "shared_macros.h"
#include "cpprest/http_msg.h"
#include "nsal.h"
#include "auth_manager.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class xtitle_service
{
public:
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
    get_default_nsal(
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) = 0;

    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
    get_title_nsal(
        _In_ std::shared_ptr<auth_manager> authMan,
        _In_ const string_t& titleId,
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) = 0;
};

class xtitle_service_impl : public xtitle_service
{
public:
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
    get_default_nsal(
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) override;

    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
    get_title_nsal(
        _In_ std::shared_ptr<auth_manager> authMan,
        _In_ const string_t& titleId,
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) override;

};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

