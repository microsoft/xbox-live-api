// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"
#include "xbox_live_context_internal.h"
#include "real_time_activity_manager.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::system;
using namespace xbox::services::legacy;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

#define GET_ACTIIVITIES_SUBPATH "/handles/query?include=relatedInfo,customProperties"
#define GET_SEARCH_HANDLES_SUBPATH "/handles/query?include=relatedInfo,roleInfo,customProperties"
#define MULTIPLAYER_SERVICE_CONTRACT_VERSION 107

MultiplayerService::MultiplayerService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::AppConfig> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> realTimeActivity
) noexcept
    : m_user{ std::move(user) },
    m_xboxLiveContextSettings{ std::move(xboxLiveContextSettings) },
    m_appConfig{ std::move(appConfig) },
    m_rtaManager{ std::move(realTimeActivity) }
{
}

MultiplayerService::~MultiplayerService() noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    UnsubscribeFromRta();
}

HRESULT MultiplayerService::WriteSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ XblMultiplayerSessionWriteMode mode,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
) noexcept
{
    auto& sessionReference = session->SessionReference();

    auto pathAndQuery = MultiplayerSessionDirectoryCreateOrUpdateSubpath(
        sessionReference.Scid,
        sessionReference.SessionTemplateName,
        sessionReference.SessionName
    );

    return WriteSessionUsingSubpath(
        session,
        mode,
        pathAndQuery,
        std::move(async)
    );
}

HRESULT MultiplayerService::WriteSessionByHandle(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ XblMultiplayerSessionWriteMode mode,
    _In_ const String& handleId,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(handleId.empty());

    auto pathAndQuery = MultiplayerSessionDirectoryCreateOrUpdateByHandleSubpath(handleId);
    return WriteSessionUsingSubpath(
        session,
        mode,
        pathAndQuery,
        std::move(async)
    );
}

HRESULT MultiplayerService::GetCurrentSession(
    _In_ XblMultiplayerSessionReference sessionReference,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(sessionReference.Scid);

    String pathAndQuery = MultiplayerSessionDirectoryCreateOrUpdateSubpath(
        sessionReference.Scid,
        sessionReference.SessionTemplateName,
        sessionReference.SessionName
    );

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("sessiondirectory", pathAndQuery),
        xbox_live_api::get_current_session
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [
            sessionReference,
            xuid{ m_user.Xuid() },
            async
        ]
    (HttpResult httpResult)
    {
        HRESULT hr = httpResult.Hresult();
        if (FAILED(hr))
        {
            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(hr, "Http call failed"));
        }

        hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(hr, errorMessagePtr));
        }
        else if (httpResult.Payload()->HttpStatus() == 204)
        {
            // Return a not found error when trying to get an non-existing session
            return async.Complete(__HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND));
        }

        auto session = MakeShared<XblMultiplayerSession>(
            xuid,
            sessionReference,
            httpResult.Payload()->GetResponseHeader(ETAG_HEADER),
            httpResult.Payload()->GetResponseHeader(DATE_HEADER),
            httpResult.Payload()->GetResponseBodyJson()
        );

        async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(session, session->DeserializationError(), "Deserialize error"));
    }
    });
}

HRESULT MultiplayerService::GetCurrentSessionByHandle(
    _In_ const String& handleId,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(handleId.empty());
    String pathAndQuery = MultiplayerSessionDirectoryCreateOrUpdateByHandleSubpath(handleId);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("sessiondirectory", pathAndQuery),
        xbox_live_api::get_current_session_by_handle
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [
            xuid{ m_user.Xuid() },
            async
        ]
    (HttpResult httpResult)
    {
        HRESULT hr = httpResult.Hresult();
        if (FAILED(hr))
        {
            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(hr));
        }

        hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(hr));
        }
        else if (httpResult.Payload()->HttpStatus() == 204)
        {
            // Return a not found error when trying to get an non-existing session
            return async.Complete(__HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND));
        }

        auto contentLocation = httpResult.Payload()->GetResponseHeader("Content-Location");

        XblMultiplayerSessionReference sessionReference;
        hr = XblMultiplayerSessionReferenceParseFromUriPath(contentLocation.c_str(), &sessionReference);
        if (FAILED(hr))
        {
            // Failed to parse session reference from URI
            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(hr));
        }

        auto session = MakeShared<XblMultiplayerSession>(
            xuid,
            sessionReference,
            httpResult.Payload()->GetResponseHeader(ETAG_HEADER),
            httpResult.Payload()->GetResponseHeader(DATE_HEADER),
            httpResult.Payload()->GetResponseBodyJson()
        );

        async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(session, session->DeserializationError()));
    }
    });
}

SessionQuery::SessionQuery(const XblMultiplayerSessionQuery* other) noexcept
    : XblMultiplayerSessionQuery{ *other }
{
    // Deep copy xuid filters & keyword filter so that we own them
    for (size_t i = 0; i < XuidFiltersCount; ++i)
    {
        m_xuidFilters.push_back(other->XuidFilters[i]);
    }
    XuidFilters = m_xuidFilters.data();

    if (KeywordFilter)
    {
        m_keywordFilter = KeywordFilter;
        KeywordFilter = m_keywordFilter.data();
    }
}

SessionQuery::SessionQuery(const SessionQuery& other) noexcept
    : SessionQuery{ &other }
{
}

String SessionQuery::PathAndQuery() const noexcept
{
    Stringstream source;

    source << "/serviceconfigs/";
    source << Scid;
    if (SessionTemplateNameFilter[0] != 0)
    {
        source << "/sessiontemplates/";
        source << SessionTemplateNameFilter;
    }

    if (XuidFiltersCount > 1)
    {
        source << "/batch";
    }
    else
    {
        source << "/sessions";
    }

    Vector<String> params;
    if (XuidFiltersCount == 1)
    {
        Stringstream param;
        param << "xuid=";
        param << xbox::services::uri::encode_uri(utils::uint64_to_internal_string(XuidFilters[0]));
        params.push_back(param.str());
    }

    if (!m_keywordFilter.empty())
    {
        Stringstream param;
        param << "keyword=";
        param << xbox::services::uri::encode_uri(KeywordFilter);
        params.push_back(param.str());
    }

    if (VisibilityFilter != XblMultiplayerSessionVisibility::Any)
    {
        Stringstream param;
        param << "visibility=";
        param << xbox::services::uri::encode_uri(Serializers::StringFromMultiplayerSessionVisibility(VisibilityFilter));
        params.push_back(param.str());
    }

    if (ContractVersionFilter != 0)
    {
        Stringstream param;
        param << "version=";
        param << ContractVersionFilter;
        params.push_back(param.str());
    }

    if (IncludePrivateSessions)
    {
        params.push_back("private=true");
    }

    if (IncludeReservations)
    {
        params.push_back("reservations=true");
    }

    if (IncludeInactiveSessions)
    {
        params.push_back("inactive=true");
    }

    if (MaxItems != 0)
    {
        Stringstream param;
        param << "take=";
        param << MaxItems;
        params.push_back(param.str());
    }

    source << utils::get_query_from_params(params);

    return source.str();
}

JsonDocument SessionQuery::RequestBody() const noexcept
{
    JsonDocument requestBody{ rapidjson::kNullType };
    if (m_xuidFilters.size() > 1)
    {
        requestBody.SetObject();
        JsonValue xuidsArrayJson{ rapidjson::kArrayType };
        JsonUtils::SerializeVector(JsonUtils::JsonXuidSerializer, m_xuidFilters, xuidsArrayJson, requestBody.GetAllocator());
        requestBody.AddMember("xuids", xuidsArrayJson, requestBody.GetAllocator());
    }
    return requestBody;
}

HRESULT MultiplayerService::GetSessions(
    _In_ const SessionQuery& getSessionsRequest,
    _In_ AsyncContext<Result<Vector<XblMultiplayerSessionQueryResult>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(getSessionsRequest.Scid);
    RETURN_HR_INVALIDARGUMENT_IF((getSessionsRequest.XuidFilters == nullptr || getSessionsRequest.XuidFiltersCount == 0) && 
                                 (getSessionsRequest.KeywordFilter == nullptr || getSessionsRequest.KeywordFilter[0] == 0));
    RETURN_HR_INVALIDARGUMENT_IF(getSessionsRequest.IncludeReservations && (getSessionsRequest.XuidFilters == nullptr || getSessionsRequest.XuidFiltersCount == 0));
    RETURN_HR_INVALIDARGUMENT_IF(getSessionsRequest.IncludeInactiveSessions && (getSessionsRequest.XuidFilters == nullptr || getSessionsRequest.XuidFiltersCount == 0));

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        getSessionsRequest.XuidFiltersCount > 1 ? "POST" : "GET",
        XblHttpCall::BuildUrl("sessiondirectory", getSessionsRequest.PathAndQuery()),
        xbox_live_api::get_sessions
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

    auto requestBody{ getSessionsRequest.RequestBody() };
    if (!requestBody.IsNull())
    {
        RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));
    }

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
    (HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            return async.Complete({ httpResult.Hresult(), "Http call failed" });
        }

        auto hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        Vector<XblMultiplayerSessionQueryResult> sessionStates;
        hr = JsonUtils::ExtractJsonVector<XblMultiplayerSessionQueryResult>(
            Serializers::DeserializeMultiplayerSessionQueryResult,
            httpResult.Payload()->GetResponseBodyJson(),
            "results",
            sessionStates,
            true
        );
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
        
        return async.Complete(sessionStates);
    }
    });
}

HRESULT MultiplayerService::SetActivity(
    _In_ const XblMultiplayerSessionReference& sessionReference,
    _In_ AsyncContext<Result<void>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(sessionReference.Scid);

    MultiplayerActivityHandlePostRequest request{ sessionReference };

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", "/handles"),
        xbox_live_api::set_activity
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
    JsonDocument requestJson;
    request.Serialize(requestJson, requestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(requestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            async.Complete({ httpResult.Hresult(), "Http call failed" });
        }
        else
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            async.Complete({ httpResult.Payload()->Result(), errorMessagePtr });
        }
    }
    });
}

HRESULT MultiplayerService::SetTransferHandle(
    _In_ const XblMultiplayerSessionReference& targetSessionReference,
    _In_ const XblMultiplayerSessionReference& originSessionReference,
    _In_ AsyncContext<Result<String>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(targetSessionReference.Scid);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(originSessionReference.Scid);

    MultiplayerTransferHandlePostRequest request{ targetSessionReference, originSessionReference };

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", "/handles"),
        xbox_live_api::set_transfer_handle
    ));
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
    JsonDocument requestJson;
    request.Serialize(requestJson, requestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(requestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            return async.Complete({ httpResult.Hresult(), "Http call failed" });
        }

        auto hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        auto result = Serializers::DeserializeMultiplayerInvite(httpResult.Payload()->GetResponseBodyJson());
        auto multiplayerInvite = result.Payload();

        if (Failed(result))
        {
            return async.Complete(result.Hresult());
        }
        else
        {
            return async.Complete(String{ multiplayerInvite.Data });
        }
    }});
}

HRESULT MultiplayerService::CreateSearchHandle(
    _In_ MultiplayerSearchHandleRequest searchHandleRequest,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>> async
) const noexcept
{
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", "/handles"),
        xbox_live_api::set_search_handle
    ));
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
	JsonDocument searchHandleRequestJson;
    searchHandleRequest.Serialize(searchHandleRequestJson, searchHandleRequestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(searchHandleRequestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            return async.Complete({ httpResult.Hresult(), "Http call failed" });
        }

        auto hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        auto result = XblMultiplayerSearchHandleDetails::Deserialize(httpResult.Payload()->GetResponseBodyJson());
        async.Complete(result);
    }});
}

HRESULT MultiplayerService::ClearActivity(
    _In_ const String& scid,
    _In_ AsyncContext<Result<void>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(scid.empty());

    return GetActivitiesForUsers(
        scid, 
        Vector<uint64_t>{ m_user.Xuid() },
        AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ async.Queue().DeriveWorkerQueue(),
        [
            sharedThis{ shared_from_this() },
            this,
            async
        ]
    (Result<Vector<XblMultiplayerActivityDetails>> result)
    {
        if (FAILED(result.Hresult()))
        {
            return async.Complete(result.Hresult());
        }

        auto& activityDetails = result.Payload();
        size_t responseSize = activityDetails.size();

        Stringstream subPath;
        if (responseSize == 0)
        {
            // There should be at least one activity per user
            // Don't want to change behavior, but I think we should treat it as a success if there is no
            // activity to clear.
            return async.Complete(utils::convert_xbox_live_error_code_to_hresult(xbl_error_code::invalid_argument));
        }
        else if (responseSize > 1)
        {
            // There should only be one activity per user
            // Don't want to change behavior, but it seems like this is an unexpected service response, so 
            // we should return E_UNEXPECTED rather than E_INVALIDARG
            return async.Complete(utils::convert_xbox_live_error_code_to_hresult(xbl_error_code::invalid_argument));
        }
        else
        {
            subPath << "/handles/" << activityDetails.at(0).HandleId;
        }

        Result<User> userResult = m_user.Copy();
        if (FAILED(userResult.Hresult()))
        {
            return async.Complete(userResult.Hresult());
        }

        auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
        HRESULT hr = httpCall->Init(
            m_xboxLiveContextSettings,
            "DELETE",
            XblHttpCall::BuildUrl("sessiondirectory", subPath.str()),
            xbox_live_api::clear_activity
        );

        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        hr = httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION);
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        hr = httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
            [async](HttpResult httpResult)
        {
            if (Failed(httpResult))
            {
                async.Complete(httpResult.Hresult());
            }
            else
            {
                async.Complete(httpResult.Payload()->Result());
            }
        }});

        if (FAILED(hr))
        {
            async.Complete(hr);
        }
    }
    });
}

HRESULT MultiplayerService::DeleteSearchHandle(
    _In_ const String& handleId,
    _In_ AsyncContext<Result<void>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(handleId.empty());

    String handleStr = "/handles/" + handleId;

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "DELETE",
        XblHttpCall::BuildUrl("sessiondirectory", handleStr),
        xbox_live_api::delete_search_handle
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (Failed(httpResult))
        {
            async.Complete(httpResult.Hresult());
        }
        else
        {
            async.Complete(httpResult.Payload()->Result());
        }
    }});
}

HRESULT MultiplayerService::SendInvites(
    _In_ XblMultiplayerSessionReference sessionReference,
    _In_ const Vector<uint64_t>& xuids,
    _In_ uint32_t titleId,
    _In_ const String& contextStringId,
    _In_ const String& customActivationContext,
    _In_ AsyncContext<Result<Vector<String>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(!XblMultiplayerSessionReferenceIsValid(&sessionReference) || xuids.empty());

    // MPSD only allows creating a single invite handle at a time. SendInvitesOperation attempts
    // to create an invite handle for each invited user. If creation of a single handle fails, the operation
    // will continue, attempting to create the remaining handles. The result will contain all handles which
    // were successfully created.
    struct SendInvitesOperation : public std::enable_shared_from_this<SendInvitesOperation>
    {
        SendInvitesOperation(
            std::shared_ptr<const MultiplayerService> multiplayerService,
            const XblMultiplayerSessionReference& sessionReference,
            const Vector<uint64_t>& xuidsToInvite,
            uint32_t titleId,
            const String& contextString,
            const String& customActivationContext,
            AsyncContext<Result<Vector<String>>> async
        ) noexcept
            : m_multiplayerService{ std::move(multiplayerService) },
            m_requestBody{ sessionReference, 0, titleId, contextString, customActivationContext },
            m_xuidsToInvite{ xuidsToInvite.rbegin(), xuidsToInvite.rend() },
            m_async{ std::move(async) }
        {
        }

        void Run() noexcept
        {
            if (m_xuidsToInvite.empty())
            {
                m_async.Complete(m_inviteHandles);
            }
            else
            {
                auto nextXuid{ m_xuidsToInvite.back() };
                m_xuidsToInvite.pop_back();
                HRESULT hr = SendInvite(nextXuid);
                if (FAILED(hr))
                {
                    LOGS_ERROR << __FUNCTION__ << " Invite failed for user[" << nextXuid << "], hr=" << hr;
                    LOGS_ERROR << __FUNCTION__ << " Continuing with remaining invited users.";
                    this->Run();
                }
            }
        }

    private:
        HRESULT SendInvite(uint64_t xuid) noexcept
        {
            Result<User> userResult = m_multiplayerService->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            RETURN_HR_IF_FAILED(httpCall->Init(
                m_multiplayerService->m_xboxLiveContextSettings,
                "POST",
                XblHttpCall::BuildUrl("sessiondirectory", "/handles"),
                xbox_live_api::send_invites
            ));

            RETURN_HR_IF_FAILED(httpCall->SetRetryAllowed(false));
            RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

            m_requestBody.SetInvitedXuid(xuid);
            RETURN_HR_IF_FAILED(httpCall->SetRequestBody(m_requestBody.Json()));

            return httpCall->Perform(AsyncContext<HttpResult>{ m_async.Queue().DeriveWorkerQueue(),
                [
                    sharedThis{ shared_from_this() },
                    this,
                    xuid
                ]
                (HttpResult httpResult)
                {
                    auto inviteHandleResult = HandleServiceResult(httpResult);
                    if (Succeeded(inviteHandleResult))
                    {
                        m_inviteHandles.push_back(inviteHandleResult.ExtractPayload());
                    }
                    else
                    {
                        LOGS_ERROR << __FUNCTION__ << " Invite failed for user[" << xuid << "], hr=" << inviteHandleResult.Hresult();
                        LOGS_ERROR << __FUNCTION__ << " Continuing with remaining invited users.";
                    }
                    Run();
                }
            });
        }

        Result<String> HandleServiceResult(HttpResult httpResult) noexcept
        {
            RETURN_HR_IF_FAILED(httpResult.Hresult());
            RETURN_HR_IF_FAILED(httpResult.Payload()->Result());

            auto deserializationResult = Serializers::DeserializeMultiplayerInvite(httpResult.Payload()->GetResponseBodyJson());
            return Result<String>{ deserializationResult.Payload().Data, deserializationResult.Hresult() };
        }

        std::shared_ptr<const MultiplayerService> m_multiplayerService;
        MultiplayerInviteHandlePostRequest m_requestBody;
        Vector<uint64_t> m_xuidsToInvite;
        Vector<String> m_inviteHandles;
        AsyncContext<Result<Vector<String>>> m_async;
    };

    auto operation = MakeShared<SendInvitesOperation>(
        shared_from_this(),
        sessionReference,
        xuids,
        titleId,
        contextStringId,
        customActivationContext,
        std::move(async)
    );

    operation->Run();
    return S_OK;
}

HRESULT MultiplayerService::GetActivitiesForSocialGroup(
    _In_ const String& scid,
    _In_ uint64_t socialGroupOwnerXuid,
    _In_ const String& socialGroup,
    _In_ AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(scid.empty() || socialGroupOwnerXuid == 0 || socialGroup.empty());

    MultiplayerActivityQueryPostRequest request{ scid, socialGroup, socialGroupOwnerXuid };

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", GET_ACTIIVITIES_SUBPATH),
        xbox_live_api::get_activities_for_social_group
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
	JsonDocument requestJson;
    request.Serialize(requestJson, requestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(requestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        auto hr = httpResult.Hresult();
        if (FAILED(hr))
        {
            return async.Complete({ hr, "Http call failed" });
        }

        hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        Vector<XblMultiplayerActivityDetails> activityDetails;
        hr = JsonUtils::ExtractJsonVector<XblMultiplayerActivityDetails>(
            Serializers::DeserializeMultiplayerActivityDetails,
            httpResult.Payload()->GetResponseBodyJson(),
            "results",
            activityDetails,
            true
        );

        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        async.Complete(activityDetails);
    }});
}

HRESULT MultiplayerService::GetActivitiesForUsers(
    _In_ const String& scid,
    _In_ const Vector<uint64_t>& xuids,
    _In_ AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(scid.empty() || xuids.empty());

    MultiplayerActivityQueryPostRequest request{ scid, xuids };

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", GET_ACTIIVITIES_SUBPATH),
        xbox_live_api::get_activities_for_users
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
    JsonDocument requestJson;
    request.Serialize(requestJson, requestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(requestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            return async.Complete({ httpResult.Hresult(), "Http call failed" });
        }

        auto hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        Vector<XblMultiplayerActivityDetails> activityDetails;
        hr = JsonUtils::ExtractJsonVector<XblMultiplayerActivityDetails>(
            Serializers::DeserializeMultiplayerActivityDetails,
            httpResult.Payload()->GetResponseBodyJson(),
            "results",
            activityDetails,
            true
        );

        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        async.Complete(activityDetails);
    }});
}

HRESULT MultiplayerService::GetSearchHandles(
    _In_ const MultiplayerQuerySearchHandleRequest& searchHandleRequest,
    _In_ AsyncContext<Result<Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(searchHandleRequest.Scid().empty() || searchHandleRequest.SessionTemplateName().empty());

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("sessiondirectory", GET_SEARCH_HANDLES_SUBPATH),
        xbox_live_api::get_search_handles
    ));

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));
	JsonDocument searchHandleRequestJson;
    searchHandleRequest.Serialize(m_user.Xuid(), searchHandleRequestJson, searchHandleRequestJson.GetAllocator());
    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(searchHandleRequestJson)));

    return httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
        [async](HttpResult httpResult)
    {
        if (FAILED(httpResult.Hresult()))
        {
            return async.Complete({ httpResult.Hresult(), "Http call failed" });
        }

        auto hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            const char* errorMessagePtr{};
            std::unique_ptr<const char> errorMessage{ errorMessagePtr };
            httpResult.Payload()->GetErrorMessage(&errorMessagePtr);
            return async.Complete({ hr, errorMessagePtr });
        }

        Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>> searchHandleDetails;
        hr = JsonUtils::ExtractJsonVector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>(
            XblMultiplayerSearchHandleDetails::Deserialize,
            httpResult.Payload()->GetResponseBodyJson(),
            "results",
            searchHandleDetails,
            true
            );

        async.Complete(Result<Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>>{ searchHandleDetails, hr });
    }});
}

HRESULT MultiplayerService::WriteSessionUsingSubpath(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ XblMultiplayerSessionWriteMode mode,
    _In_ const String& subpathAndQuery,
    _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(subpathAndQuery.empty());

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "PUT",
        XblHttpCall::BuildUrl("sessiondirectory", subpathAndQuery),
        xbox_live_api::write_session_using_subpath
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRetryAllowed(false));
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(MULTIPLAYER_SERVICE_CONTRACT_VERSION));

    switch (mode)
    {
    case XblMultiplayerSessionWriteMode::CreateNew:
    {
        RETURN_HR_IF_FAILED(httpCall->SetHeader("If-None-Match", "*"));
        break;
    }
    case XblMultiplayerSessionWriteMode::UpdateExisting:
    {
        RETURN_HR_IF_FAILED(httpCall->SetHeader("If-Match", "*"));
        break;
    }
    case XblMultiplayerSessionWriteMode::UpdateOrCreateNew:
    {
        // No match header
        break;
    }
    case XblMultiplayerSessionWriteMode::SynchronizedUpdate:
    {
        if (session->ETag().empty())
        {
            RETURN_HR_IF_FAILED(httpCall->SetHeader("If-None-Match", "*"));
        }
        else
        {
            RETURN_HR_IF_FAILED(httpCall->SetHeader("If-Match", session->ETag()));
        }
        break;
    }
    default:
    {
        return E_INVALIDARG;
    }
    }

    // Set the ConnectionId for the session
    TaskQueue derivedQueue{ async.Queue().DeriveWorkerQueue() };

    return SetRtaConnectionId(session, AsyncContext<Result<void>>{ derivedQueue,
        [
            httpCall,
            xuid{ m_user.Xuid() },
            sessionReference{ session->SessionReference() },
            session,
            async{ std::move(async) }
        ]
    (Result<void> setConnectionIdResult)
    {
        if (Failed(setConnectionIdResult))
        {
            return async.Complete({ setConnectionIdResult.Hresult(), "Failed to establish MPSD RTA subscription" });
        }

        JsonDocument requestBody{ rapidjson::kObjectType };
        session->Serialize(requestBody, requestBody.GetAllocator());

        HRESULT hr = httpCall->SetRequestBody(requestBody);
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        hr = httpCall->Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
            [
                xuid,
                sessionReference,
                async
            ]
        (HttpResult httpResult)
        {
            HRESULT hr = httpResult.Hresult();
            if (FAILED(hr))
            {
                return async.Complete({ hr, "Http call failed" });
            }

            hr = httpResult.Payload()->Result();
            auto statusCode = httpResult.Payload()->HttpStatus();
            if (FAILED(hr) && statusCode != 412)
            {
                return async.Complete(hr);
            }
            else if (statusCode == 204)
            {
                // Consistent with XDK behavior, return success on 204 when writing session
                return async.Complete(S_OK);
            }

            auto responseJson = httpResult.Payload()->GetResponseBodyJson();
            if (responseJson.IsNull())
            {
                return async.Complete(hr);
            }

            XblMultiplayerSessionReference localSessionRef;
            if (sessionReference.Scid[0] == 0)
            {
                auto contentLocation = httpResult.Payload()->GetResponseHeader("Content-Location");

                hr = XblMultiplayerSessionReferenceParseFromUriPath(contentLocation.c_str(), &localSessionRef);
                if (FAILED(hr))
                {
                    return async.Complete({ E_FAIL, "Failed to parse session reference from URI" });
                }
            }
            else
            {
                localSessionRef = sessionReference;
            }

            auto session = MakeShared<XblMultiplayerSession>(
                xuid,
                localSessionRef,
                httpResult.Payload()->GetResponseHeader(ETAG_HEADER),
                httpResult.Payload()->GetResponseHeader(DATE_HEADER),
                httpResult.Payload()->GetResponseBodyJson()
                );

            if (FAILED(session->DeserializationError()) && SUCCEEDED(hr))
            {
                // WriteSession failed due to deserialization error
                hr = session->DeserializationError();
            }

            session->SetWriteSessionStatus(
                statusCode
            );

            return async.Complete(Result<std::shared_ptr<XblMultiplayerSession>>(session, hr));
        }});

        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
    }
    });
}

HRESULT MultiplayerService::SetRtaConnectionId(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ AsyncContext<Result<void>> async
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    XblMultiplayerSessionReadLockGuard sessionSafe(session);
    if (!m_subscription || !sessionSafe.CurrentUserInternal())
    {
        // If we don't have an active subscription or the current user is not in the session do nothing
        async.Complete(S_OK);
    }
    else if (!m_subscription->RtaConnectionId().empty())
    {
        // If we already have a connectionId add it
        sessionSafe.CurrentUserInternal()->SetRtaConnectionId(m_subscription->RtaConnectionId());
        async.Complete(S_OK);
    }
    else
    {
        // Wait for subscription to be finalized and add connectionId then
        m_sessionsAwaitingConnectionId.emplace_back(session, std::move(async));
    }
    return S_OK;
}

HRESULT MultiplayerService::SubscribeToRta(std::unique_lock<std::mutex> lock) noexcept
{
    if (m_subscription == nullptr)
    {
        m_subscription = MakeShared<MultiplayerSubscription>();

        m_subscription->AddConnectionIdChangedHandler(
            [
                weakThis = std::weak_ptr<MultiplayerService>{ shared_from_this() }
            ]
        (const String& connectionId)
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                std::unique_lock<std::mutex> lock{ sharedThis->m_mutexMultiplayerService };
                for (auto& pair : sharedThis->m_sessionsAwaitingConnectionId)
                {
                    // Make sure the current user is still in the session
                    XblMultiplayerSessionReadLockGuard pairSafe(pair.first);
                    if (pairSafe.CurrentUserInternal())
                    {
                        pairSafe.CurrentUserInternal()->SetRtaConnectionId(connectionId);
                    }
                    pair.second.Complete(S_OK);
                }
                sharedThis->m_sessionsAwaitingConnectionId.clear();

                // Invoke client connectionId changed handlers as well
                auto clientHandlers{ sharedThis->m_connectionIdChangedHandlers };
                lock.unlock();

                for (auto& handler : clientHandlers)
                {
                    handler.second(connectionId);
                }
            }
        });

        // To support title subscription lost events, add an RTA connection state changed handler
        m_rtaConnectionStateChangedToken = m_rtaManager->AddStateChangedHandler(m_user, 
            [
                weakThis = std::weak_ptr<MultiplayerService>{ shared_from_this() }
            ]
        (XblRealTimeActivityConnectionState state)
        {
            auto sharedThis{ weakThis.lock() };
            if (state == XblRealTimeActivityConnectionState::Disconnected && sharedThis)
            {
                std::unique_lock<std::mutex> lock{ sharedThis->m_mutexMultiplayerService };
                auto handlers{ sharedThis->m_subscriptionLostHandlers };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second();
                }

                // If there were sessions awaiting a connectionId, complete those AsyncContexts
                for (auto& pair : sharedThis->m_sessionsAwaitingConnectionId)
                {
                    pair.second.Complete(S_OK);
                }
                sharedThis->m_sessionsAwaitingConnectionId.clear();
            }
        });

        // Unlock before adding subscription as it can synchronously call back into our ConnectionIdChanged handler
        lock.unlock();
        return m_rtaManager->AddSubscription(m_user, m_subscription);
    }
    return S_OK;
}

HRESULT MultiplayerService::UnsubscribeFromRta() noexcept
{
    if (m_subscription != nullptr)
    {
        m_rtaManager->RemoveStateChangedHandler(m_user, m_rtaConnectionStateChangedToken);
        m_rtaManager->RemoveSubscription(m_user, m_subscription);
        m_subscription.reset();

        // If there were sessions awaiting a connectionId, complete those AsyncContexts
        for (auto& pair : m_sessionsAwaitingConnectionId)
        {
            pair.second.Complete(S_OK);
        }
        m_sessionsAwaitingConnectionId.clear();
    }
    return S_OK;
}

HRESULT MultiplayerService::EnableMultiplayerSubscriptions() noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutexMultiplayerService };
    m_forceEnableRtaSubscription = true;
    return SubscribeToRta(std::move(lock));
}

HRESULT MultiplayerService::DisableMultiplayerSubscriptions() noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutexMultiplayerService };
    m_forceEnableRtaSubscription = false;
    HRESULT hr = UnsubscribeFromRta();

    // Maintain existing behavior and invoke subscription lost handler here
    auto handlers{ m_subscriptionLostHandlers };
    lock.unlock();

    for (auto& handler : handlers)
    {
        handler.second();
    }

    return hr;
}

bool MultiplayerService::SubscriptionsEnabled() noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    return m_subscription != nullptr;
}

XblFunctionContext MultiplayerService::AddMultiplayerSessionChangedHandler(
    _In_ MultiplayerSubscription::SessionChangedHandler handler
) noexcept
{
    {
        std::unique_lock<std::mutex> lock{ m_mutexMultiplayerService };
        SubscribeToRta(std::move(lock));
    }

    XblFunctionContext token{};
    {
        std::unique_lock<std::mutex> lock{ m_mutexMultiplayerService };
        if (m_subscription)
        {
            token = m_subscription->AddSessionChangedHandler(std::move(handler));
        }
    }

    return token;
}

void MultiplayerService::RemoveMultiplayerSessionChangedHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };

    if (m_subscription)
    {
        size_t remainingHandlers = m_subscription->RemoveSessionChangedHandler(token);
        // If that was the last handler and the title hasn't force enabled the RTA subscription then unsubscribe
        if (!remainingHandlers && !m_forceEnableRtaSubscription)
        {
            UnsubscribeFromRta();
        }
    }
}

XblFunctionContext MultiplayerService::AddMultiplayerSubscriptionLostHandler(
    _In_ SubscriptionLostHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    m_subscriptionLostHandlers[m_nextClientToken] = std::move(handler);
    return m_nextClientToken++;
}

void MultiplayerService::RemoveMultiplayerSubscriptionLostHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    m_subscriptionLostHandlers.erase(token);
}

XblFunctionContext MultiplayerService::AddMultiplayerConnectionIdChangedHandler(
    _In_ MultiplayerSubscription::ConnectionIdChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };

    // For legacy reasons, allow adding a connectionId changed handler even if subscriptions are not enabled
    m_connectionIdChangedHandlers[m_nextClientToken] = std::move(handler);
    return m_nextClientToken++;
}

void MultiplayerService::RemoveMultiplayerConnectionIdChangedHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerService };
    m_connectionIdChangedHandlers.erase(token);
}


String MultiplayerService::MultiplayerSessionDirectoryCreateOrUpdateSubpath(
    _In_ const String& serviceConfigurationId,
    _In_ const String& sessionTemplateName,
    _In_ const String& sessionName
) noexcept
{
    Stringstream source;
    source << "/serviceconfigs/";
    source << serviceConfigurationId;
    source << "/sessionTemplates/";
    source << sessionTemplateName;
    source << "/sessions/";
    source << sessionName;
    return source.str();
}

String MultiplayerService::MultiplayerSessionDirectoryCreateOrUpdateByHandleSubpath(
    _In_ const String& handleId
) noexcept
{
    Stringstream source;
    source << "/handles/";
    source << handleId;
    source << "/session";

    return source.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END

STDAPI MultiplayerWriteSessionHelper(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblMultiplayerSessionHandle multiplayerSession,
    _In_ XblMultiplayerSessionWriteMode writeMode,
    _In_opt_ const char* handleIdArg,
    _Inout_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || multiplayerSession == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            inputSession{ multiplayerSession->shared_from_this() },
            writeMode,
            handleId{ String{handleIdArg ? handleIdArg : ""} },
            outputSession{ std::shared_ptr<XblMultiplayerSession>() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async{ data->async->queue,
                [
                    &outputSession,
                    async{ data->async }
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> result)
            {
                outputSession = result.ExtractPayload();
                auto hr = result.Hresult();

                // Still must return latest session to allow retries
                if (hr == HTTP_E_STATUS_PRECOND_FAILED)
                {
                    hr = S_OK;
                }

                XAsyncComplete(async, hr, sizeof(XblMultiplayerSessionHandle));
            }
            };

            if (handleId.empty())
            {
                RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->WriteSession(
                    inputSession,
                    writeMode,
                    std::move(async)
                ));
            }
            else
            {
                RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->WriteSessionByHandle(
                    inputSession,
                    writeMode,
                    handleId,
                    std::move(async)
                ));
            }

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto handlePtr = static_cast<XblMultiplayerSessionHandle*>(data->buffer);
            if (outputSession)
            {
                outputSession->AddRef();
                *handlePtr = outputSession.get();
            }
            else
            {
                *handlePtr = nullptr;
            }
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerWriteSessionAsync(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionHandle multiplayerSession,
    _In_ XblMultiplayerSessionWriteMode writeMode,
    _Inout_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(multiplayerSession);

    if (!XblMultiplayerSessionReferenceIsValid(&multiplayerSession->SessionReference()))
    {
        LOGS_DEBUG << "XblMultiplayerWriteSessionAsync cannot be called on a session without a valid session reference";
        return E_XBL_RUNTIME_ERROR;
    }

    return MultiplayerWriteSessionHelper(xblContext, multiplayerSession, writeMode, nullptr, async);
}
CATCH_RETURN()

STDAPI XblMultiplayerWriteSessionResult(
    _Inout_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT
try
{
    XblMultiplayerSessionHandle handleCopy = nullptr;
    auto hr = XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerSessionHandle), &handleCopy, nullptr);
    if (handle != nullptr)
    {
        *handle = handleCopy;
    }
    else
    {
        XblMultiplayerSessionCloseHandle(handleCopy);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerWriteSessionByHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionHandle multiplayerSession,
    _In_ XblMultiplayerSessionWriteMode writeMode,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(handleId);

    return MultiplayerWriteSessionHelper(xblContext, multiplayerSession, writeMode, handleId, async);
}
CATCH_RETURN()

STDAPI XblMultiplayerWriteSessionByHandleResult(
    _Inout_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT
try
{
    return XblMultiplayerWriteSessionResult(async, handle);
}
CATCH_RETURN()

STDAPI MultiplayerGetSessionHelper(
    _In_ XblContextHandle xblContextHandle,
    _In_opt_ const XblMultiplayerSessionReference* sessionReferenceArg,
    _In_opt_ const char* handleIdArg,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            sessionReference = sessionReferenceArg ? XblMultiplayerSessionReference{ *sessionReferenceArg } : XblMultiplayerSessionReference{},
            handleId = handleIdArg ? String{ handleIdArg } : String{},
            session = std::shared_ptr<XblMultiplayerSession>{ nullptr }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async{ data->async->queue,
                [
                    &session,
                    async{ data->async }
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> result)
            {
                session = result.ExtractPayload();
                XAsyncComplete(async, result.Hresult(), sizeof(XblMultiplayerSessionHandle));
            }
            };

            if (handleId.empty())
            {
                RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetCurrentSession(sessionReference, std::move(async)));
            }
            else
            {
                RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetCurrentSessionByHandle(handleId, std::move(async)));
            }

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto handlePtr = static_cast<XblMultiplayerSessionHandle*>(data->buffer);
            if (session)
            {
                session->AddRef();
                *handlePtr = session.get();
            }
            else
            {
                *handlePtr = nullptr;
            }
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSessionAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(sessionReference);
    return MultiplayerGetSessionHelper(xblContext, sessionReference, nullptr, async);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSessionResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerSessionHandle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSessionByHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(handleId);
    return MultiplayerGetSessionHelper(xblContext, nullptr, handleId, async);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSessionByHandleResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerSessionHandle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerQuerySessionsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblMultiplayerSessionQuery* sessionQuery,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || sessionQuery == nullptr || async == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(sessionQuery->Scid);
    RETURN_HR_INVALIDARGUMENT_IF((sessionQuery->XuidFilters == nullptr || sessionQuery->XuidFiltersCount == 0) &&
        (sessionQuery->KeywordFilter == nullptr || sessionQuery->KeywordFilter[0] == 0));
    RETURN_HR_INVALIDARGUMENT_IF(sessionQuery->IncludeReservations && (sessionQuery->XuidFilters == nullptr || sessionQuery->XuidFiltersCount == 0));
    RETURN_HR_INVALIDARGUMENT_IF(sessionQuery->IncludeInactiveSessions && (sessionQuery->XuidFilters == nullptr || sessionQuery->XuidFiltersCount == 0));
    RETURN_HR_INVALIDARGUMENT_IF(sessionQuery->VisibilityFilter == XblMultiplayerSessionVisibility::Unknown);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            query = SessionQuery{ sessionQuery },
            sessions = Vector<XblMultiplayerSessionQueryResult>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetSessions(
                query,
                AsyncContext<Result<Vector<XblMultiplayerSessionQueryResult>>>{ data->async->queue,
                [
                    &sessions,
                    async{ data->async }
                ]
            (Result<Vector<XblMultiplayerSessionQueryResult>> result)
            {
                sessions = result.ExtractPayload();
                XAsyncComplete(async, result.Hresult(), sizeof(XblMultiplayerSessionQueryResult) * sessions.size());
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            memcpy(data->buffer, sessions.data(), data->bufferSize);
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerQuerySessionsResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* sessionCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || sessionCount == nullptr);

    size_t sizeInBytes;
    auto hr = XAsyncGetResultSize(async, &sizeInBytes);
    *sessionCount = sizeInBytes / sizeof(XblMultiplayerSessionQueryResult);
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerQuerySessionsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t sessionCount,
    _Out_writes_(sessionCount) XblMultiplayerSessionQueryResult* sessions
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(sessionCount == 0, S_OK);
    return XAsyncGetResult(async, nullptr, sessionCount * sizeof(XblMultiplayerSessionQueryResult), sessions, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerSetActivityAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblMultiplayerSessionReference* sessionReferenceArg,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || sessionReferenceArg == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            sessionReference{ *sessionReferenceArg }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->SetActivity(sessionReference, data->async));
            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerClearActivityAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_z_ const char* scidArg,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || scidArg == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            scid = String{ scidArg }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->ClearActivity(scid, data->async));
            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerSendInvitesAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ uint32_t titleId,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || sessionReference == nullptr || xuids == nullptr || xuidsCount == 0 || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            sessionRef{ *sessionReference },
            xuidsVector{ Vector<uint64_t>(xuids, xuids + xuidsCount) },
            titleId,
            contextString{ String{ contextStringId ? contextStringId : "" } },
            activiationContext{ String{customActivationContext ? customActivationContext : ""} },
            inviteHandles{ Vector<String>() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->SendInvites(
                sessionRef,
                xuidsVector,
                titleId,
                contextString,
                activiationContext,
                AsyncContext<Result<Vector<String>>>{ data->async->queue,
                [
                    &inviteHandles,
                    async{ data->async }
                ]
            (Result<Vector<String>> result)
            {
                if (Succeeded(result))
                {
                    inviteHandles = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), sizeof(XblMultiplayerInviteHandle) * inviteHandles.size());
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto handlesArray = static_cast<XblMultiplayerInviteHandle*>(data->buffer);
            for (uint32_t i = 0; i < inviteHandles.size(); ++i)
            {
                utils::strcpy(handlesArray[i].Data, sizeof(handlesArray[i].Data), inviteHandles[i].data());
            }
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerSendInvitesResult(
    _In_ XAsyncBlock* async,
    _In_ size_t handlesCount,
    _Out_writes_(handlesCount) XblMultiplayerInviteHandle* handles
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(handlesCount == 0, S_OK);
    return XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerInviteHandle) * handlesCount, handles, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForSocialGroupAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char* scidArg,
    _In_ uint64_t socialGroupOwnerXuid,
    _In_ const char* socialGroupArg,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || scidArg == nullptr || socialGroupOwnerXuid == 0 || socialGroupArg == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            scid = String{ scidArg },
            socialGroupOwnerXuid,
            socialGroup = String{ socialGroupArg },
            activityDetails = Vector<XblMultiplayerActivityDetails>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetActivitiesForSocialGroup(
                scid,
                socialGroupOwnerXuid,
                socialGroup,
                AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ data->async->queue,
                [
                    &activityDetails,
                    async{ data->async }
                ]
            (Result<Vector<XblMultiplayerActivityDetails>> result)
            {
                activityDetails = result.ExtractPayload();
                XAsyncComplete(async, result.Hresult(), sizeof(XblMultiplayerActivityDetails) * activityDetails.size());
            }
            }));
            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            for (auto& activity : activityDetails)
            {
                Delete(activity.CustomSessionPropertiesJson);
                activity.CustomSessionPropertiesJson = nullptr;
            }

            memcpy(data->buffer, activityDetails.data(), data->bufferSize);
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char* scidArg,
    _In_ uint64_t socialGroupOwnerXuid,
    _In_ const char* socialGroupArg,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || scidArg == nullptr || socialGroupOwnerXuid == 0 || socialGroupArg == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            scid = String{ scidArg },
            socialGroupOwnerXuid,
            socialGroup = String{ socialGroupArg },
            activityDetails = Vector<XblMultiplayerActivityDetails>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetActivitiesForSocialGroup(
                scid,
                socialGroupOwnerXuid,
                socialGroup,
                AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ data->async->queue,
                [
                    &activityDetails,
                    async{ data->async }
                ]
            (Result<Vector<XblMultiplayerActivityDetails>> result)
            {
                activityDetails = result.ExtractPayload();
                auto hr = result.Hresult();

                size_t jsonSize{};
                for (auto& activity : activityDetails)
                {
                    jsonSize += strlen(activity.CustomSessionPropertiesJson) + 1;
                }

                size_t bufferSize = jsonSize + sizeof(XblMultiplayerActivityDetails) * activityDetails.size();
                bufferSize = static_cast<size_t>((bufferSize + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
                XAsyncComplete(async, hr, bufferSize);
            }
            }));
            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto activityPtr = reinterpret_cast<XblMultiplayerActivityDetails*>(data->buffer);
            auto jsonPtr = reinterpret_cast<char*>(data->buffer) + sizeof(XblMultiplayerActivityDetails) * activityDetails.size();

            for (auto& activity : activityDetails)
            {
                size_t len = strlen(activity.CustomSessionPropertiesJson) + 1;

                *activityPtr = activity;
                utils::strcpy(jsonPtr, len, activity.CustomSessionPropertiesJson);
                activityPtr->CustomSessionPropertiesJson = jsonPtr;
                Delete(activity.CustomSessionPropertiesJson);

                jsonPtr += len;
                ++activityPtr;
            }

            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* activityCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(activityCount);

    size_t sizeInBytes;
    auto hr = XAsyncGetResultSize(async, &sizeInBytes);
    *activityCount = sizeInBytes / sizeof(XblMultiplayerActivityDetails);
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t activityCount,
    _Out_writes_(activityCount) XblMultiplayerActivityDetails* activities
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(activityCount == 0, S_OK);
    return XAsyncGetResult(async, nullptr, activityCount * sizeof(XblMultiplayerActivityDetails), activities, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(resultSizeInBytes);

    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityDetails** ptrToBuffer,
    _Out_ size_t* ptrToBufferCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(buffer == nullptr || ptrToBuffer == nullptr || ptrToBufferCount == nullptr);

    size_t bufferUsedTemp{};
    if (bufferUsed == nullptr)
    {
        bufferUsed = &bufferUsedTemp;
    }

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblMultiplayerActivityDetails*>(buffer);

        size_t count{ 0 };
        size_t verifiedSize{ 0 };
        for (; *bufferUsed > 0 && verifiedSize < *bufferUsed - XBL_ALIGN_SIZE; ++count)
        {
            verifiedSize += sizeof(XblMultiplayerActivityDetails);
            verifiedSize += strlen((*ptrToBuffer)[count].CustomSessionPropertiesJson) + 1;
        }
        verifiedSize = static_cast<size_t>((verifiedSize + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
        assert(verifiedSize == *bufferUsed);
        *ptrToBufferCount = count;
    }

    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForUsersAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char* scidArg,
    _In_reads_(xuidsCount) const uint64_t* xuidsArg,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || scidArg == nullptr || xuidsArg == nullptr || xuidsCount == 0 || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            scid = String{ scidArg },
            xuids = Vector<uint64_t>(xuidsArg, xuidsArg + xuidsCount),
            activityDetails = Vector<XblMultiplayerActivityDetails>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetActivitiesForUsers(
                scid,
                xuids,
                AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ data->async->queue,
                [
                    &activityDetails,
                    async{ data->async }
                ]
            (Result<Vector<XblMultiplayerActivityDetails>> result)
            {
                activityDetails = result.ExtractPayload();
                XAsyncComplete(async, result.Hresult(), sizeof(XblMultiplayerActivityDetails) * activityDetails.size());
            }
            }));
            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            for (auto& activity : activityDetails)
            {
                Delete(activity.CustomSessionPropertiesJson);
                activity.CustomSessionPropertiesJson = nullptr;
            }

            memcpy(data->buffer, activityDetails.data(), data->bufferSize);
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char* scidArg,
    _In_reads_(xuidsCount) const uint64_t* xuidsArg,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || scidArg == nullptr || xuidsArg == nullptr || xuidsCount == 0 || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            scid = String{ scidArg },
            xuids = Vector<uint64_t>(xuidsArg, xuidsArg + xuidsCount),
            activityDetails = Vector<XblMultiplayerActivityDetails>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->GetActivitiesForUsers(
                scid,
                xuids,
                AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ data->async->queue,
                [
                    &activityDetails,
                    async{ data->async }
                ]
            (Result<Vector<XblMultiplayerActivityDetails>> result)
            {
                activityDetails = result.ExtractPayload();
                auto hr = result.Hresult();

                size_t jsonSize{};
                for (auto& activity : activityDetails)
                {
                    jsonSize += strlen(activity.CustomSessionPropertiesJson) + 1;
                }

                size_t bufferSize = jsonSize + sizeof(XblMultiplayerActivityDetails) * activityDetails.size();
                bufferSize = static_cast<size_t>((bufferSize + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
                XAsyncComplete(async, hr, bufferSize);
            }
            }));
            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto activityPtr = reinterpret_cast<XblMultiplayerActivityDetails*>(data->buffer);
            auto jsonPtr = reinterpret_cast<char*>(data->buffer) + sizeof(XblMultiplayerActivityDetails) * activityDetails.size();

            for (auto& activity : activityDetails)
            {
                size_t len = strlen(activity.CustomSessionPropertiesJson) + 1;

                *activityPtr = activity;
                utils::strcpy(jsonPtr, len, activity.CustomSessionPropertiesJson);
                activityPtr->CustomSessionPropertiesJson = jsonPtr;
                Delete(activity.CustomSessionPropertiesJson);

                jsonPtr += len;
                ++activityPtr;
            }

            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForUsersResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* activityCount
) XBL_NOEXCEPT
try
{
    return XblMultiplayerGetActivitiesForSocialGroupResultCount(async, activityCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesForUsersResult(
    _In_ XAsyncBlock* async,
    _In_ size_t activityCount,
    _Out_writes_(activityCount) XblMultiplayerActivityDetails* activities
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, activityCount * sizeof(XblMultiplayerActivityDetails), activities, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityDetails** ptrToBuffer,
    _Out_ size_t* ptrToBufferCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    return XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(async, bufferSize, buffer, ptrToBuffer, ptrToBufferCount, bufferUsed);
}
CATCH_RETURN()

STDAPI XblMultiplayerSetSubscriptionsEnabled(
    _In_ XblContextHandle xblContext,
    _In_ bool subscriptionsEnabled
) XBL_NOEXCEPT
try
{
    if (subscriptionsEnabled)
    {
        return xblContext->MultiplayerService()->EnableMultiplayerSubscriptions();
    }
    else
    {
        return xblContext->MultiplayerService()->DisableMultiplayerSubscriptions();
    }
}
CATCH_RETURN()

STDAPI_(bool) XblMultiplayerSubscriptionsEnabled(
    _In_ XblContextHandle xblContext
) XBL_NOEXCEPT
try
{
    return xblContext->MultiplayerService()->SubscriptionsEnabled();
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblMultiplayerAddSessionChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return xblContext->MultiplayerService()->AddMultiplayerSessionChangedHandler(
        [
            handler,
            context
        ]
    (const XblMultiplayerSessionChangeEventArgs& args)
    {
        try 
        {
            handler(context, args);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerRemoveSessionChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    xblContext->MultiplayerService()->RemoveMultiplayerSessionChangedHandler(token);
}
CATCH_RETURN_WITH(;)


STDAPI_(XblFunctionContext) XblMultiplayerAddSubscriptionLostHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionSubscriptionLostHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return xblContext->MultiplayerService()->AddMultiplayerSubscriptionLostHandler(
        [
            handler,
            context
        ]
    {
        try
        {
            handler(context);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerRemoveSubscriptionLostHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    xblContext->MultiplayerService()->RemoveMultiplayerSubscriptionLostHandler(token);
}
CATCH_RETURN_WITH(;)

STDAPI_(XblFunctionContext) XblMultiplayerAddConnectionIdChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerConnectionIdChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return xblContext->MultiplayerService()->AddMultiplayerConnectionIdChangedHandler(
        [
            handler,
            context
        ]
    (const String&)
    {
        try
        {
            handler(context);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerRemoveConnectionIdChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    xblContext->MultiplayerService()->RemoveMultiplayerConnectionIdChangedHandler(token);
}
CATCH_RETURN_WITH(;)
