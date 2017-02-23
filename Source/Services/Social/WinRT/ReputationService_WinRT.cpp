// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ReputationService_WinRT.h"
#include "Utils_WinRT.h"
#include "utils.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::social;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

ReputationService::ReputationService(
    _In_ reputation_service cppObj
    ) :
    m_cppObj(cppObj)
{
}

IAsyncAction^
ReputationService::SubmitReputationFeedbackAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ ReputationFeedbackType reputationFeedbackType,
    _In_opt_ Platform::String^ sessionName,
    _In_opt_ Platform::String^ reasonMessage,
    _In_opt_ Platform::String^ evidenceResourceId
    )
{
    auto task = m_cppObj.submit_reputation_feedback(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        static_cast<reputation_feedback_type>(reputationFeedbackType),
        STRING_T_FROM_PLATFORM_STRING(sessionName),
        STRING_T_FROM_PLATFORM_STRING(reasonMessage),
        STRING_T_FROM_PLATFORM_STRING(evidenceResourceId)
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncAction^ 
ReputationService::SubmitBatchReputationFeedbackAsync(
    _In_ IVectorView<ReputationFeedbackItem^>^ feedbackItems
    )
{
    auto task = m_cppObj.submit_batch_reputation_feedback(
        UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<reputation_feedback_item>(feedbackItems)
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}


ReputationFeedbackItem::ReputationFeedbackItem()
{
}

ReputationFeedbackItem::ReputationFeedbackItem(
    _In_ Platform::String^ xboxUserId,
    _In_ ReputationFeedbackType reputationFeedbackType,
    _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionRef,
    _In_ Platform::String^ reasonMessage,
    _In_ Platform::String^ evidenceResourceId
    ) :
    m_cppObj(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId), 
        static_cast<reputation_feedback_type>(reputationFeedbackType),
        sessionRef == nullptr ? xbox::services::multiplayer::multiplayer_session_reference() : sessionRef->GetCppObj(),
        STRING_T_FROM_PLATFORM_STRING(reasonMessage), 
        STRING_T_FROM_PLATFORM_STRING(evidenceResourceId)
        )
{
    m_sessionRef = sessionRef;
}

Xbox::Services::Multiplayer::MultiplayerSessionReference^ ReputationFeedbackItem::SessionReference::get()
{ 
    return m_sessionRef;
}

xbox::services::social::reputation_feedback_item ReputationFeedbackItem::GetCppObj() const
{
    return m_cppObj;
}



NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END