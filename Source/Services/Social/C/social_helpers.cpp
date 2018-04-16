// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/social_c.h"
#include "social_internal.h"
#include "social_helpers.h"

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbl_xbox_social_relationship_result_wrapper::xbl_xbox_social_relationship_result_wrapper(
    std::shared_ptr<xbox_social_relationship_result_internal> socialRelationshipResult
    ) :
    m_socialRelationshipResult(std::move(socialRelationshipResult))
{
    m_xblXboxSocialRelationshipResult.hasNext = m_socialRelationshipResult->has_next();
    m_xblXboxSocialRelationshipResult.itemsCount = static_cast<uint32_t>(m_socialRelationshipResult->items().size());
    m_xblXboxSocialRelationshipResult.totalCount = m_socialRelationshipResult->total_count();
    m_xblXboxSocialRelationshipResult.filter = static_cast<XblSocialRelationshipFilter>(m_socialRelationshipResult->filter());
    m_xblXboxSocialRelationshipResult.continuationSkip = m_socialRelationshipResult->continuation_skip();

    for (const auto& relationship : m_socialRelationshipResult->items())
    {
        XBL_XBOX_SOCIAL_RELATIONSHIP item;

        item.xboxUserId = relationship->xbox_user_id().data();
        item.isFavorite = relationship->is_favorite();
        item.isFollowingCaller = relationship->is_following_caller();
        item.socialNetworksCount = static_cast<uint32_t>(relationship->social_networks().size());
        item.socialNetworks = (PCSTR*)xsapi_memory::mem_alloc(relationship->social_networks().size() * sizeof(PCSTR));

        for (uint32_t i = 0; i < item.socialNetworksCount; ++i)
        {
            item.socialNetworks[i] = relationship->social_networks()[i].data();
        }
        m_items.push_back(item);
    }
    m_xblXboxSocialRelationshipResult.items = m_items.data();
}

xbl_xbox_social_relationship_result_wrapper::~xbl_xbox_social_relationship_result_wrapper()
{
    for (const auto& item : m_items)
    {
        xsapi_memory::mem_free(item.socialNetworks);
    }
}

const XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT* xbl_xbox_social_relationship_result_wrapper::xbl_xbox_social_relationship_result() const
{
    return &m_xblXboxSocialRelationshipResult;
}

xbl_social_relationship_change_event_args_wrapper::xbl_social_relationship_change_event_args_wrapper(
    std::shared_ptr<social_relationship_change_event_args_internal> eventArgs
    ) :
    m_socialRelationshipChangeEventArgs(std::move(eventArgs))
{
    m_xblSocialRelationshipChangeEventArgs.callerXboxUserId = m_socialRelationshipChangeEventArgs->caller_xbox_user_id().data();
    m_xblSocialRelationshipChangeEventArgs.socialNotification = static_cast<XBL_SOCIAL_NOTIFICATION_TYPE>(m_socialRelationshipChangeEventArgs->social_notification());
    m_xblSocialRelationshipChangeEventArgs.xboxUserIdsCount = static_cast<uint32_t>(m_socialRelationshipChangeEventArgs->xbox_user_ids().size());
    m_xblSocialRelationshipChangeEventArgs.xboxUserIds = (PCSTR*)xsapi_memory::mem_alloc(m_xblSocialRelationshipChangeEventArgs.xboxUserIdsCount * sizeof(PCSTR));

    for (uint32_t i = 0; i < m_xblSocialRelationshipChangeEventArgs.xboxUserIdsCount; ++i)
    {
        m_xblSocialRelationshipChangeEventArgs.xboxUserIds[i] = m_socialRelationshipChangeEventArgs->xbox_user_ids()[i].data();
    }
}

xbl_social_relationship_change_event_args_wrapper::~xbl_social_relationship_change_event_args_wrapper()
{
    xsapi_memory::mem_free(m_xblSocialRelationshipChangeEventArgs.xboxUserIds);
}

const XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS& xbl_social_relationship_change_event_args_wrapper::xbl_social_relationship_change_event_args() const
{
    return m_xblSocialRelationshipChangeEventArgs;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END