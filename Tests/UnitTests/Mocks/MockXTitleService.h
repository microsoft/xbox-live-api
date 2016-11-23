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
#include "xtitle_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockXTitleService : public xtitle_service
{
public:
    MockXTitleService();

    virtual pplx::task<xbox::services::xbox_live_result<nsal>>
    get_default_nsal(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) override;

    virtual pplx::task<xbox::services::xbox_live_result<nsal>>
    get_title_nsal(
        _In_ std::shared_ptr<auth_manager> authMan,
        _In_ const string_t& titleId,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<auth_config> authConfig
        ) override;

    nsal DefaultNsal;
    nsal TitleNsal;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

