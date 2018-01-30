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

#define XBOX_LIVE_CREATORS_SDK
#include "xsapi\services.h"
#include "xsapi-c\system_c.h"
#include "xsapi-c\xbox_live_context_c.h"
#include "xsapi-c\profile_c.h"
#include "xsapi-c\xbox_live_global_c.h"
#include "xsapi-c\social_c.h"