// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class web_socket_connection_state : int32_t
{
    disconnected,
    activated,
    connecting,
    connected,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END