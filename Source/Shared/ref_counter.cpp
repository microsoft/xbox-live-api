// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ref_counter.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void RefCounter::AddRef()
{
    if (m_refCount++ == 0)
    {
        m_extraRefHolder = GetSharedThis();
    }
}

void RefCounter::DecRef()
{
    if (--m_refCount == 0)
    {
        m_extraRefHolder.reset();
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END