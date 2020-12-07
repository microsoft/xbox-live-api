// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "xbox_live_context_internal.h"
#include "xsapi-c/http_call_c.h"
#include "http_call_legacy.h"

using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
xbox_live_user_t xbox_system_factory::httpCallUserHandle;
std::shared_ptr<legacy::http_call>
xbox_system_factory::create_http_call(
    _In_ const std::shared_ptr<XboxLiveContextSettings>& xboxLiveContextSettings,
    _In_ const string_t& _httpMethod,
    _In_ const string_t& _serverName,
    _In_ const xbox::services::uri& pathQueryFragment,
    _In_ xbox_live_api xboxLiveApi
)
{
    UNREFERENCED_PARAMETER(xboxLiveContextSettings);
    UNREFERENCED_PARAMETER(xboxLiveApi);
    if (xbox_system_factory::httpCallUserHandle == 0)
    {
        return nullptr;
    }

    auto httpMethod_str = StringFromStringT(_httpMethod);
    auto fullUrl = _serverName;
    if (!pathQueryFragment.is_empty())
    {
        fullUrl += pathQueryFragment.to_string();
    }
    auto fullUrl_str = StringFromStringT(fullUrl);
    XblContextHandle contextHandle;
    XblContextCreateHandle(xbox_system_factory::httpCallUserHandle, &contextHandle);


    XblHttpCallHandle callHandle;
    auto hr = XblHttpCallCreate(contextHandle, httpMethod_str.c_str(), fullUrl_str.c_str(), &callHandle);
    XblContextCloseHandle(contextHandle);

    if (FAILED(hr))
    {
        return nullptr;
    }

    web::uri_builder builder;
    builder.append(pathQueryFragment.to_string());
    web::uri tempPathQuery = builder.to_uri();
    return std::make_shared<legacy::http_call>(callHandle, _httpMethod, _serverName, tempPathQuery);
}

std::shared_ptr<xbox_system_factory> 
xbox_system_factory::get_factory()
{
    auto xsapiSingleton{ xbox::services::get_xsapi_singleton() };
    if (xsapiSingleton)
    {
        return xsapiSingleton->m_factoryInstance;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

void xbox_system_factory::set_factory(
    _In_ std::shared_ptr<xbox_system_factory> factory
    )
{
    auto xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton != nullptr)
    {
        std::lock_guard<std::mutex> hold(xsapiSingleton->m_singletonLock);
        xsapiSingleton->m_factoryInstance = factory;
    }
}

/*static*/ void xbox_system_factory::set_http_call_user(xbox_live_user_t user)
{
    xbox_system_factory::httpCallUserHandle = user;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
