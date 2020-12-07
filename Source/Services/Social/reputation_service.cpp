// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

ReputationService::ReputationService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
) noexcept :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings{ std::move(xboxLiveContextSettings) }
{
}

HRESULT ReputationService::SubmitFeedback(
    const ReputationFeedbackRequest& request,
    AsyncContext<HRESULT> async
) const noexcept
{
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("reputation", request.PathAndQuery()),
        xbox_live_api::submit_reputation_feedback
    ));

    httpCall->SetRetryAllowed(false);
    httpCall->SetXblServiceContractVersion(101);
    httpCall->SetRequestBody(request.Body());

    return httpCall->Perform({ async.Queue(),
        [async](HttpResult httpResult)
        {
            async.Complete(Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result());
        }
        });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END