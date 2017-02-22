// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "XboxSocialRelationshipResult_WinRT.h"
#include "SocialService_WinRT.h"
#include "Utils_WinRT.h"

using namespace pplx;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::social;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

XboxSocialRelationshipResult::XboxSocialRelationshipResult(
    _In_ xbox_social_relationship_result cppObj
    ) : m_cppObj(std::move(cppObj))
{
    Vector<XboxSocialRelationship^>^ itemsVector = ref new Vector<XboxSocialRelationship^>();
    const std::vector<xbox_social_relationship>& cppItems = m_cppObj.items();
    for (auto& cppItem : cppItems)
    {
        itemsVector->Append(ref new XboxSocialRelationship(cppItem));
    }
    m_itemView = itemsVector->GetView();
}

IVectorView<XboxSocialRelationship^>^
XboxSocialRelationshipResult::Items::get()
{
    return m_itemView;
}

IAsyncOperation<XboxSocialRelationshipResult^>^
XboxSocialRelationshipResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox::services::xbox_live_result<xbox_social_relationship_result> cppSocialRelationshipResult)
    {
        return ref new XboxSocialRelationshipResult(cppSocialRelationshipResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END