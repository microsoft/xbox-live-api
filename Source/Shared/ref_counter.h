// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Class for managing client ref count to Xbox Live objects. All types that need client references
// counted should inherit from this class
struct RefCounter
{
    RefCounter() = default;
    virtual ~RefCounter() = default;

    void AddRef();
    void DecRef();

protected:
    virtual std::shared_ptr<RefCounter> GetSharedThis() = 0;

private:
    std::atomic<uint32_t> m_refCount{ 0 };
    std::shared_ptr<RefCounter> m_extraRefHolder;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END