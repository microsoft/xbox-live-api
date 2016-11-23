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
#include "MultiplayerSearchHandleDetails_WinRT.h"
#include "MultiplayerRoleType_WinRT.h"
#include "Macros_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSearchHandleDetails::MultiplayerSearchHandleDetails(
    _In_ xbox::services::multiplayer::multiplayer_search_handle_details cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_sessionReference = ref new MultiplayerSessionReference(m_cppObj.session_reference());
    m_sessionOwnerXboxUserIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.session_owner_xbox_user_ids());
    m_tags = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.tags());

    m_stringsMetadata = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& address : m_cppObj.strings_metadata())
    {
        m_stringsMetadata->Insert(ref new Platform::String(address.first.c_str()), ref new Platform::String(address.second.c_str()));
    }

    m_numbersMetadata = ref new Platform::Collections::Map<Platform::String^, double>();
    for (const auto& address : m_cppObj.numbers_metadata())
    {
        m_numbersMetadata->Insert(ref new Platform::String(address.first.c_str()), address.second);
    }

    m_roleTypes = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<MultiplayerRoleType>(m_cppObj.role_types());
}

MultiplayerSessionReference^
MultiplayerSearchHandleDetails::SessionReference::get()
{
    return m_sessionReference;
}

IVectorView<Platform::String^>^
MultiplayerSearchHandleDetails::SessionOwnerXboxUserIds::get()
{
    return m_sessionOwnerXboxUserIds->GetView();
}

IVectorView<Platform::String^>^
MultiplayerSearchHandleDetails::Tags::get()
{
    return m_tags->GetView();
}

IMapView<Platform::String^, Platform::String^>^
MultiplayerSearchHandleDetails::StringsMetadata::get()
{
    return m_stringsMetadata->GetView();
}

IMapView<Platform::String^, double>^
MultiplayerSearchHandleDetails::NumbersMetadata::get()
{
    return m_numbersMetadata->GetView();
}

Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^
MultiplayerSearchHandleDetails::RoleTypes::get()
{
    return m_roleTypes->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
