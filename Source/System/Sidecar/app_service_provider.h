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

#if UWP_API && !XSAPI_SIDECAR
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"
#include "system_internal.h"
#include "app_service_messages.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service {

	namespace provider {
		void initialize_from_incoming_connection(_In_ Windows::ApplicationModel::AppService::AppServiceConnection ^connection);
	}

class service_provider
{
public:
	static void initialize_from_incoming_connection(_In_ Windows::ApplicationModel::AppService::AppServiceConnection ^connection);

private:
	static void on_request_received(_In_ Windows::ApplicationModel::AppService::AppServiceConnection ^sender, _In_ Windows::ApplicationModel::AppService::AppServiceRequestReceivedEventArgs ^args);
	static void on_service_closed(_In_ Windows::ApplicationModel::AppService::AppServiceConnection ^sender, _In_ Windows::ApplicationModel::AppService::AppServiceClosedEventArgs ^args);

	static void sign_in(_In_ Windows::ApplicationModel::AppService::AppServiceDeferral ^deferral, _In_ const sign_in_request_message &message);
	static void get_token_and_signature(_In_ Windows::ApplicationModel::AppService::AppServiceDeferral ^deferral, _In_ const token_and_signature_request_message &message);
	static void show_title_achievements_ui(_In_ Windows::ApplicationModel::AppService::AppServiceDeferral ^deferral, _In_ const achievements_ui_request_message &message);

	static Windows::ApplicationModel::AppService::AppServiceConnection ^m_connection;
	static std::shared_ptr<xbox::services::system::xbox_live_user> m_user;
};

}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif