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
#include "MultiplayerSessionProperties_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionProperties::MultiplayerSessionProperties(
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_properties> cppObj
    ) :
    m_cppObj(cppObj)
{
    THROW_INVALIDARGUMENT_IF_NULL(m_cppObj);
    m_turnCollection = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSessionMember>(m_cppObj->turn_collection())->GetView();

    Vector<uint32>^ responseVector = ref new Vector<uint32>();
    const std::vector<uint32_t>& cppSessionOwners = m_cppObj->session_owner_indices();
    for (auto& owner : cppSessionOwners)
    {
        responseVector->Append(owner);
    }
    m_sessionOwnerIndices = responseVector->GetView();
}


IVectorView<Platform::String^>^ 
MultiplayerSessionProperties::Keywords::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->keywords())->GetView();
}

void
MultiplayerSessionProperties::Keywords::set(_In_ IVectorView<Platform::String^>^ value)
{
   m_cppObj->set_keywords( UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(value) );
}

IVectorView<MultiplayerSessionMember^>^
MultiplayerSessionProperties::TurnCollection::get()
{
    return m_turnCollection;
}

void
MultiplayerSessionProperties::TurnCollection::set(_In_ IVectorView<MultiplayerSessionMember^>^ value)
{
    m_cppObj->set_turn_collection(UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>>(value));
    m_turnCollection = value;
}

IVectorView<Platform::String^>^ 
#pragma warning(suppress: 28251)
MultiplayerSessionProperties::ServerConnectionStringCandidates::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->server_connection_string_candidates())->GetView();
}

IVectorView<uint32>^
MultiplayerSessionProperties::SessionOwnerIndices::get()
{
    return m_sessionOwnerIndices;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END