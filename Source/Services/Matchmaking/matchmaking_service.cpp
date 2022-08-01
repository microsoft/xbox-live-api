// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "matchmaking_internal.h"
#include "xbox_live_app_config_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;

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

    if (FAILED(errc))
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

    if (FAILED(errc))
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

    if (FAILED(errc))
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
try
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
CATCH_RETURN()

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
try
{
    auto hr = XAsyncGetResult(asyncBlock, nullptr, sizeof(XblCreateMatchTicketResponse), resultPtr, nullptr);
    return hr;
}
CATCH_RETURN()

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
try
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
CATCH_RETURN()

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
try
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
CATCH_RETURN()

/// <summary>
/// Get the result size for an XblMatchmakingGetMatchTicketDetailsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
STDAPI XblMatchmakingGetMatchTicketDetailsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT 
try
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}
CATCH_RETURN()

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
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblMatchTicketDetailsResponse*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

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
try
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
CATCH_RETURN()

/// <summary>
/// Get the result size for an XblMatchmakingGetHopperStatisticsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
STDAPI XblMatchmakingGetHopperStatisticsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT 
try
{
    return XAsyncGetResultSize(asyncBlock, resultSizeInBytes);
}
CATCH_RETURN()

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
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(asyncBlock, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblHopperStatisticsResponse*>(buffer);
    }
    return hr;
}
CATCH_RETURN()
