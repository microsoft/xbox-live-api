// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

// TODO merge these functions with xsapi core utils
class utils_c
{
public:
    static XBL_RESULT std_bad_alloc_to_result(
        std::bad_alloc const& e, 
        _In_z_ char const* file, 
        uint32_t line
        );

    static XBL_RESULT std_exception_to_result(
        std::exception const& e, 
        _In_z_ char const* file, 
        uint32_t line
        );

    static XBL_RESULT unknown_exception_to_result(
        _In_z_ char const* file, 
        uint32_t line
        );

    static XBL_RESULT xsapi_result_from_hc_result(HC_RESULT hcr);

    static XBL_RESULT xsapi_result_from_xbox_live_result_err(std::error_code errc);

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