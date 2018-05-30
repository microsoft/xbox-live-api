// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <collection.h>
#include <string>
#include <set>
#include <unordered_set>

#define XBOX_LIVE_CREATORS_SDK
#include "xsapi\services.h"
#include "xsapi-c\services_c.h"
#include "system_c.h"  // This sample is using an internal header.  UWP isn't officially supported by the C API yet since this isn't the final design
