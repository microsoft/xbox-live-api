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
#if !TV_API && defined(_WIN32)
#include <Pathcch.h>
#endif
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"
#include "a/user_impl_a.h"
#include "a/java_interop.h"

using namespace std;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_live_result<void> local_config::read()
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    string_t fileData = interop->read_config_file();
    if (!fileData.empty())
    {
        std::error_code err;
        m_jsonConfig = web::json::value::parse(fileData, err);
        if (!err)
        {
            return xbox_live_result<void>();
        }
        else
        {
            LOG_ERROR("Invalid config file");
            return xbox_live_result<void>(
                std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
                "Invalid config file"
                );
        }
    }
    else
    {
        LOG_ERROR("ERROR: Could not find xboxservices.config");
        return xbox_live_result<void>(
            std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
            "ERROR: Could not find xboxservices.config"
            );
    }
}

bool local_config::is_android_native_activity()
{
    return get_value_from_config(_T("AndroidNativeActivity"), false, _T("true")) == _T("true");
}

bool local_config::use_brokered_authorization()
{
    return get_bool_from_config(_T("UseBrokeredAuthorization"), false, false);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

