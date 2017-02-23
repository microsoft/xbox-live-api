// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <wrl.h>
#include <wrl/implements.h>
#include <wrl/client.h>

#include <Windows.h>
#include <Robuffer.h>

#include <mstcpip.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#include <tchar.h>
#include <collection.h>
#include <concrt.h>
#include <ppltasks.h>
#include <queue>
#include <stdlib.h>

#ifdef _XBOX_ONE
#include <xdk.h>
#include "Windows.Xbox.Chat.h"

#ifdef _TITLE
#include <d3d11_x.h>
#include <pix.h>
#endif
#endif

#include <DirectXMath.h>

#include "Common/macros.h"
