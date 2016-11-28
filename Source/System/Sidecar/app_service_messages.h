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

#if UWP_API || XSAPI_SIDECAR
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service {

#define APP_SERVICE_MESSAGE_LIST \
	APP_SERVICE_MESSAGE(sign_in_request_message) \
	APP_SERVICE_MESSAGE(sign_in_result_message) \
	APP_SERVICE_MESSAGE(token_and_signature_request_message) \
	APP_SERVICE_MESSAGE(token_and_signature_result_message) \
	APP_SERVICE_MESSAGE(achievements_ui_request_message) \
	APP_SERVICE_MESSAGE(achievements_ui_result_message)

#define APP_SERVICE_MESSAGE_sign_in_request_message(format) \
	MESSAGE_MEMBER(bool, show_ui, format)  \
	MESSAGE_MEMBER(bool, force_refresh, format) \

#define APP_SERVICE_MESSAGE_sign_in_result_message(format) \
	MESSAGE_MEMBER(xbox::services::system::sign_in_status, status, format) \
	MESSAGE_MEMBER(string_t, xbox_user_id, format) \
	MESSAGE_MEMBER(string_t, gamertag, format) \
	MESSAGE_MEMBER(string_t, age_group, format) \
	MESSAGE_MEMBER(string_t, privileges, format) \
	MESSAGE_MEMBER(string_t, web_account_id, format)

#define APP_SERVICE_MESSAGE_token_and_signature_request_message(format) \
	MESSAGE_MEMBER(string_t, http_method, format) \
	MESSAGE_MEMBER(string_t, url, format) \
	MESSAGE_MEMBER(string_t, endpoint_for_nsal, format) \
	MESSAGE_MEMBER(string_t, headers, format) \
	MESSAGE_MEMBER(string_t, body_base64, format) \
	MESSAGE_MEMBER(bool, prompt_for_credentials_if_needed, format) \
	MESSAGE_MEMBER(bool, force_refresh, format)

#define APP_SERVICE_MESSAGE_token_and_signature_result_message(format) \
	MESSAGE_MEMBER(string_t, token, format) \
	MESSAGE_MEMBER(string_t, signature, format) \
	MESSAGE_MEMBER(string_t, xbox_user_id, format) \
	MESSAGE_MEMBER(string_t, gamertag, format) \
	MESSAGE_MEMBER(string_t, xbox_user_hash, format) \
	MESSAGE_MEMBER(string_t, age_group, format) \
	MESSAGE_MEMBER(string_t, privileges, format) \
	MESSAGE_MEMBER(string_t, web_account_id, format) \
	MESSAGE_MEMBER(string_t, reserved, format)

#define APP_SERVICE_MESSAGE_achievements_ui_request_message(format) \
	MESSAGE_MEMBER(uint32, title_id, format)

#define APP_SERVICE_MESSAGE_achievements_ui_result_message(format)

#define MESSAGE_MEMBER(type, name, format) MESSAGE_MEMBER_##format(type, name)

#define MESSAGE_MEMBER_IN_PARAM(type, name) ,_In_ type in_##name
#define MESSAGE_MEMBER_ACCESSOR(type, name) const type& name() const { return m_##name; }
#define MESSAGE_MEMBER_FIELD(type, name) type m_##name;

#define APP_SERVICE_MESSAGE(type) \
class type \
{ \
public: \
	type() {} \
	type( \
		uint32 requestId \
		APP_SERVICE_MESSAGE_##type(IN_PARAM) \
		); \
	APP_SERVICE_MESSAGE_##type(ACCESSOR) \
	uint32 request_id() const { return m_requestId; } \
	Windows::Foundation::Collections::ValueSet ^to_service_message() const; \
	static xbox::services::xbox_live_result<type> from_service_message(_In_ Windows::Foundation::Collections::ValueSet ^message); \
	private: \
	uint32 m_requestId; \
	APP_SERVICE_MESSAGE_##type(FIELD) \
};

	APP_SERVICE_MESSAGE_LIST

#undef APP_SERVICE_MESSAGE

	template <class REQUEST> struct response_type { typedef void value; };
	template <> struct response_type<sign_in_request_message> { typedef sign_in_result_message value; };
	template <> struct response_type<token_and_signature_request_message> { typedef token_and_signature_result_message value; };
	template <> struct response_type<achievements_ui_request_message> { typedef achievements_ui_result_message value; };

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif

