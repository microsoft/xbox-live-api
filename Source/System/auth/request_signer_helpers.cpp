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
#include "request_signer_helpers.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

// inserts version into buffer in network byte order
void request_signer_helpers::insert_version(
    _In_reads_(4) unsigned char* buffer,
    _In_ unsigned int version)
{
    buffer[0] = (unsigned char)((version >> 24) & 0xFF);
    buffer[1] = (unsigned char)((version >> 16) & 0xFF);
    buffer[2] = (unsigned char)((version >> 8) & 0xFF);
    buffer[3] = (unsigned char)(version & 0xFF);
}

// inserts timestamp into buffer in network byte order
void request_signer_helpers::insert_timestamp(
    _In_reads_(8) unsigned char* buffer,
    _In_ int64_t timestamp)
{
    unsigned char* pTimestamp = (unsigned char*)&timestamp;
    for (int i = 0; i < 8; i++)
    {
        buffer[i] = pTimestamp[7 - i];
    }
}

string_t request_signer_helpers::get_header_or_empty_string(
    _In_ const web::http::http_headers& headers,
    _In_ const string_t& headerName)
{
    auto it(headers.find(headerName));
    return it == headers.end() ? _T("") : it->second;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
