// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class xbox_live_mutex
{
public:
    xbox_live_mutex();
    xbox_live_mutex(_In_ const xbox_live_mutex& other);
    xbox_live_mutex operator=(_In_ const xbox_live_mutex& other);
    std::mutex& get();
private:
    std::mutex m_xboxLiveMutex;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END