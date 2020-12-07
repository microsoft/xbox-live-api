// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// RAII Win32 event wrapper
class Event
{
public:
    Event() noexcept;
    ~Event() noexcept;
    Event(const Event& other) = delete;
    Event& operator=(Event other) = delete;

    void Set() noexcept;
    void Wait() noexcept;

private:
    HANDLE m_handle{ nullptr };
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

