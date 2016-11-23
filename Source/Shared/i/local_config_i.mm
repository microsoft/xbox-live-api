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

using namespace std;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

string_t g_iOSLocalStoragePath;

string_t local_config::get_local_storage_folder()
{
    if(g_iOSLocalStoragePath.empty())
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        g_iOSLocalStoragePath = [documentsDirectory UTF8String];
        g_iOSLocalStoragePath.append(_T("/"));
    }
    return g_iOSLocalStoragePath;
}

xbox_live_result<void> local_config::read()
{
    string_t filePath = [[[NSBundle mainBundle] pathForResource:@"xboxservices" ofType:@"config"] UTF8String];
    
    if (!filePath.empty())
    {
        stringstream_t ss;
        ifstream file(filePath, ifstream::in);
        
        ss << file.rdbuf();
        string_t fileData = ss.str();
        if(!fileData.empty())
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
    }
    LOG_ERROR("ERROR: Could not find xboxservices.config");
    return xbox_live_result<void>(
        std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
        "ERROR: Could not find xboxservices.config"
        );
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

