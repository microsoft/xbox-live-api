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
#include "TeamRequestResult_WinRT.h"

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;
using namespace xbox::services::tournaments;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamRequestResult::TeamRequestResult(
    _In_ xbox::services::tournaments::team_request_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_teams = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<TeamInfo, team_info>(m_cppObj.teams())->GetView();
}

const xbox::services::tournaments::team_request_result&
TeamRequestResult::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<TeamInfo^>^
TeamRequestResult::Teams::get()
{
    return m_teams;
}

IAsyncOperation<TeamRequestResult^>^
TeamRequestResult::GetNextAsync()
{
    auto task = m_cppObj.get_next()
    .then([](xbox_live_result<team_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TeamRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END