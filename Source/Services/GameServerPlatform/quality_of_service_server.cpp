// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

quality_of_service_server::quality_of_service_server()
{
}

quality_of_service_server::quality_of_service_server(
    _In_ string_t serverFullQualifiedDomainName,
    _In_ string_t secureDeviceAddressBase64,
    _In_ string_t targetLocation
    ) :
    m_serverFullQualifiedDomainName(std::move(serverFullQualifiedDomainName)),
    m_secureDeviceAddressBase64(std::move(secureDeviceAddressBase64)),
    m_targetLocation(std::move(targetLocation))
{
}

const string_t& 
quality_of_service_server::server_full_qualified_domain_name() const
{
    return m_serverFullQualifiedDomainName;
}

const string_t& 
quality_of_service_server::secure_device_address_base64() const
{
    return m_secureDeviceAddressBase64;
}

const string_t& 
quality_of_service_server::target_location() const
{
    return m_targetLocation;
}

xbox_live_result<quality_of_service_server>
quality_of_service_server::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<quality_of_service_server>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = quality_of_service_server(
        utils::extract_json_string(json, _T("serverFqdn"), errc),
        utils::extract_json_string(json, _T("serverSecureDeviceAddress"), errc),
        utils::extract_json_string(json, _T("targetLocation"), errc)
        );

    return xbox_live_result<quality_of_service_server>(result, errc);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
