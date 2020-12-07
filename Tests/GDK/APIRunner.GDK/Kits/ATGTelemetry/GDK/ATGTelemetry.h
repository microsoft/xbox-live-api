//--------------------------------------------------------------------------------------
// File: ATGTelemetry.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright(c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once

namespace ATG
{
// To opt-out of telemetry uncomment the following line
// #undef ATG_ENABLE_TELEMETRY

#ifdef ATG_ENABLE_TELEMETRY
    // Sends sample launch telemetry
    void SendLaunchTelemetry();
#else
    void SendLaunchTelemetry() {};
#endif

}