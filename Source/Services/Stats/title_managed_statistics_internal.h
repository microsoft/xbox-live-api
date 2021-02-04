// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/title_managed_statistics_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

struct TitleManagedStatistic
{
    TitleManagedStatistic(const XblTitleManagedStatistic& s) noexcept
        : name{ s.statisticName },
        type{ static_cast<Type>(s.statisticType) },
        numberValue{ s.numberValue },
        stringValue{ s.stringValue ? s.stringValue : "" }
    {
    }

    // The statswrite PATCH endpoint allows stat deletions by writing a null
    // JSON value for an existing statname. This constructor is used to create a
    // a "NULL" TitleManagedStatistic to be used for deletion.
    TitleManagedStatistic(const char* nameToDelete) noexcept
        : name{ nameToDelete },
        type{ Type::Null }
    {
    }

    String name;
    enum class Type : uint32_t
    {
        Number = static_cast<uint32_t>(XblTitleManagedStatType::Number),
        String = static_cast<uint32_t>(XblTitleManagedStatType::String),
        Null
    } type;
    double numberValue{ 0.0 };
    String stringValue;
};

class TitleManagedStatisticsService: public std::enable_shared_from_this<TitleManagedStatisticsService>
{
public:
    TitleManagedStatisticsService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
    ) noexcept;

    HRESULT WriteTitleManagedStatisticsAsync(
        _In_ const Vector<TitleManagedStatistic>& statistics,
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

    HRESULT UpdateTitleManagedStatistics(
        _In_ const Vector<TitleManagedStatistic>& statistics,
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

private:
    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    String m_statswritePath;

    static uint64_t GetRevisionFromClock() noexcept;

    static JsonDocument SerializeStatistics(
        const Vector<TitleManagedStatistic>& stats
    ) noexcept;
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END