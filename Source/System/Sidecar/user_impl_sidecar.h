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
#include "user_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class user_impl_sidecar : public user_impl
{
public:
	virtual pplx::task<xbox_live_result<sign_in_result>> sign_in_impl(
		_In_ bool showUI,
		_In_ bool forceRefresh
	) override;

	virtual pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
		internal_get_token_and_signature(
			_In_ const string_t& httpMethod,
			_In_ const string_t& url,
			_In_ const string_t& endpointForNsal,
			_In_ const string_t& headers,
			_In_ const std::vector<unsigned char>& bytes,
			_In_ bool promptForCredentialsIfNeeded,
			_In_ bool forceRefresh
		) override;

	user_impl_sidecar(Windows::System::User^ systemUser);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END