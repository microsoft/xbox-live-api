// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubType_WinRT.h"
#include "ClubRole_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubStringSetting sealed
{
public:
    /// <summary>Current value of the setting</summary>
    DEFINE_PROP_GET_STR_OBJ(Value, value);
    
    /// <summary>List of possible values for the setting. Will be nullptr if the allowed values have not been configured.</summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ AllowedValues
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>Can the user viewing the setting change it</summary>
    DEFINE_PROP_GET_OBJ(CanViewerChangeSetting, can_viewer_change_setting, bool);

internal:
    ClubStringSetting(xbox::services::clubs::club_setting<string_t> cppObj);

private:
    xbox::services::clubs::club_setting<string_t> m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_allowedValues;
};

public ref class ClubStringMultiSetting sealed
{
public:
    /// <summary>Current values of the setting</summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Values
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>List of possible values for the setting. Will be nullptr if the allowed values have not been configured.</summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ AllowedValues
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>Can the user viewing the setting change it</summary>
    DEFINE_PROP_GET_OBJ(CanViewerChangeSetting, can_viewer_change_setting, bool);

internal:
    ClubStringMultiSetting(xbox::services::clubs::club_multi_setting<string_t> cppObj);

private:
    xbox::services::clubs::club_multi_setting<string_t> m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_values;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_allowedValues;
};

public ref class ClubBooleanSetting sealed
{
public:
    /// <summary>Current value of the setting</summary>
    DEFINE_PROP_GET_OBJ(Value, value, bool);

    /// <summary>Can the user viewing the setting change it</summary>
    DEFINE_PROP_GET_OBJ(CanViewerChangeSetting, can_viewer_change_setting, bool);

internal:
    ClubBooleanSetting(xbox::services::clubs::club_setting<bool> cppObj);

private:
    xbox::services::clubs::club_setting<bool> m_cppObj;
    Windows::Foundation::Collections::IVector<bool>^ m_allowedValues;
};

public ref class ClubActionSetting sealed
{
public:
    /// <summary>Role required to take an action</summary>
    DEFINE_PROP_GET_ENUM_OBJ(RequiredRole, required_role, ClubRole);

    /// <summary>List of possible values for RequiredRole. Will be nullptr if the allowed values have not been configured.</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRole>^ AllowedValues
    {
        Windows::Foundation::Collections::IVectorView<ClubRole>^ get();
    }

    /// <summary>Can the viewer change the role required to act</summary>
    DEFINE_PROP_GET_OBJ(CanViewerChangeSetting, can_viewer_change_setting, bool);

    /// <summary>Can the viewer take the action</summary>
    DEFINE_PROP_GET_OBJ(CanViewerAct, can_viewer_act, bool);

internal:
    ClubActionSetting(xbox::services::clubs::club_action_setting cppObj);

private:
    xbox::services::clubs::club_action_setting m_cppObj;
    Windows::Foundation::Collections::IVector<ClubRole>^ m_allowedValues;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END