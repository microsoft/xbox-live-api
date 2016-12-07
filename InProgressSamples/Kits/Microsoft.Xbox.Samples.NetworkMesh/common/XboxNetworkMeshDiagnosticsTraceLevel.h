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

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

/// <summary>
/// Specifies what messages to output for the Xbox Services classes
/// </summary>
public enum class XboxNetworkMeshDiagnosticsTraceLevel
{ 
    /// <summary>
    /// Output no tracing and debugging messages.
    /// </summary>
    Off = 0,

    /// <summary>
    /// Output error-handling messages.
    /// </summary>
    Error,

    /// <summary>
    /// Output warnings and error-handling messages.
    /// </summary>
    Warning,

    /// <summary>
    /// Output informational messages, warnings, and error-handling messages.
    /// </summary>
    Info,

    /// <summary>
    /// Output all debugging and tracing messages.
    /// </summary>
    Verbose
};

}}}}
