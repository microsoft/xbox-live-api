//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Tournament, tournament>(m_cppObj.tournaments())->GetView();
}

const xbox::services::tournaments::tournament_request_result&
TournamentRequestResult::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<Tournament^>^
TournamentRequestResult::Items::get()
{
    return m_items;
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