// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "pch_common.h"

// APIRunner uses cpprestsdk and XSAPI lib doesn't on non-GDK platforms
#if HC_PLATFORM == HC_PLATFORM_GDK
#include "cpprestsdk_impl.h"
#endif
