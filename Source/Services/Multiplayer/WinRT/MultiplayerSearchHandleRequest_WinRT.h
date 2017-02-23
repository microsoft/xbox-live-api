// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Sets the search handle based on the configuration of this request.
/// </summary>
public ref class MultiplayerSearchHandleRequest sealed
{
public:
    /// <summary>
    /// Creates a MultiplayerSearchHandleRequest object.
    /// </summary> 
    /// <param name="sessionRef">The session referenceid that the sessions part of.</param>
    MultiplayerSearchHandleRequest(
        _In_ MultiplayerSessionReference^ sessionReference
    );

    /// <summary>
    /// The session reference that the sessions part of.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// Filter result to just sessions with the tags set.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Tags 
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
        void set(_In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ tags);
    }

    /// <summary>
    /// Filter result to just sessions with the numbers metadata.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, double>^ NumbersMetadata
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, double>^ get();
        void set(_In_ Windows::Foundation::Collections::IMapView<Platform::String^, double>^ metadata);
    }

    /// <summary>
    /// Filter result to just sessions with the strings metadata.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ StringsMetadata
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
        void set(_In_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ metadata);
    }

internal:
    MultiplayerSearchHandleRequest(
        _In_ xbox::services::multiplayer::multiplayer_search_handle_request cppObj
    );

    xbox::services::multiplayer::multiplayer_search_handle_request GetCppObj();

private:
    xbox::services::multiplayer::multiplayer_search_handle_request m_cppObj;
    MultiplayerSessionReference^ m_sessionReference;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_tags;
    Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ m_stringsMetadata;
    Windows::Foundation::Collections::IMapView<Platform::String^, double>^ m_numbersMetadata;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END