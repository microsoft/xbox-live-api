// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "matchmaking_internal.h"
#include "xbox_live_app_config_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

MatchmakingService::MatchmakingService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) :
    m_user{ std::move(user) },
    m_contextSettings(contextSettings)
{
}

/*static*/ XblTicketStatus MatchTicketDetailsResponse::ConvertStringToTicketStatus(_In_ const xsapi_internal_string& value)
{
    XblTicketStatus ticketStatus = XblTicketStatus::Unknown;
    if (!value.empty())
    {
        if (utils::str_icmp_internal(value, "expired") == 0)
        {
            ticketStatus = XblTicketStatus::Expired;
        }
        else if (utils::str_icmp_internal(value, "searching") == 0)
        {
            ticketStatus = XblTicketStatus::Searching;
        }
        else if (utils::str_icmp_internal(value, "found") == 0)
        {
            ticketStatus = XblTicketStatus::Found;
        }
        else if (utils::str_icmp_internal(value, "canceled") == 0)
        {
            ticketStatus = XblTicketStatus::Canceled;
        }
    }
    return ticketStatus;
}

/*static*/ XblPreserveSessionMode MatchTicketDetailsResponse::ConvertStringToPreserveSessionMode(_In_ const xsapi_internal_string& value)
{
    XblPreserveSessionMode preserveSessionMode = XblPreserveSessionMode::Unknown;
    if (!value.empty())
    {
        if (utils::str_icmp_internal(value, "always") == 0)
        {
            preserveSessionMode = XblPreserveSessionMode::Always;
        }
        else if (utils::str_icmp_internal(value, "never") == 0)
        {
            preserveSessionMode = XblPreserveSessionMode::Never;
        }
    }
    return preserveSessionMode;
}


xsapi_internal_string MatchmakingService::GetMatchmakingSubPath(
    _In_ const xsapi_internal_string& serviceConfigId,
    _In_ const xsapi_internal_string& hopperName,
    _In_ const xsapi_internal_string& ticketId
)
{
    xsapi_internal_stringstream ss;
    ss << "/serviceconfigs/";
    ss << serviceConfigId;
    ss << "/hoppers/";
    ss << hopperName;

    if (!ticketId.empty())
    {
        ss << "/tickets/";
        ss << ticketId;
    }

    return ss.str();
}

xsapi_internal_string MatchmakingService::GetHopperSubPath(
    _In_ const xsapi_internal_string& serviceConfigId,
    _In_ const xsapi_internal_string& hopperName
) 
{
    xsapi_internal_stringstream ss;
    ss << "/serviceconfigs/";
    ss << serviceConfigId;
    ss << "/hoppers/";
    ss << hopperName;
    ss << "/stats";
    
    return ss.str();
}

xsapi_internal_string MatchmakingService::ConvertPreserveSessionModeToString(
    _In_ XblPreserveSessionMode preserve_session
) 
{
    xsapi_internal_string retval("unknown");
    if (preserve_session == XblPreserveSessionMode::Always)
    {
        retval = "always";
    }
    else if (preserve_session == XblPreserveSessionMode::Never)
    {
        retval = "never";
    }
    
    return retval;
}

/*static*/ HRESULT DeserializeHopperStatisticsResponseResult(
    _In_ const JsonValue& json,
    _Out_ HopperStatisticsResponse& result
)
{
    HRESULT errc = S_OK;
    if (json.IsNull())
    {
        result = HopperStatisticsResponse();
    }
    else
    {
        xsapi_internal_string name;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", name, true));
        int waitTime = 0;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "waitTime", waitTime));
        int population = 0;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "population", population, false));
        result = HopperStatisticsResponse(
            name,
            std::chrono::seconds(waitTime),
            population
        );
    }

    if (errc)
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return S_OK;
}

/*static*/ HRESULT DeserializeCreateMatchTicketResponseResult(
    _In_ const JsonValue& json,
    _Out_ XblCreateMatchTicketResponse& result
)
{
    HRESULT errc = S_OK;
    if (json.IsNull())
    {
        result = XblCreateMatchTicketResponse();
        memset(&result, 0, sizeof(XblCreateMatchTicketResponse));
    }
    else
    {
        xsapi_internal_string ticketId;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "ticketId", ticketId, true));
        memset(&result, 0, sizeof(XblCreateMatchTicketResponse));
        utils::strcpy(result.matchTicketId, ticketId.size() + 1, ticketId.c_str());
        int waitTime = 0;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "waitTime", waitTime));
        result.estimatedWaitTime = waitTime;
    }

    if (errc)
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return S_OK;
}

/*static*/ HRESULT DeserializeMatchTicketDetailsResponse(
    _In_ const JsonValue& json,
    _Out_ MatchTicketDetailsResponse& result
)
{
    HRESULT errc = S_OK;
    if (json.IsNull() || !json.IsObject())
    {
        result = MatchTicketDetailsResponse();
    }
    else
    {
        XblMultiplayerSessionReference ticketSession{};
        XblMultiplayerSessionReference targetSession{};
        if (json.HasMember("ticketSessionRef"))
        {
            ticketSession = xbox::services::multiplayer::Serializers::DeserializeSessionReference(json["ticketSessionRef"]).Payload();
        }
        if (json.HasMember("targetSessionRef"))
        {
            targetSession = xbox::services::multiplayer::Serializers::DeserializeSessionReference(json["targetSessionRef"]).Payload();
        }

        xsapi_internal_string ticketStatus, preserveSession;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "ticketStatus", ticketStatus, true));
        int waitTime = 0;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "waitTime", waitTime));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "preserveSession", preserveSession, false));

        if (json.HasMember("ticketAttributes"))
        {
            result = MatchTicketDetailsResponse(
                MatchTicketDetailsResponse::ConvertStringToTicketStatus(ticketStatus),
                std::chrono::seconds(waitTime),
                MatchTicketDetailsResponse::ConvertStringToPreserveSessionMode(preserveSession),
                ticketSession, 
                targetSession,
                json["ticketAttributes"]
            );
        }
        else
        {
            result = MatchTicketDetailsResponse(
                MatchTicketDetailsResponse::ConvertStringToTicketStatus(ticketStatus),
                std::chrono::seconds(waitTime),
                MatchTicketDetailsResponse::ConvertStringToPreserveSessionMode(preserveSession),
                ticketSession,
                targetSession,
                JsonValue()
            );
        }


    }

    if (errc)
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return S_OK;
}

HRESULT MatchmakingService::CreateMatchTicket(
    _In_ const XblMultiplayerSessionReference& ticketSessionReference,
    _In_ const xsapi_internal_string& matchmakingServiceConfigurationId,
    _In_ const xsapi_internal_string& hopperName,
    _In_ const std::chrono::seconds& ticketTimeout,
    _In_ XblPreserveSessionMode preserveSession,
    _In_ JsonValue& ticketAttributesJson,
    _In_ XAsyncBlock* async
) 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    XblCreateMatchTicketResponse result;
    xsapi_internal_string subPath = GetMatchmakingSubPath(
        matchmakingServiceConfigurationId,
        hopperName,
        xsapi_internal_string()
    );

    JsonDocument request(rapidjson::kObjectType);
    JsonDocument::AllocatorType& allocator = request.GetAllocator();
    request.AddMember("giveUpDuration", JsonValue(static_cast<int32_t>(ticketTimeout.count())).Move(), allocator);
    request.AddMember("preserveSession", JsonValue(ConvertPreserveSessionModeToString(preserveSession).c_str(), allocator).Move(), allocator);
    XblMultiplayerSessionReference _ticketSessionReference = XblMultiplayerSessionReferenceCreate(
            ticketSessionReference.Scid, 
            ticketSessionReference.SessionTemplateName, 
            ticketSessionReference.SessionName);
    JsonValue ticketSessonRefJsonObj(rapidjson::kObjectType);
    multiplayer::Serializers::SerializeSessionReference(_ticketSessionReference, ticketSessonRefJsonObj, allocator);
    request.AddMember("ticketSessionRef", ticketSessonRefJsonObj, allocator);
    if (!ticketAttributesJson.IsNull())
    {
       request.AddMember("ticketAttributes", ticketAttributesJson, allocator);
    }

    xsapi_internal_string requestString = JsonUtils::SerializeJson(request);

    return RunAsync(async, __FUNCTION__,
        [
            result, 
            sharedThis{ shared_from_this() }, 
            subPath, 
            requestString
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_contextSettings,
                "POST",
                XblHttpCall::BuildUrl("smartmatch", subPath),
                xbox_live_api::create_match_ticket
            );

            RETURN_HR_IF_FAILED(hr);
            RETURN_HR_IF_FAILED(httpCall->SetRetryAllowed(false));
            RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(103));
            RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestString));
           
            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [&result, data](HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    if (SUCCEEDED(hr))
                    {
                        hr = httpResult.Payload()->Result();
                        if (SUCCEEDED(hr))
                        {
                            hr = DeserializeCreateMatchTicketResponseResult(httpResult.Payload()->GetResponseBodyJson(), result);
                        }
                    }
                    XAsyncComplete(data->async, hr, sizeof(XblCreateMatchTicketResponse));
                }});

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            auto resultPtr = static_cast<XblCreateMatchTicketResponse*>(data->buffer);
            *resultPtr = result;
        }

        return S_OK;
    });
}

HRESULT MatchmakingService::DeleteMatchTicketAsync(
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& hopperName,
    _In_ const xsapi_internal_string& ticketId,
    _In_ XAsyncBlock* async
)
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    xsapi_internal_string subPath = GetMatchmakingSubPath(
        serviceConfigurationId,
        hopperName,
        ticketId
    );

    return RunAsync(async, __FUNCTION__,
        [
            sharedThis{ shared_from_this() }, 
            subPath
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_contextSettings,
                "DELETE",
                XblHttpCall::BuildUrl("smartmatch", subPath),
                xbox_live_api::delete_match_ticket
            );

            RETURN_HR_IF_FAILED(hr);
            RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(103));

            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [data](HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    XAsyncComplete(data->async, hr, 0);
                }});

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }

        return S_OK;
    });
}

HRESULT MatchmakingService::GetMatchTicketDetailsAsync(
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& hopperName,
    _In_ const xsapi_internal_string& ticketId,
    _In_ XAsyncBlock* async
) 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    xsapi_internal_string subPath = GetMatchmakingSubPath(
        serviceConfigurationId,
        hopperName,
        ticketId
    );

    MatchTicketDetailsResponse result;

    return RunAsync(async, __FUNCTION__,
        [
            result, 
            subPath, 
            sharedThis{ shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_contextSettings,
                "GET",
                XblHttpCall::BuildUrl("smartmatch", subPath),
                xbox_live_api::get_match_ticket_details
            );

            RETURN_HR_IF_FAILED(hr);
            RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(103));

            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [data, &result](HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    if (SUCCEEDED(hr))
                    {
                        hr = httpResult.Payload()->Result();
                        if (SUCCEEDED(hr))
                        {
                            hr = DeserializeMatchTicketDetailsResponse(httpResult.Payload()->GetResponseBodyJson(), result);
                        }
                    }

                    size_t bufferSize = 0;
                    if (!result.TicketAttributes().IsNull()) {
                        bufferSize += JsonUtils::SerializeJson(result.TicketAttributes()).length() + 1;
                    }
        
                    bufferSize += sizeof(XblMatchTicketDetailsResponse);
                    XAsyncComplete(data->async, hr, bufferSize);
                }});

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            XblMatchTicketDetailsResponse* matchTicketResult = reinterpret_cast<XblMatchTicketDetailsResponse*>(buffer);
            matchTicketResult->preserveSession = result.PreserveSession();
            matchTicketResult->estimatedWaitTime = result.EstimatedWaitTime().count();
            matchTicketResult->matchStatus = result.MatchStatus();
            utils::strcpy(matchTicketResult->ticketSession.Scid, sizeof(result.TicketSession().Scid), result.TicketSession().Scid);
            utils::strcpy(matchTicketResult->ticketSession.SessionName, sizeof(result.TicketSession().SessionName), result.TicketSession().SessionName);
            utils::strcpy(matchTicketResult->ticketSession.SessionTemplateName, sizeof(result.TicketSession().SessionTemplateName), result.TicketSession().SessionTemplateName);
            utils::strcpy(matchTicketResult->targetSession.Scid, sizeof(result.TargetSession().Scid), result.TargetSession().Scid);
            utils::strcpy(matchTicketResult->targetSession.SessionName, sizeof(result.TargetSession().SessionName), result.TargetSession().SessionName);
            utils::strcpy(matchTicketResult->targetSession.SessionTemplateName, sizeof(result.TargetSession().SessionTemplateName), result.TargetSession().SessionTemplateName);
            if (!result.TicketAttributes().IsNull()) {
                buffer += sizeof(XblMatchTicketDetailsResponse);
                matchTicketResult->ticketAttributes = static_cast<char*>(buffer);
            	utils::strcpy(buffer, JsonUtils::SerializeJson(result.TicketAttributes()).size() + 1, JsonUtils::SerializeJson(result.TicketAttributes()).c_str());
            }
            else 
            {
                matchTicketResult->ticketAttributes = nullptr;
            }
        }

        return S_OK;
    });
}

HRESULT MatchmakingService::GetHopperStatistics(
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& hopperName,
    _In_ XAsyncBlock* async
)
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    xsapi_internal_string subPath = GetHopperSubPath(
        serviceConfigurationId,
        hopperName
    );
    HopperStatisticsResponse result;

    return RunAsync(async, __FUNCTION__,
        [
            result, 
            subPath, 
            sharedThis{ shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_contextSettings,
                "GET",
                XblHttpCall::BuildUrl("smartmatch", subPath),
                xbox_live_api::get_hopper_statistics
            );

            RETURN_HR_IF_FAILED(hr);
            RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(103));

            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [data, &result](HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    if (SUCCEEDED(hr))
                    {
                        hr = httpResult.Payload()->Result();
                        if (SUCCEEDED(hr))
                        {
                            hr = DeserializeHopperStatisticsResponseResult(httpResult.Payload()->GetResponseBodyJson(), result);
                        }
                    }
                    XAsyncComplete(data->async, hr, sizeof(XblHopperStatisticsResponse) + result.HopperName().size() + 1);
                }
            });

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            auto resultPtr = reinterpret_cast<XblHopperStatisticsResponse*>(buffer);
            resultPtr->estimatedWaitTime = result.EstimatedWaitTime().count();
            resultPtr->playersWaitingToMatch = result.PlayersWaitingToMatch();
            auto hopperName = result.HopperName();
            auto stringPtr = buffer + sizeof(XblHopperStatisticsResponse);
            utils::strcpy(stringPtr, hopperName.size() + 1, hopperName.c_str());
            resultPtr->hopperName = stringPtr;
        }

        return S_OK;
    });
}

#if !defined(XBOX_LIVE_CREATORS_SDK)
#if !XSAPI_NO_PPL
namespace legacy
{
    match_ticket_details_response::match_ticket_details_response() :
        m_buffer(),
        m_matchTicketDetailsResponse()
    {
    }

    match_ticket_details_response::match_ticket_details_response(std::shared_ptr<char> buffer) :
        m_buffer(buffer),
        m_matchTicketDetailsResponse(*reinterpret_cast<XblMatchTicketDetailsResponse*>(m_buffer.get()))
    {
    }

    ticket_status
        match_ticket_details_response::match_status() const
    {
        return static_cast<ticket_status>(m_matchTicketDetailsResponse.matchStatus);
    }

    std::chrono::seconds
        match_ticket_details_response::estimated_wait_time() const
    {
        return std::chrono::seconds(m_matchTicketDetailsResponse.estimatedWaitTime);
    }

    preserve_session_mode
        match_ticket_details_response::preserve_session() const
    {
        return static_cast<preserve_session_mode>(m_matchTicketDetailsResponse.preserveSession);
    }

    xbox::services::multiplayer::legacy::multiplayer_session_reference
        match_ticket_details_response::ticket_session() const
    {
        return xbox::services::multiplayer::legacy::multiplayer_session_reference(
            StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.Scid),
            StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.SessionTemplateName),
            StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.SessionName));
    }

    xbox::services::multiplayer::legacy::multiplayer_session_reference
        match_ticket_details_response::target_session() const
    {
        return xbox::services::multiplayer::legacy::multiplayer_session_reference(
            StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.Scid),
            StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.SessionTemplateName),
            StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.SessionName));
    }

    web::json::value
        match_ticket_details_response::ticket_attributes() const
    {
        return web::json::value(StringTFromUtf8(m_matchTicketDetailsResponse.ticketAttributes));
    }

    ticket_status match_ticket_details_response::convert_string_to_ticket_status(
        _In_ const string_t& value
    )
    {
        ticket_status ticketStatus = ticket_status::unknown;
        if (!value.empty())
        {
            if (Stricmp(value, _T("expired")) == 0)
            {
                ticketStatus = ticket_status::expired;
            }
            else if (Stricmp(value, _T("searching")) == 0)
            {
                ticketStatus = ticket_status::searching;
            }
            else if (Stricmp(value, _T("found")) == 0)
            {
                ticketStatus = ticket_status::found;
            }
            else if (Stricmp(value, _T("canceled")) == 0)
            {
                ticketStatus = ticket_status::canceled;
            }
        }

        return ticketStatus;
    }

    preserve_session_mode match_ticket_details_response::convert_string_to_preserve_session_mode(
        _In_ const string_t& value
    )
    {
        preserve_session_mode preserve_session_mode = preserve_session_mode::unknown;
        if (!value.empty())
        {
            if (Stricmp(value, _T("always")) == 0)
            {
                preserve_session_mode = preserve_session_mode::always;
            }
            else if (Stricmp(value, _T("never")) == 0)
            {
                preserve_session_mode = preserve_session_mode::never;
            }
        }
        return preserve_session_mode;
    }


    hopper_statistics_response::hopper_statistics_response() :
        m_buffer(),
        m_hopperStatisticsResponse()
    {
    }

    hopper_statistics_response::hopper_statistics_response(std::shared_ptr<char> buffer) :
        m_buffer(buffer),
        m_hopperStatisticsResponse(*reinterpret_cast<XblHopperStatisticsResponse*>(m_buffer.get()))
    {
    }

    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    string_t
        hopper_statistics_response::hopper_name() const
    {
        return StringTFromUtf8(m_hopperStatisticsResponse.hopperName);
    }

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    std::chrono::seconds hopper_statistics_response::estimated_wait_time() const
    {
        return std::chrono::seconds(m_hopperStatisticsResponse.estimatedWaitTime);
    }

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    uint32_t hopper_statistics_response::players_waiting_to_match() const
    {
        return m_hopperStatisticsResponse.playersWaitingToMatch;
    }

    create_match_ticket_response::create_match_ticket_response() :
        m_createMatchTicketResponse{  }
    {
    }

    create_match_ticket_response::create_match_ticket_response(
        XblCreateMatchTicketResponse response
    ) :
        m_createMatchTicketResponse(response)
    {
    }

    string_t
        create_match_ticket_response::match_ticket_id() const
    {
        return StringTFromUtf8(m_createMatchTicketResponse.matchTicketId);
    }

    std::chrono::seconds
        create_match_ticket_response::estimated_wait_time() const
    {
        return std::chrono::seconds(m_createMatchTicketResponse.estimatedWaitTime);
    }

    matchmaking_service::matchmaking_service(_In_ XblContextHandle contextHandle)
    {
        XblContextDuplicateHandle(contextHandle, &m_xblContext);
    }

    matchmaking_service::matchmaking_service(const matchmaking_service& other)
    {
        XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
    }

    matchmaking_service& matchmaking_service::operator=(matchmaking_service other)
    {
        std::swap(m_xblContext, other.m_xblContext);
        return *this;
    }

    matchmaking_service::~matchmaking_service()
    {
        XblContextCloseHandle(m_xblContext);
    }

    pplx::task<xbl_result<create_match_ticket_response>> matchmaking_service::create_match_ticket(
        _In_ const xbox::services::multiplayer::legacy::multiplayer_session_reference& ticketSessionReference,
        _In_ const string_t& matchmakingServiceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const std::chrono::seconds& ticketTimeout,
        _In_ preserve_session_mode preserveSession,
        _In_ const web::json::value& ticketAttributesJson
    )
    {
        auto xblContext = m_xblContext;

        auto asyncWrapper = new xbox::services::legacy::AsyncWrapper<create_match_ticket_response>(
            [](XAsyncBlock* async, create_match_ticket_response& result)
            {
                XblCreateMatchTicketResponse resultResponse;
                auto hr = XblMatchmakingCreateMatchTicketResult(async, &resultResponse);
                if (SUCCEEDED(hr))
                {
                    result = create_match_ticket_response(resultResponse);
                }
                return hr;
            });

        XblMultiplayerSessionReference _ticketSessionReference;
        Utf8FromCharT(ticketSessionReference.service_configuration_id().data(), _ticketSessionReference.Scid, sizeof(_ticketSessionReference.Scid));
        Utf8FromCharT(ticketSessionReference.session_template_name().data(), _ticketSessionReference.SessionTemplateName, sizeof(_ticketSessionReference.SessionTemplateName));
        Utf8FromCharT(ticketSessionReference.session_name().data(), _ticketSessionReference.SessionName, sizeof(_ticketSessionReference.SessionName));

        auto hr = XblMatchmakingCreateMatchTicketAsync(
            xblContext,
            _ticketSessionReference,
            StringFromStringT(matchmakingServiceConfigurationId).c_str(),
            StringFromStringT(hopperName).c_str(),
            ticketTimeout.count(),
            static_cast<XblPreserveSessionMode>(preserveSession),
            StringFromStringT(ticketAttributesJson.serialize()).c_str(),
            &asyncWrapper->async
        );

        return asyncWrapper->Task(hr);
    }

    pplx::task<xbl_result<void>> matchmaking_service::delete_match_ticket(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const string_t& ticketId
    )
    {
        auto xblContext = m_xblContext;

        auto asyncWrapper = new xbox::services::legacy::AsyncWrapper<void>();
        auto hr = XblMatchmakingDeleteMatchTicketAsync(
            xblContext,
            StringFromStringT(serviceConfigurationId).c_str(),
            StringFromStringT(hopperName).c_str(),
            StringFromStringT(ticketId).c_str(),
            &asyncWrapper->async);

        return asyncWrapper->Task(hr);
    }

    pplx::task<xbl_result<match_ticket_details_response>> matchmaking_service::get_match_ticket_details(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const string_t& ticketId
    )
    {
        auto xblContext = m_xblContext;

        auto asyncWrapper = new xbox::services::legacy::AsyncWrapper<match_ticket_details_response>(
            [](XAsyncBlock* async, match_ticket_details_response& result)
            {
                size_t bufferSize;
                auto hr = XblMatchmakingGetMatchTicketDetailsResultSize(async, &bufferSize);
                if (SUCCEEDED(hr))
                {
                    std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
                    XblMatchTicketDetailsResponse* resultPtr;
                    hr = XblMatchmakingGetMatchTicketDetailsResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

                    if (SUCCEEDED(hr))
                    {
                        result = match_ticket_details_response(buffer);
                    }
                }
                return hr;
            });

        auto hr = XblMatchmakingGetMatchTicketDetailsAsync(
            xblContext,
            StringFromStringT(serviceConfigurationId).c_str(),
            StringFromStringT(hopperName).c_str(),
            StringFromStringT(ticketId).c_str(),
            &asyncWrapper->async);

        return asyncWrapper->Task(hr);
    }

    pplx::task<xbl_result<hopper_statistics_response>>  matchmaking_service::get_hopper_statistics(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName
    )
    {
        auto xblContext = m_xblContext;

        auto asyncWrapper = new xbox::services::legacy::AsyncWrapper<hopper_statistics_response>(
            [](XAsyncBlock* async, hopper_statistics_response& result)
            {
                size_t bufferSize;
                auto hr = XblMatchmakingGetHopperStatisticsResultSize(async, &bufferSize);
                if (SUCCEEDED(hr))
                {
                    std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
                    XblHopperStatisticsResponse* resultPtr;
                    hr = XblMatchmakingGetHopperStatisticsResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

                    if (SUCCEEDED(hr))
                    {
                        result = hopper_statistics_response(buffer);
                    }
                }
                return hr;
            });

        auto hr = XblMatchmakingGetHopperStatisticsAsync(
            xblContext,
            StringFromStringT(serviceConfigurationId).c_str(),
            StringFromStringT(hopperName).c_str(),
            &asyncWrapper->async);

        return asyncWrapper->Task(hr);
    }
}
#endif // !XSAPI_NO_PPL
#endif // !defined(XBOX_LIVE_CREATORS_SDK)

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END


/// <summary>
/// Sends a matchmaking request to the server and returns the match ticket with a ticket id.
/// Call XblMatchmakingCreateMatchTicketResultSize and XblMatchmakingCreateMatchTicketResult upon completion to get the result
/// </summary>
/// <param name="XboxLiveContext">Xbox live context for the local user.</param>
/// <param name="ticketSessionReference">The multiplayer session to use for the match.</param>
/// <param name="matchmakingServiceConfigurationId">The service configuration ID for the match.</param>
/// <param name="hopperName">The name of the hopper.</param>
/// <param name="ticketTimeout">The maximum time to wait for players to join the session.</param>
/// <param name="preserveSession">Indicates if the session should be preserved.</param>
/// <param name="ticketAttributesJson">The ticket attributes for the session. (Optional)</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>The async object for notifying when the operation is completed. With the handler, a new match ticket
/// object is returned. The match ticket object contains server returned information such as ticket id and wait
/// time, and also contains copies of the title specified data from the ticket data object.</returns>
/// <remarks>Calls V103 POST /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}</remarks>
STDAPI XblMatchmakingCreateMatchTicketAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblMultiplayerSessionReference ticketSessionReference,
    _In_ const char* matchmakingServiceConfigurationId,
    _In_ const char* hopperName,
    _In_ const uint64_t ticketTimeout,
    _In_ XblPreserveSessionMode preserveSession,
    _In_ const char* ticketAttributesJson,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(matchmakingServiceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(hopperName);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ticketAttributesJson);
    VERIFY_XBL_INITIALIZED();

    return xboxLiveContext->MatchmakingService()->CreateMatchTicket(
        ticketSessionReference,
        matchmakingServiceConfigurationId,
        hopperName,
        std::chrono::seconds(ticketTimeout),
        preserveSession,
        JsonDocument().Parse(ticketAttributesJson),
        asyncBlock);
}

/// <summary>
/// Get the result for an XblMatchmakingCreateMatchTicketAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.</param>
/// <param name="buffer">Byte buffer to write result into.</param>
/// <param name="resultPtr">Pointer to result.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
STDAPI XblMatchmakingCreateMatchTicketResult(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ XblCreateMatchTicketResponse* resultPtr
) XBL_NOEXCEPT 
{
    auto hr = XAsyncGetResult(asyncBlock, nullptr, sizeof(XblCreateMatchTicketResponse), resultPtr, nullptr);
    return hr;
}

/// <summary>
/// Deletes a the match ticket on the server.
/// </summary>
/// <param name="XboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
/// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
/// <param name="ticketId">The id of the ticket to delete on the server.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>The async object for notifying when the operation has been completed.</returns>
/// <remarks>Calls V103 DELETE /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
STDAPI XblMatchmakingDeleteMatchTicketAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfiguration,
    _In_ const char* hopperName,
    _In_ const char* ticketId,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfiguration);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(hopperName);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ticketId);
    VERIFY_XBL_INITIALIZED();

    return xboxLiveContext->MatchmakingService()->DeleteMatchTicketAsync(
        serviceConfiguration,
        hopperName,
        ticketId,
        asyncBlock);
}

/// <summary>
/// Retrieves the properties of a match ticket from the server.
/// </summary>
/// <param name="XboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
/// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
/// <param name="ticketId">The ticket id of the match ticket to retrieve.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>The async object for notifying when the operation is completed. With the handler, the match
/// ticket object with the data for the ticket, including ticket id and wait time information, is returned
/// returned from the server.</returns>
/// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
STDAPI XblMatchmakingGetMatchTicketDetailsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfiguration,
    _In_ const char* hopperName,
    _In_ const char* ticketId,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfiguration);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(hopperName);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ticketId);
    VERIFY_XBL_INITIALIZED();

    return xboxLiveContext->MatchmakingService()->GetMatchTicketDetailsAsync(
        serviceConfiguration,
        hopperName,
        ticketId,
        asyncBlock);

}

/// <summary>
/// Get the result size for an XblMatchmakingGetMatchTicketDetailsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
STDAPI XblMatchmakingGetMatchTicketDetailsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT 
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}

/// <summary>
/// Get the result for an XblMatchmakingGetMatchTicketDetailsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.</param>
/// <param name="buffer">Byte buffer to write result into.</param>
/// <param name="result">Pointer to result.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
STDAPI XblMatchmakingGetMatchTicketDetailsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMatchTicketDetailsResponse** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblMatchTicketDetailsResponse*>(buffer);
    }
    return hr;
}

/// <summary>
/// Gets statistics about a hopper such as how many players are in it.
/// </summary>
/// <param name="XboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
/// <param name="hopperName">The name of the hopper to query stats for.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>The async object for notifying when the operation is completed. With the handler, an object
/// containing statistics about the hopper is returned.</returns>
/// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/stats</remarks>
STDAPI XblMatchmakingGetHopperStatisticsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfiguration,
    _In_ const char* hopperName,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT 
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfiguration);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(hopperName);
    VERIFY_XBL_INITIALIZED();

    return xboxLiveContext->MatchmakingService()->GetHopperStatistics(
        serviceConfiguration,
        hopperName,
        asyncBlock
    );
}

/// <summary>
/// Get the result size for an XblMatchmakingGetHopperStatisticsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
STDAPI XblMatchmakingGetHopperStatisticsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT 
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}

/// <summary>
/// Get the result for an XblMatchmakingGetHopperStatisticsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.</param>
/// <param name="buffer">Byte buffer to write result into.</param>
/// <param name="result">Pointer to result.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
STDAPI XblMatchmakingGetHopperStatisticsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblHopperStatisticsResponse** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblHopperStatisticsResponse*>(buffer);
    }
    return hr;
}
