// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"
#include "StatisticEventType_WinRT.h"
#include "StatisticEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary> 
/// Represents a statistic event.
/// </summary>
public ref class StatisticEvent sealed
{
public:
    /// <summary>
    /// The Xbox Live user this event is for.
    /// </summary>
    property XboxLiveUser_t User { XboxLiveUser_t get(); }

    /// <summary> 
    /// The type of event the statistic is.
    /// </summary>
    /// <return>The event type</return>
    DEFINE_PROP_GET_ENUM_OBJ(EventType, event_type, StatisticEventType);

    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    property int ErrorCode
    {
        int get();
    };

    /// <summary>
    /// Returns call specific debug information if join fails.
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    property Platform::String^ ErrorMessage
    {
        Platform::String^ get();
    };

    /// <summary> 
    /// The data of event from stats manager.
    /// </summary>
    property StatisticEventArgs^ EventArgs
    {
        StatisticEventArgs^ get();
    };

internal:
    StatisticEvent(_In_ xbox::services::stats::manager::stat_event cppObj);

private:
    XboxLiveUser_t m_user;
    xbox::services::stats::manager::stat_event m_cppObj;
    Platform::Exception^ m_errorCode;
    Platform::String^ m_errorMessage;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END
