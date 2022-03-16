// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/social_c.h"
#include "real_time_activity_subscription.h"
#include "string_array.h"

struct XblSocialRelationshipResult : public xbox::services::RefCounter, public std::enable_shared_from_this<XblSocialRelationshipResult>
{
public:
    XblSocialRelationshipResult() noexcept = default;
    XblSocialRelationshipResult(const XblSocialRelationshipResult&) = delete;
    XblSocialRelationshipResult& operator=(XblSocialRelationshipResult) = delete;

    static xbox::services::Result<std::shared_ptr<XblSocialRelationshipResult>> Deserialize(const JsonValue& json);

    const xsapi_internal_vector<XblSocialRelationship>& SocialRelationships() const noexcept;

    // Service paging metadata
    bool HasNext() const noexcept;
    size_t TotalCount() const noexcept;
    size_t ContinuationSkip{ 0 };
    XblSocialRelationshipFilter Filter{ XblSocialRelationshipFilter::All };

private:
    std::shared_ptr<RefCounter> GetSharedThis();

    size_t m_totalCount{ 0 };
    xsapi_internal_vector<XblSocialRelationship> m_socialRelationships;
    xsapi_internal_vector<xbox::services::UTF8StringArray> m_socialNetworks;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

namespace legacy
{
    class social_group_constants
    {
    public:
        /// <summary>
        /// Returns Favorites constant string
        /// </summary>
        static const char* favorite() { return "Favorites"; }

        /// <summary>
        /// Returns People constant string
        /// </summary>
        static const char* people() { return "People"; }
    };
}

typedef Callback<const XblSocialRelationshipChangeEventArgs&> SocialRelationshipChangedHandler;

class SocialRelationshipChangeSubscription : public real_time_activity::Subscription
{
public:
    SocialRelationshipChangeSubscription(_In_ uint64_t xuid) noexcept;

    XblFunctionContext AddHandler(SocialRelationshipChangedHandler handler) noexcept;
    size_t RemoveHandler(XblFunctionContext token) noexcept;

protected:
    void OnEvent(const JsonValue& data) noexcept override;

private:
    uint64_t m_xuid;
    Map<XblFunctionContext, SocialRelationshipChangedHandler> m_handlers;
    XblFunctionContext m_nextHandlerToken{ 1 };
    mutable std::mutex m_lock;
};

class SocialService : public std::enable_shared_from_this<SocialService>
{
public:
    SocialService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    ~SocialService() noexcept;

    XblFunctionContext AddSocialRelationshipChangedHandler(
        _In_ SocialRelationshipChangedHandler handler
    ) noexcept;

    void RemoveSocialRelationshipChangedHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    HRESULT GetSocialRelationships(
        _In_ uint64_t xuid,
        _In_ XblSocialRelationshipFilter filter,
        _In_ size_t startIndex,
        _In_ size_t maxItems,
        _In_ AsyncContext<Result<std::shared_ptr<XblSocialRelationshipResult>>> async
    ) const noexcept;

private:
    static xsapi_internal_string SocialRelationshipFilterToString(
        _In_ XblSocialRelationshipFilter filter
    ) noexcept;

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;

    std::shared_ptr<SocialRelationshipChangeSubscription> m_socialRelationshipChangedSubscription;
    mutable std::mutex m_lock;
};

class ReputationFeedbackRequest
{
public:
    ReputationFeedbackRequest(
        _In_ uint64_t xuid,
        _In_ XblReputationFeedbackType feedbackType,
        _In_opt_ const XblMultiplayerSessionReference* sessionReference,
        _In_z_ const char* reasonMessage,
        _In_opt_z_ const char* evidenceResourceId
    );

    ReputationFeedbackRequest(
        _In_ const XblReputationFeedbackItem* items,
        _In_ size_t itemsCount
    );

    ReputationFeedbackRequest(const ReputationFeedbackRequest& other);

    const xsapi_internal_string& PathAndQuery() const noexcept;

    const JsonValue& Body() const noexcept;

    static xsapi_internal_string ReputationFeedbackTypeToString(
        XblReputationFeedbackType feedbackType
    );

private:
    xsapi_internal_string m_pathAndQuery;
    JsonDocument m_requestBody;
};

class ReputationService : public std::enable_shared_from_this<ReputationService>
{
public:
    ReputationService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
    ) noexcept;

    HRESULT SubmitFeedback(
        const ReputationFeedbackRequest& request,
        AsyncContext<HRESULT> async
    ) const noexcept;

private:
    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END