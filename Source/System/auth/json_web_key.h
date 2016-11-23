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
#include "shared_macros.h"
#include "Ecdsa.h"
#include "cpprest/json.h"

#include <string>
#include <stdint.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class json_web_key
{
public:
    static web::json::value serialize_json_web_key(_In_ std::shared_ptr<ecdsa> eccKey);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

