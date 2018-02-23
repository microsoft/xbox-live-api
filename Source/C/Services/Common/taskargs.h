// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#include "xsapi-c/errors_c.h"

struct taskargs
{
    virtual ~taskargs() {}

    template<typename T>
    void copy_xbox_live_result(xbox::services::xbox_live_result<T> cppResult)
    {
        result = utils::create_xbl_result(cppResult.err());
    }
    XBL_RESULT result;
};

template<typename T>
struct taskargs_with_payload : public taskargs
{
    // completionRoutinePayload is the final result object that will be passed to the completion function
    T completionRoutinePayload;
};