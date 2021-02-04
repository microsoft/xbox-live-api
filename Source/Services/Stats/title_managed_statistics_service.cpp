// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "title_managed_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

TitleManagedStatisticsService::TitleManagedStatisticsService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
) noexcept :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings))
{
    Stringstream ss;
    ss << "/stats/users/" << m_user.Xuid() << "/scids/" << AppConfig::Instance()->Scid();
    m_statswritePath = ss.str();
}

HRESULT TitleManagedStatisticsService::WriteTitleManagedStatisticsAsync(
    _In_ const Vector<TitleManagedStatistic>& statistics,
    _In_ AsyncContext<HRESULT> async
) const noexcept
{
    JsonDocument request{ SerializeStatistics(statistics) };
    JsonDocument::AllocatorType& allocator = request.GetAllocator();
    request.AddMember("$schema", "http://stats.xboxlive.com/2017-1/schema#", allocator);
    request.AddMember("previousRevision", 0, allocator);
    request.AddMember("revision", GetRevisionFromClock(), allocator);
    request.AddMember("timestamp", JsonValue(xbox::services::datetime::utc_now().to_string(xbox::services::datetime::date_format::ISO_8601).c_str(), allocator).Move(), allocator);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("statswrite", m_statswritePath),
        xbox_live_api::post_stats_value_document
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(4));
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(request));

    return httpCall->Perform({ async.Queue().GetHandle(),
        [
            async
        ]
        (HttpResult result)
        {
           HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
           return async.Complete(hr);
        }
    });
}

HRESULT TitleManagedStatisticsService::UpdateTitleManagedStatistics(
    _In_ const Vector<TitleManagedStatistic>& statistics,
    _In_ AsyncContext<HRESULT> async
) const noexcept
{
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "PATCH",
        XblHttpCall::BuildUrl("statswrite", m_statswritePath),
        xbox_live_api::patch_stats_value_document
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(4));
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(SerializeStatistics(statistics)));

    return httpCall->Perform({ async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
        (HttpResult result)
        {
            HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
            return async.Complete(hr);
        }
    });
}

uint64_t TitleManagedStatisticsService::GetRevisionFromClock() noexcept
{
    uint64_t dateTime = xbox::services::datetime::utc_now().to_interval(); // eg. 131472330440000000
    const uint64_t dateTimeFromJan1st2015 = 130645440000000000;
    if (dateTime < dateTimeFromJan1st2015)
    {
        return 1; // Clock is wrong and is not yet sync'd with internet time, so just setting the revision to 1
    }
    else
    {
        uint64_t dateTimeSince2015 = dateTime - dateTimeFromJan1st2015; // eg. 826888900000000
        uint64_t dateTimeTrimmed = dateTimeSince2015 >> 16; // divide by 2^16 to get it to sub second range.  eg. 12617323303
        return dateTimeTrimmed;
    }
}


JsonDocument TitleManagedStatisticsService::SerializeStatistics(
    const Vector<TitleManagedStatistic>& stats
) noexcept
{
    assert(!stats.empty());

    JsonDocument jsonDoc{ rapidjson::kObjectType };
    auto& a{ jsonDoc.GetAllocator() };

    JsonValue statsJson{ rapidjson::kObjectType };
    JsonValue titleJson{ rapidjson::kObjectType };

    for (auto& stat : stats)
    {
        JsonValue statJson{ rapidjson::kObjectType };
        switch (stat.type)
        {
        case TitleManagedStatistic::Type::Number:
        {
            statJson.AddMember("value", stat.numberValue, a);
            break;
        }
        case TitleManagedStatistic::Type::String:
        {
            statJson.AddMember("value", JsonValue{ stat.stringValue.data(), a }.Move(), a);
            break;
        }
        case TitleManagedStatistic::Type::Null:
        {
            statJson.SetNull();
            break;
        }
        default:
        {
            assert(false);
            break;
        }
        }
        titleJson.AddMember(JsonValue{ stat.name.data(), a }.Move(), statJson.Move(), a);
    }
    statsJson.AddMember("title", titleJson.Move(), a);
    jsonDoc.AddMember("stats", statsJson.Move(), a);

    return jsonDoc;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
