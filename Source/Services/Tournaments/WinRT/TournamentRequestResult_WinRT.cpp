// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentRequestResult_WinRT.h"

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;
using namespace xbox::services::tournaments;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentRequestResult::TournamentRequestResult(
    _In_ xbox::services::tournaments::tournament_request_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_tournaments = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Tournament, tournament>(m_cppObj.tournaments())->GetView();
}

const xbox::services::tournaments::tournament_request_result&
TournamentRequestResult::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<Tournament^>^
TournamentRequestResult::Tournaments::get()
{
    return m_tournaments;
}

IAsyncOperation<TournamentRequestResult^>^
TournamentRequestResult::GetNextAsync()
{
    auto task = m_cppObj.get_next()
    .then([](xbox_live_result<tournament_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TournamentRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END