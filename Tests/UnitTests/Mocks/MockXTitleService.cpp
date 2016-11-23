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
#include "MockXTitleService.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockXTitleService::MockXTitleService()
    : DefaultNsal(nsal::deserialize(StockMocks::GetStockNsal()))
{
}

pplx::task<xbox::services::xbox_live_result<nsal>>
MockXTitleService::get_default_nsal(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::system::auth_config> authConfig
    )
{
    return pplx::task_from_result(xbox::services::xbox_live_result<nsal>(DefaultNsal));
}

pplx::task<xbox::services::xbox_live_result<nsal>>
MockXTitleService::get_title_nsal(
    _In_ std::shared_ptr<auth_manager> authMan,
    _In_ const string_t& titleId,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<auth_config> authConfig
    )
{
    return pplx::task_from_result(xbox::services::xbox_live_result<nsal>(TitleNsal));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

