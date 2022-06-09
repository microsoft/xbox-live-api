// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer::manager;
using std::dynamic_pointer_cast;

STDAPI XblMultiplayerEventArgsXuid(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || xuid == nullptr);
    VERIFY_XBL_INITIALIZED();

    auto userAddedArgs = dynamic_cast<UserAddedEventArgs*>(argsHandle);
    auto userRemovedArgs = dynamic_cast<UserRemovedEventArgs*>(argsHandle);
    auto joinLobbyCompletedArgs = dynamic_cast<JoinLobbyCompletedEventArgs*>(argsHandle);

    if (userAddedArgs != nullptr)
    {
        *xuid = userAddedArgs->Xuid;
    }
    else if (userRemovedArgs != nullptr)
    {
        *xuid = userRemovedArgs->Xuid;
    }
    else if (joinLobbyCompletedArgs != nullptr)
    {
        *xuid = joinLobbyCompletedArgs->Xuid;
    }
    else
    {
        return E_INVALIDARG;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsMembersCount(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ size_t* memberCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || memberCount == nullptr);

    auto memberJoinedArgs = dynamic_cast<MemberJoinedEventArgs*>(argsHandle);
    auto memberLeftArgs = dynamic_cast<MemberLeftEventArgs*>(argsHandle);

    if (memberJoinedArgs != nullptr)
    {
        *memberCount = memberJoinedArgs->Members.size();
    }
    else if (memberLeftArgs != nullptr)
    {
        *memberCount = memberLeftArgs->Members.size();
    }
    else
    {
        return E_INVALIDARG;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsMembers(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || members == nullptr);

    auto memberJoinedArgs = dynamic_cast<MemberJoinedEventArgs*>(argsHandle);
    auto memberLeftArgs = dynamic_cast<MemberLeftEventArgs*>(argsHandle);

    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> membersVector;
    if (memberJoinedArgs != nullptr)
    {
        membersVector = memberJoinedArgs->Members;
    }
    else if (memberLeftArgs != nullptr)
    {
        membersVector = memberLeftArgs->Members;
    }
    else
    {
        return E_INVALIDARG;
    }
    RETURN_HR_INVALIDARGUMENT_IF(membersCount < membersVector.size());

    for (size_t i = 0; i < membersVector.size(); ++i)
    {
        members[i] = membersVector[i]->GetReference();
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsMember(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerManagerMember* member
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || member == nullptr);

    auto hostChangedArgs = dynamic_cast<HostChangedEventArgs*>(argsHandle);
    auto memberPropertyChangedArgs = dynamic_cast<MemberPropertyChangedEventArgs*>(argsHandle);

    if (hostChangedArgs != nullptr)
    {
        if (hostChangedArgs->HostMember != nullptr)
        {
            *member = hostChangedArgs->HostMember->GetReference();
        }
        else
        {
            return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
        }
    }
    else if (memberPropertyChangedArgs != nullptr)
    {
        if (memberPropertyChangedArgs->Member != nullptr)
        {
            *member = memberPropertyChangedArgs->Member->GetReference();
        }
        else
        {
            return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
        }
    }
    else
    {
        return E_INVALIDARG;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsPropertiesJson(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ const char** properties
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || properties == nullptr);

    auto sessionPropertyChangedArgs = dynamic_cast<SessionPropertyChangedEventArgs*>(argsHandle);
    auto memberPropertyChangedArgs = dynamic_cast<MemberPropertyChangedEventArgs*>(argsHandle);

    if (sessionPropertyChangedArgs != nullptr)
    {
        *properties = sessionPropertyChangedArgs->Properties.data();
    }
    else if (memberPropertyChangedArgs != nullptr)
    {
        *properties = memberPropertyChangedArgs->Properties.data();
    }
    else
    {
        return E_INVALIDARG;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsFindMatchCompleted(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_opt_ XblMultiplayerMatchStatus* matchStatus,
    _Out_opt_ XblMultiplayerMeasurementFailure* initializationFailureCause
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(argsHandle);
    auto args = dynamic_cast<FindMatchCompletedEventArgs*>(argsHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args);

    if (matchStatus != nullptr)
    {
        *matchStatus = args->MatchStatus;
    }
    if (initializationFailureCause != nullptr)
    {
        *initializationFailureCause = args->InitializationFailure;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsTournamentRegistrationStateChanged(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_opt_ XblTournamentRegistrationState* registrationState,
    _Out_opt_ XblTournamentRegistrationReason* registrationReason
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(argsHandle);
    UNREFERENCED_PARAMETER(registrationState);
    UNREFERENCED_PARAMETER(registrationReason);
    return E_NOTIMPL;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsTournamentGameSessionReady(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ time_t* startTime
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(argsHandle);
    UNREFERENCED_PARAMETER(startTime);

    return E_NOTIMPL;
}
CATCH_RETURN()

STDAPI XblMultiplayerEventArgsPerformQoSMeasurements(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerPerformQoSMeasurementsArgs* performQoSMeasurementsArgs
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(argsHandle == nullptr || performQoSMeasurementsArgs == nullptr);
    auto args = dynamic_cast<PerformQosMeasurementsEventArgs*>(argsHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args);

    performQoSMeasurementsArgs->remoteClientsSize = args->remoteClients.size();
    performQoSMeasurementsArgs->remoteClients = args->remoteClients.data();

    return S_OK;
}
CATCH_RETURN()
