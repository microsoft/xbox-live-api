// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Configuration.h"
#include "Utils.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {


bool Configuration::s_enableDebugOutputEvents = false;
XboxNetworkMeshDiagnosticsTraceLevel Configuration::s_xboxNetworkMeshDiagnosticsTraceLevel = XboxNetworkMeshDiagnosticsTraceLevel::Off;
Concurrency::critical_section Configuration::s_writeLock;

bool Configuration::EnableDebugOutputEvents::get()
{
    return s_enableDebugOutputEvents;
}

void Configuration::EnableDebugOutputEvents::set(bool value)
{
    critical_section::scoped_lock lock(s_writeLock);
    s_enableDebugOutputEvents = value;
}

XboxNetworkMeshDiagnosticsTraceLevel Configuration::DiagnosticsTraceLevel::get()
{
    return Configuration::s_xboxNetworkMeshDiagnosticsTraceLevel;
}

void Configuration::DiagnosticsTraceLevel::set(XboxNetworkMeshDiagnosticsTraceLevel value)
{
    critical_section::scoped_lock lock(s_writeLock);

    THROW_INVALIDARGUMENT_IF(
        value < XboxNetworkMeshDiagnosticsTraceLevel::Off ||
        value > XboxNetworkMeshDiagnosticsTraceLevel::Verbose
        );

    Configuration::s_xboxNetworkMeshDiagnosticsTraceLevel = value;
}

void Configuration::RaiseDebugOutput(
    __in Platform::String^ debugOutputString
    )
{
    if( Configuration::s_enableDebugOutputEvents &&
        !debugOutputString->IsEmpty() )
    {
        DebugOutput(nullptr, debugOutputString);
    }
}

bool Configuration::IsAtDiagnosticsTraceLevel(XboxNetworkMeshDiagnosticsTraceLevel level)
{
    return (int)Configuration::s_xboxNetworkMeshDiagnosticsTraceLevel >= (int)level;
}


}}}}