// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "QualityOfServiceServer_WinRT.h"

using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

QualityOfServiceServer::QualityOfServiceServer(
    _In_ quality_of_service_server cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::game_server_platform::quality_of_service_server& 
QualityOfServiceServer::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
