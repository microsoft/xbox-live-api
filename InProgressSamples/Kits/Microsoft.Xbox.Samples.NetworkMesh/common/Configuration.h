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
#include "macros.h"
#include "XboxNetworkMeshDiagnosticsTraceLevel.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class Configuration sealed
{
public:

    /// <summary>
    /// Registers for all DebugOutput notifications 
    /// </summary>
    static event Windows::Foundation::EventHandler<Platform::String^>^ DebugOutput;

    /// <summary>
    /// Indicates if events should be generated for DebugOutput
    /// </summary>
    static property bool EnableDebugOutputEvents
    { 
        bool get();
        void set(bool value);
    }

    /// <summary>
    /// Indicates the level of debug messages sent to the debugger's output window.
    /// This property can be used to override the XboxLiveContextSettings::DiagnosticsTraceLevel
    /// for all XboxLiveContexts.  In addition, the setting is the only one that enables
    /// tracing for internally thrown exceptions.
    /// </summary>
    static property XboxNetworkMeshDiagnosticsTraceLevel DiagnosticsTraceLevel 
    { 
        XboxNetworkMeshDiagnosticsTraceLevel get();
        void set(XboxNetworkMeshDiagnosticsTraceLevel value);
    }

internal:
    static void RaiseDebugOutput( __in Platform::String^ debugOutputString );
    static bool IsAtDiagnosticsTraceLevel( XboxNetworkMeshDiagnosticsTraceLevel level );

private:
    static Concurrency::critical_section s_writeLock;
    static bool s_enableDebugOutputEvents;
    static Microsoft::Xbox::Samples::NetworkMesh::XboxNetworkMeshDiagnosticsTraceLevel s_xboxNetworkMeshDiagnosticsTraceLevel;
};

}}}}