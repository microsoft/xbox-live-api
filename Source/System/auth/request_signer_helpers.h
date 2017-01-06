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
#include "cpprest/http_msg.h"

#include <string>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

/// <summary>
/// Platform agnostic helpers for request signing.
/// </summary>
class request_signer_helpers
{
public:
    static string_t get_header_or_empty_string(
        _In_ const web::http::http_headers& headers, 
        _In_ const string_t& headerName);

    static void insert_version(
        _In_reads_(4) unsigned char* buffer, 
        _In_ unsigned int version);

    static void insert_timestamp(
        _In_reads_(8) unsigned char* buffer, 
        _In_ int64_t timestamp);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
