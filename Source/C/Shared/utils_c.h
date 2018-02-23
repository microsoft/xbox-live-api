// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

// TODO merge these functions with xsapi core utils
class utils_c
{
public:
    template<typename T, typename T2>
    static HC_RESULT execute_completion_routine_with_payload(
        _In_opt_ void* writeResultsRoutineContext,
        _In_ HC_TASK_HANDLE taskHandle,
        _In_opt_ void* completionRoutine,
        _In_opt_ void* completionRoutineContext
        )
    {
        auto args = reinterpret_cast<T*>(writeResultsRoutineContext);
        T2 typedCr = (T2)completionRoutine;

        if (typedCr != nullptr)
        {
            typedCr(args->result, args->completionRoutinePayload, completionRoutineContext);
        }
        delete args;
        return HC_OK;
    }

    template<typename T, typename T2>
    static HC_RESULT execute_completion_routine(
        _In_opt_ void* writeResultsRoutineContext,
        _In_ HC_TASK_HANDLE taskHandle,
        _In_opt_ void* completionRoutine,
        _In_opt_ void* completionRoutineContext
        )
    {
        auto args = reinterpret_cast<T*>(writeResultsRoutineContext);
        T2 typedCr = (T2)completionRoutine;

        if (typedCr != nullptr)
        {
            typedCr(args->result, completionRoutineContext);
        }
        delete args;
        return HC_OK;
    }
};