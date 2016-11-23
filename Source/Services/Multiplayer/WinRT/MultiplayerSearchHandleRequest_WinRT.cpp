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
#include "MultiplayerSearchHandleRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::multiplayer;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSearchHandleRequest::MultiplayerSearchHandleRequest(
    _In_ xbox::services::multiplayer::multiplayer_search_handle_request cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

MultiplayerSearchHandleRequest::MultiplayerSearchHandleRequest(
    _In_ MultiplayerSessionReference^ sessionReference
    ) : m_cppObj(
        multiplayer_search_handle_request(sessionReference->GetCppObj())
    )
{
}

multiplayer_search_handle_request
MultiplayerSearchHandleRequest::GetCppObj()
{
    return m_cppObj;
}

MultiplayerSessionReference^
MultiplayerSearchHandleRequest::SessionReference::get()
{
    if (m_sessionReference == nullptr)
    {
        m_sessionReference = ref new MultiplayerSessionReference(m_cppObj.session_reference());
    }
    return m_sessionReference;
}

IVectorView<Platform::String^>^
MultiplayerSearchHandleRequest::Tags::get()
{
    return m_tags;
}

void
MultiplayerSearchHandleRequest::Tags::set(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ tags
)
{
    m_tags = tags;
    m_cppObj.set_tags(
        Microsoft::Xbox::Services::System::UtilsWinRT::CovertVectorViewToStdVectorString(m_tags)
    );
}

IMapView<Platform::String^, Platform::String^>^
MultiplayerSearchHandleRequest::StringsMetadata::get()
{
    return m_stringsMetadata;
}

void
MultiplayerSearchHandleRequest::StringsMetadata::set(
    _In_ IMapView<Platform::String^, Platform::String^>^ metadata
    )
{
    m_stringsMetadata = metadata;
    m_cppObj.set_strings_metadata(
        UtilsWinRT::CreateStdUnorderedMapStringFromMapObj(m_stringsMetadata)
    );
}

IMapView<Platform::String^, double>^
MultiplayerSearchHandleRequest::NumbersMetadata::get()
{
    return m_numbersMetadata;
}

void
MultiplayerSearchHandleRequest::NumbersMetadata::set(
    _In_ IMapView<Platform::String^, double>^ metadata
)
{
    m_numbersMetadata = metadata;
    m_cppObj.set_numbers_metadata(
        UtilsWinRT::CreateStdUnorderedMapObjectWithStringKeyFromPlatformMapObj<double>(m_numbersMetadata)
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END