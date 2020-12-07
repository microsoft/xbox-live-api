// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_social_relationship::xbox_social_relationship(
    const XblSocialRelationship& socialRelationship
) :
    m_xuid{ Utils::StringTFromUint64(socialRelationship.xboxUserId) },
    m_isFavorite{ socialRelationship.isFavorite },
    m_isFollowingCaller{ socialRelationship.isFollowingCaller },
    m_socialNetworks{ Utils::Transform<string_t>(socialRelationship.socialNetworks, socialRelationship.socialNetworksCount, Utils::StringTFromUtf8) }
{
}

const string_t& xbox_social_relationship::xbox_user_id() const
{
    return m_xuid;
}

bool xbox_social_relationship::is_favorite() const
{
    return m_isFavorite;
}

bool xbox_social_relationship::is_following_caller() const
{
    return m_isFollowingCaller;
}

const std::vector<string_t>& xbox_social_relationship::social_networks() const
{
    return m_socialNetworks;
}

xbox_social_relationship_result::xbox_social_relationship_result(
    XblSocialRelationshipResultHandle resultHandle,
    XblContextHandle xblContextHandle
)
{
    assert(resultHandle);
    assert(xblContextHandle);
    
    XblSocialRelationshipResultDuplicateHandle(resultHandle, &m_resultHandle);
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

xbox_social_relationship_result::xbox_social_relationship_result(
    const xbox_social_relationship_result& other
)
{
    if (other.m_resultHandle)
    {
        XblSocialRelationshipResultDuplicateHandle(other.m_resultHandle, &m_resultHandle);
    }
    if (other.m_xblContextHandle)
    {
        XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
    }
}
xbox_social_relationship_result& xbox_social_relationship_result::operator=(
    xbox_social_relationship_result other
)
{
    std::swap(m_resultHandle, other.m_resultHandle);
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    return *this;
}

xbox_social_relationship_result::~xbox_social_relationship_result()
{
    if (m_resultHandle)
    {
        XblSocialRelationshipResultCloseHandle(m_resultHandle);
    }
    if (m_xblContextHandle)
    {
        XblContextCloseHandle(m_xblContextHandle);
    }
}

std::vector<xbox_social_relationship> xbox_social_relationship_result::items() const
{
    const XblSocialRelationship* relationships{ nullptr };
    size_t relationshipsCount{ 0 };
    XblSocialRelationshipResultGetRelationships(m_resultHandle, &relationships, &relationshipsCount);

    return Utils::Transform<xbox_social_relationship>(relationships, relationshipsCount);
}

uint32_t xbox_social_relationship_result::total_count() const
{
    size_t totalCount{ 0 };
    XblSocialRelationshipResultGetTotalCount(m_resultHandle, &totalCount);
    return static_cast<uint32_t>(totalCount);
}

bool xbox_social_relationship_result::has_next() const
{
    bool hasNext{ false };
    XblSocialRelationshipResultHasNext(m_resultHandle, &hasNext);
    return hasNext;
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> xbox_social_relationship_result::get_next(
    _In_ uint32_t maxItems
)
{
    auto asyncWrapper = new AsyncWrapper<xbox_social_relationship_result>(
        [this](XAsyncBlock* async, xbox_social_relationship_result& result)
        {
            XblSocialRelationshipResultHandle resultHandle{ nullptr };
            auto hr = XblSocialRelationshipResultGetNextResult(async, &resultHandle);

            if (SUCCEEDED(hr))
            {
                result = xbox_social_relationship_result(resultHandle, m_xblContextHandle);
                XblSocialRelationshipResultCloseHandle(resultHandle);
            }

            return hr;
        });

    auto hr = XblSocialRelationshipResultGetNextAsync(m_xblContextHandle, m_resultHandle, static_cast<size_t>(maxItems), &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}

social_relationship_change_event_args::social_relationship_change_event_args(
    const XblSocialRelationshipChangeEventArgs* args
) :
    m_callerXuid{ Utils::StringTFromUint64(args->callerXboxUserId) },
    m_notificationType{ static_cast<social_notification_type>(args->socialNotification) },
    m_xuids{ Utils::Transform<string_t>(args->xboxUserIds, args->xboxUserIdsCount, Utils::StringTFromUint64) }
{
}

const string_t& social_relationship_change_event_args::caller_xbox_user_id() const
{
    return m_callerXuid;
}

social_notification_type social_relationship_change_event_args::social_notification() const
{
    return m_notificationType;
}

const std::vector<string_t>& social_relationship_change_event_args::xbox_user_ids() const
{
    return m_xuids;
}

social_relationship_change_subscription::social_relationship_change_subscription(
    _In_ XblRealTimeActivitySubscriptionHandle handle, 
    _In_ uint64_t xuid
) :
    real_time_activity_subscription(handle),
    m_xuid{ Utils::StringTFromUint64(xuid) }
{
    stringstream_t uri;
    uri << _T("http://social.xboxlive.com/users/xuid(") << m_xuid << ")/friends";
    m_resourceUri = uri.str();
}

const string_t& social_relationship_change_subscription::xbox_user_id() const
{
    return m_xuid;
}

social_service::social_service(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
    XblContextGetXboxUserId(m_xblContextHandle, &m_xuid);
}

social_service::social_service(const social_service& other)
    : m_xuid{ other.m_xuid }
{
    XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
}

social_service& social_service::operator=(social_service other)
{
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    m_xuid = other.m_xuid;
    return *this;
}

social_service::~social_service()
{
    XblContextCloseHandle(m_xblContextHandle);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> social_service::get_social_relationships(
    _In_ uint64_t xuid,
    _In_ XblSocialRelationshipFilter filter,
    _In_ size_t startIndex,
    _In_ size_t maxItems
)
{
    XblContextHandle xblContextHandle;
    XblContextDuplicateHandle(m_xblContextHandle, &xblContextHandle);
    auto asyncWrapper = new AsyncWrapper<xbox_social_relationship_result>(
        [xblContextHandle](XAsyncBlock* async, xbox_social_relationship_result& result)
    {
        XblSocialRelationshipResultHandle resultHandle{ nullptr };
        auto hr = XblSocialGetSocialRelationshipsResult(async, &resultHandle);

        if (SUCCEEDED(hr))
        {
            result = xbox_social_relationship_result(resultHandle, xblContextHandle);
            XblSocialRelationshipResultCloseHandle(resultHandle);
        }

        XblContextCloseHandle(xblContextHandle);
        return hr;
    });

    auto hr = XblSocialGetSocialRelationshipsAsync(m_xblContextHandle, xuid, filter, startIndex, maxItems, &asyncWrapper->async);
    if (FAILED(hr))
    {
        //Close the duplicated handle since it won't be closed in the callback
        XblContextCloseHandle(xblContextHandle);
    }

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> social_service::get_social_relationships()
{
    return get_social_relationships(m_xuid, XblSocialRelationshipFilter::All, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> social_service::get_social_relationships(
    _In_ xbox_social_relationship_filter socialRelationshipFilter
)
{
    return get_social_relationships(m_xuid, static_cast<XblSocialRelationshipFilter>(socialRelationshipFilter), 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> social_service::get_social_relationships(
    _In_ const string_t& xuid
)
{
    return get_social_relationships(Utils::Uint64FromStringT(xuid), XblSocialRelationshipFilter::All, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> social_service::get_social_relationships(
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems
)
{
    return get_social_relationships(m_xuid, static_cast<XblSocialRelationshipFilter>(filter), startIndex, maxItems);
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription>> social_service::subscribe_to_social_relationship_change(
    _In_ const string_t& xuidString
)
{
    auto xuid{ Utils::Uint64FromStringT(xuidString) };
    XblRealTimeActivitySubscriptionHandle subHandle{};
    auto hr = XblSocialSubscribeToSocialRelationshipChange(m_xblContextHandle, xuid, &subHandle);

    if (FAILED(hr))
    {
        return Utils::ConvertHr(hr);
    }
    return std::make_shared<social_relationship_change_subscription>(subHandle, xuid);
}

xbox_live_result<void> social_service::unsubscribe_from_social_relationship_change(
    _In_ std::shared_ptr<social_relationship_change_subscription> subscription
)
{
    return Utils::ConvertHr(XblSocialUnsubscribeFromSocialRelationshipChange(m_xblContextHandle, subscription->m_handle));
}

struct social_service::HandlerContext
{
    XblFunctionContext token{ 0 };
    std::function<void(social_relationship_change_event_args)> handler;
};

function_context social_service::add_social_relationship_changed_handler(
    _In_ std::function<void(social_relationship_change_event_args)> handler
)
{
    auto context = new HandlerContext{};
    context->handler = std::move(handler);

    context->token = XblSocialAddSocialRelationshipChangedHandler(m_xblContextHandle, 
        [](const XblSocialRelationshipChangeEventArgs* args, void* context)
        {
            auto handlerContext{ static_cast<HandlerContext*>(context) };
            handlerContext->handler(args);
        }, context);

    return context;
}

void social_service::remove_social_relationship_changed_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblSocialRemoveSocialRelationshipChangedHandler(m_xblContextHandle, handlerContext->token);
    delete handlerContext;
}

reputation_feedback_item::reputation_feedback_item(
    _In_ const string_t& xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
    _In_ const string_t& reasonMessage,
    _In_ const string_t& evidenceResourceId
) :
    m_xboxUserId{ Utils::Uint64FromStringT(xboxUserId) },
    m_reputationFeedbackType{ reputationFeedbackType },
    m_sessionRef{ std::move(sessionRef) },
    m_reasonMessage{ Utils::StringFromStringT(reasonMessage) },
    m_evidenceResourceId{ Utils::StringFromStringT(evidenceResourceId) }
{
}

string_t reputation_feedback_item::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_xboxUserId);
}

reputation_feedback_type reputation_feedback_item::feedback_type() const
{
    return m_reputationFeedbackType;
}

const xbox::services::multiplayer::multiplayer_session_reference& reputation_feedback_item::session_reference() const
{
    return m_sessionRef;
}

string_t reputation_feedback_item::reason_message() const
{
    return Utils::StringTFromUtf8(m_reasonMessage.data());
}

string_t reputation_feedback_item::evidence_resource_id() const
{
    return Utils::StringTFromUtf8(m_evidenceResourceId.data());
}

reputation_service::reputation_service(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

reputation_service::reputation_service(const reputation_service& other)
{
    XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
}

reputation_service& reputation_service::operator=(reputation_service other)
{
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    return *this;
}

reputation_service::~reputation_service()
{
    XblContextCloseHandle(m_xblContextHandle);
}

pplx::task<xbox_live_result<void>> reputation_service::submit_reputation_feedback(
    _In_ const string_t& xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_ const string_t& sessionName,
    _In_ const string_t& reasonMessage,
    _In_ const string_t& evidenceResourceId
)
{
    UNREFERENCED_PARAMETER(sessionName);

    auto asyncWrapper = new AsyncWrapper<void>(
        [](XAsyncBlock* async)
        {
            return XAsyncGetStatus(async, false);
        });

    auto hr = XblSocialSubmitReputationFeedbackAsync(
        m_xblContextHandle,
        Utils::Uint64FromStringT(xboxUserId),
        static_cast<XblReputationFeedbackType>(reputationFeedbackType),
        nullptr,
        Utils::StringFromStringT(reasonMessage).data(),
        evidenceResourceId.size() > 0 ? Utils::StringFromStringT(evidenceResourceId).data() : nullptr,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<void>> reputation_service::submit_batch_reputation_feedback(
    _In_ const std::vector<reputation_feedback_item>& feedbackItems
)
{
    auto asyncWrapper = new AsyncWrapper<void>(
        [](XAsyncBlock* async)
        {
            return XAsyncGetStatus(async, false);
        });

    auto items = Utils::Transform<XblReputationFeedbackItem>(feedbackItems, [](const reputation_feedback_item& i)
        {
            XblReputationFeedbackItem item{};
            item.xboxUserId = i.m_xboxUserId;
            item.feedbackType = static_cast<XblReputationFeedbackType>(i.m_reputationFeedbackType);
            item.sessionReference = &i.m_sessionRef.m_reference;
            if (i.m_reasonMessage.size() > 0)
            {
                item.reasonMessage = i.m_reasonMessage.data();
            }
            if (i.m_evidenceResourceId.size() > 0)
            {
                item.evidenceResourceId = i.m_evidenceResourceId.data();
            }
            return item;
        });

    auto hr = XblSocialSubmitBatchReputationFeedbackAsync(
        m_xblContextHandle,
        items.data(),
        items.size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END

XBL_WARNING_POP
