//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/simple_stats.h"
#include "StatisticEventType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public ref class StatisticEvent sealed
{
public:
    /// <summary>
    /// The Xbox Live user this event is for
    /// </summary>
    property XboxLiveUser_t User { XboxLiveUser_t get(); }

    /// <summary> 
    /// The type of event the statistic is
    /// </summary>
    /// <return>The event type</return>
    DEFINE_PROP_GET_ENUM_OBJ(EventType, event_type, StatisticEventType);    // TODO: uncomment

    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    property int ErrorCode
    {
        int get();
    };

    /// <summary>
    /// Returns call specific debug information if join fails
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    property Platform::String^ ErrorMessage
    {
        Platform::String^ get();
    };

internal:
    StatisticEvent(_In_ xbox::services::experimental::stats::manager::stat_event cppObj);

private:
    XboxLiveUser_t m_user;
    xbox::services::experimental::stats::manager::stat_event m_cppObj;
    Platform::Exception^ m_errorCode;
    Platform::String^ m_errorMessage;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END
