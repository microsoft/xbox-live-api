// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
