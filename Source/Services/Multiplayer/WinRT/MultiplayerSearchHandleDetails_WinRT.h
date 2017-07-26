// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionVisibility_WinRT.h"
#include "MultiplayerSessionRestriction_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MultiplayerRoleType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents a users current search handle, along with some details about the corresponding session.
/// </summary>
public ref class MultiplayerSearchHandleDetails sealed
{
public:
    /// <summary>
    /// The multiplayer session reference that contains identifying information for the session.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// The identifier of the handles that corresponds to the multiplayer activity.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HandleId, handle_id);

    /// <summary>
    /// Owners of the session.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ SessionOwnerXboxUserIds
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>
    /// The tags that are currently set on the session.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Tags
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>
    /// The numbers metadata that is currently set on the session.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, double>^ NumbersMetadata
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, double>^ get();
    }

    /// <summary>
    /// The strings metadata that is currently set on the session.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ StringsMetadata
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

    /// <summary>
    /// A collection of role types.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ RoleTypes
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ get();
    }

    /// <summary>
    /// The visibility state of the session, which indicates Whether other users can see, or join, etc.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Visibility, visibility, MultiplayerSessionVisibility);

    /// <summary>
    /// The join restriction of the session, which applies if visibility is "open".
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(JoinRestriction, join_restriction, MultiplayerSessionRestriction);

    /// <summary>
    /// Indicates whether the session is temporarily closed for joining.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Closed, closed, bool);

    /// <summary>
    /// The number of total session member slots supported for the multiplayer activity.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MaxMembersCount, max_members_count, uint32);

    /// <summary>
    /// The number of slots occupied.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MembersCount, members_count, uint32);

    /// <summary>
    /// The time when the search handle was posted.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(HandleCreationTime, handle_creation_time);

    /// <summary>
    /// String containing custom session properties JSON blob.
    /// </summary>
    DEFINE_PROP_GET_STR_FROM_JSON_OBJ(CustomSessionPropertiesJson, custom_session_properties_json);

internal:
    MultiplayerSearchHandleDetails(
        _In_ xbox::services::multiplayer::multiplayer_search_handle_details cppObj
    );

private:
    xbox::services::multiplayer::multiplayer_search_handle_details m_cppObj;
    MultiplayerSessionReference^ m_sessionReference;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_sessionOwnerXboxUserIds;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_tags;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_stringsMetadata;
    Platform::Collections::Map<Platform::String^, double>^ m_numbersMetadata;
    Platform::Collections::Map<Platform::String^, MultiplayerRoleType^>^ m_roleTypes;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END