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

#if XSAPI_SIDECAR
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"
#include "system_internal.h"
#include "app_service_messages.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service 
{
	namespace client
	{
		pplx::task<xbox_live_result<sign_in_result_message>>
			sign_in(
				_In_ bool showUI,
				_In_ bool forceRefresh,
                _In_ std::shared_ptr<local_config> config
			);

    pplx::task<xbox::services::xbox_live_result<token_and_signature_result_message> >
    get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_ std::shared_ptr<local_config> config
    );

    pplx::task<xbox::services::xbox_live_result<achievements_ui_result_message>>
    show_title_achievements_ui(
        _In_ uint32_t titleId,
        _In_ std::shared_ptr<local_config> config
    );

	void set_hwnd(_In_ HWND hWnd);
	}
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif
