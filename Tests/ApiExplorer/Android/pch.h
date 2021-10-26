// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#include "pch_common.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidAPIExplorerJni", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidAPIExplorerJni", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "AndroidAPIExplorerJni", __VA_ARGS__))
#define UNREFERENCED_PARAMETER(P) (P)
#define RETURN_STRING_FROM_ENUM(e) case(e): return #e;