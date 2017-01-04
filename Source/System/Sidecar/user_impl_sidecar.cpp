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

#if XSAPI_SIDECAR
#include "user_impl_sidecar.h"
#include "app_service_client.h"
#include "Utils_WinRT.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

user_impl_sidecar::user_impl_sidecar(Windows::System::User^)
{

}

pplx::task<xbox_live_result<sign_in_result>> user_impl_sidecar::sign_in_impl(
	_In_ bool showUI,
	_In_ bool forceRefresh
)
{
	std::weak_ptr<user_impl_sidecar> thisWeakPtr = std::dynamic_pointer_cast<user_impl_sidecar>(shared_from_this());
	auto task = app_service::client::sign_in(showUI, forceRefresh, m_localConfig)
	.then([thisWeakPtr](xbox_live_result<app_service::sign_in_result_message> result)
	{
		std::shared_ptr<user_impl_sidecar> pThis(thisWeakPtr.lock());
		if (pThis == nullptr)
		{
			return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "user_impl shutting down");
		}

		if (result.err())
		{
			return xbox_live_result<sign_in_result>(result.err(), result.err_message());
		}
		else
		{
			const auto& payload = result.payload();
			if (payload.status() == sign_in_status::success)
			{
				pThis->user_signed_in(payload.xbox_user_id(), payload.gamertag(), payload.age_group(), payload.privileges(), payload.web_account_id());
			}

			return xbox_live_result<sign_in_result>(payload.status());
		}
	});

	return utils::create_exception_free_task<sign_in_result>(task);
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
user_impl_sidecar::internal_get_token_and_signature(
	_In_ const string_t& httpMethod,
	_In_ const string_t& url,
	_In_ const string_t& endpointForNsal,
	_In_ const string_t& headers,
	_In_ const std::vector<unsigned char>& bytes,
	_In_ bool promptForCredentialsIfNeeded,
	_In_ bool forceRefresh
)
{
	std::weak_ptr<user_impl_sidecar> thisWeakPtr = std::dynamic_pointer_cast<user_impl_sidecar>(shared_from_this());
	auto task = app_service::client::get_token_and_signature(
		httpMethod,
		url,
		endpointForNsal,
		headers,
		bytes,
		promptForCredentialsIfNeeded,
		forceRefresh,
        m_localConfig
	)
	.then([thisWeakPtr](xbox_live_result<app_service::token_and_signature_result_message> result)
	{
		std::shared_ptr<user_impl_sidecar> pThis(thisWeakPtr.lock());
		if (pThis == nullptr)
		{
			return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "user_impl shutting down");
		}

		if (result.err())
		{
			return xbox_live_result<token_and_signature_result>(result.err(), result.err_message());
		}
		else
		{
			const auto& payload = result.payload();
			return xbox_live_result<token_and_signature_result>(token_and_signature_result(
				std::move(payload.token()),
				std::move(payload.signature()),
				std::move(payload.xbox_user_id()),
				std::move(payload.gamertag()),
				std::move(payload.xbox_user_hash()),
				std::move(payload.age_group()),
				std::move(payload.privileges()),
				std::move(payload.web_account_id()),
				std::move(payload.reserved())
			));
		}
	});

	return utils::create_exception_free_task<token_and_signature_result>(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif