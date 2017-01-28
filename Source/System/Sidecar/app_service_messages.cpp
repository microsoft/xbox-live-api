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

#if XSAPI_SIDECAR || UWP_API

#include "app_service_messages.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service {

#define MESSAGE_MEMBER_INITIALIZER(type, name) ,m_##name(in_##name)

#define APP_SERVICE_MESSAGE(type) \
	type::type( \
		uint32 request_id \
		APP_SERVICE_MESSAGE_##type(IN_PARAM) \
		) : \
	m_requestId(request_id) \
	APP_SERVICE_MESSAGE_##type(INITIALIZER) \
	{}
	
APP_SERVICE_MESSAGE_LIST

#undef APP_SERVICE_MESSAGE

template <class T>
inline typename std::enable_if<std::is_pod<T>::value && !std::is_enum<T>::value>::type serialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key, const T& value)
{
	message->Insert(key, value);
}

template <class T>
inline typename std::enable_if<std::is_enum<T>::value>::type serialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key, const T& value)
{
	message->Insert(key, static_cast<std::underlying_type<T>::type>(value));
}

template <class T>
inline typename std::enable_if<std::is_same<T, string_t>::value>::type serialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key, const T &value)
{
	message->Insert(key, ref new Platform::String(value.c_str()));
}


template <class T>
inline typename std::enable_if<std::is_pod<T>::value && !std::is_enum<T>::value, T>::type deserialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key)
{
	return safe_cast<T>(message->Lookup(key));
}

template <class T>
inline typename std::enable_if<std::is_enum<T>::value, T>::type deserialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key)
{
	return static_cast<T>(safe_cast<std::underlying_type<T>::type>(message->Lookup(key)));
}

template <class T>
inline typename std::enable_if<std::is_same<T, string_t>::value, T>::type deserialize_message_member(Windows::Foundation::Collections::ValueSet ^message, Platform::String ^key)
{
	return string_t(safe_cast<Platform::String^>(message->Lookup(key))->Data());
}

// Stamp out definitions for serialize methods
#define MESSAGE_MEMBER_SERIALIZE(type, name) serialize_message_member(message, L#name, name());
#define MESSAGE_MEMBER_DESERIALIZE(type, name) ,deserialize_message_member<type>(message, L#name)

#define APP_SERVICE_MESSAGE(type) \
	_Use_decl_annotations_ xbox_live_result<type> type::from_service_message(Windows::Foundation::Collections::ValueSet ^message) \
	{ \
		Platform::String ^messageType = L""; \
        if (message->HasKey(L"message_type")) \
        { \
            messageType = safe_cast<Platform::String^>(message->Lookup(L"message_type")); \
        } \
        \
		if (message->HasKey(L"err")) \
		{ \
			return xbox_live_result<type>( \
				std::error_code(safe_cast<int>(message->Lookup(L"err")), xbox::services::xbox_services_error_code_category()), \
				utility::conversions::utf16_to_utf8(safe_cast<Platform::String^>(message->Lookup(L"err_message"))->Data())); \
		} \
		else if (Platform::String::CompareOrdinal(messageType, L#type) != 0) \
		{ \
			throw std::runtime_error("Unexpected response type from app service"); \
		} \
		else \
		{ \
			return xbox_live_result<type>(type(\
				safe_cast<uint32>(message->Lookup(L"request_id")) \
				APP_SERVICE_MESSAGE_##type(DESERIALIZE) \
					)); \
		} \
	}

APP_SERVICE_MESSAGE_LIST

#undef APP_SERVICE_MESSAGE


// Stamp out definitions for deserialize methods
#define APP_SERVICE_MESSAGE(type) \
	Windows::Foundation::Collections::ValueSet ^type::to_service_message() const \
	{ \
		auto message = ref new Windows::Foundation::Collections::ValueSet(); \
		message->Insert(L"message_type", L#type); \
		message->Insert(L"request_id", request_id()); \
		APP_SERVICE_MESSAGE_##type(SERIALIZE) \
		return message; \
	}

APP_SERVICE_MESSAGE_LIST

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif