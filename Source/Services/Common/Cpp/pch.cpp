// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#if !_LINK_WITH_CPPRESTSDK && HC_PLATFORM != HC_PLATFORM_GDK
#include "cpprestsdk_impl.h"
#endif
#include <rapidjson/allocators.hpp>
#include "uri_impl.h"