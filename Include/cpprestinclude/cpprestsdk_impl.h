// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "cpprest/details/asyncrt_utils.hpp"
#include "cpprest/details/json_parsing.hpp"
#include "cpprest/details/json_serialization.hpp"
#include "cpprest/details/json.hpp"
#include "cpprest/details/uri.hpp"
#include "cpprest/details/uri_builder.hpp"
#include "cpprest/details/uri_parser.hpp"
#include "cpprest/details/http_msg.hpp"
#include "cpprest/details/http_helpers.hpp"
#include "cpprest/details/base64.hpp"
#include "cpprest/details/http_client_msg.hpp"

#if !XSAPI_NO_PPL
    #include "pplx/details/pplx.hpp"
    #if HC_PLATFORM == HC_PLATFORM_ANDROID
        #include "pplx/details/pplxlinux.hpp"
        #include "pplx/details/threadpool.hpp"
    #elif HC_PLATFORM == HC_PLATFORM_IOS
        #include "pplx/details/pplxapple.hpp"
    #elif HC_PLATFORM_IS_MICROSOFT
        #include "pplx/details/pplxwin.hpp"
    #endif
#endif
