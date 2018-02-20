// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct xbl_async_queue
{
    xbl_async_queue()
    {
        taskGroupId = xbox::services::get_xsapi_singleton()->m_nextTaskGroupId++;
    }
    uint64_t taskGroupId;
};