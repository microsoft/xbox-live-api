// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubSetting_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubProfile sealed
{
public:
    /// <summary>Name of the club</summary>
    property ClubStringSetting^ Name { ClubStringSetting^ get(); }

    /// <summary>Description of the club</summary>
    property ClubStringSetting^ Description { ClubStringSetting^ get(); }

    /// <summary>Is mature content enabled within the club</summary>
    property ClubBooleanSetting^ MatureContentEnabled { ClubBooleanSetting^ get(); }

    /// <summary>TODO</summary>
    property ClubBooleanSetting^ WatchClubTitlesOnly { ClubBooleanSetting^ get(); }

    /// <summary>Should the club show up in search results</summary>
    property ClubBooleanSetting^ IsSearchable { ClubBooleanSetting^ get(); }

    /// <summary>Should the club show up in recommendations</summary>
    property ClubBooleanSetting^ IsRecommendable { ClubBooleanSetting^ get(); }
    
    /// <summary>Can users request to join the club</summary>
    property ClubBooleanSetting^ RequestToJoinEnabled { ClubBooleanSetting^ get(); }
    
    /// <summary>Can users leave the club</summary>
    property ClubBooleanSetting^ LeaveEnabled { ClubBooleanSetting^ get(); }
    
    /// <summary>Can ownership of the club be transferred</summary>
    property ClubBooleanSetting^ TransferOwnershipEnabled { ClubBooleanSetting^ get(); }
         
    /// <summary>URL for display image</summary>
    property ClubStringSetting^ DisplayImageUrl { ClubStringSetting^ get(); }
    
    /// <summary>URL for background image</summary>
    property ClubStringSetting^ BackgroundImageUrl { ClubStringSetting^ get(); }
    
    /// <summary>Tags associated with the club (ex. "Hate-Free", "Women only")</summary>
    property ClubStringMultiSetting^ Tags { ClubStringMultiSetting^ get(); }
    
    /// <summary>The club's preferred locale</summary>
    property ClubStringSetting^ PreferredLocale { ClubStringSetting^ get(); }
    
    /// <summary>List of titles associated with the club</summary>
    property ClubStringMultiSetting^ AssociatedTitles { ClubStringMultiSetting^ get(); }
    
    /// <summary>Primary color of the club</summary>
    property ClubStringSetting^ PrimaryColor { ClubStringSetting^ get(); }
    
    /// <summary>Secondary color of the club</summary>
    property ClubStringSetting^ SecondayColor { ClubStringSetting^ get(); }
    
    /// <summary>Tertiary color of the club</summary>
    property ClubStringSetting^ TertiaryColor { ClubStringSetting^ get(); }
    
internal:
    ClubProfile(_In_ xbox::services::clubs::club_profile cppObj);

private:
    xbox::services::clubs::club_profile m_cppObj;
    ClubStringSetting^ m_name;
    ClubStringSetting^ m_description;
    ClubBooleanSetting^ m_matureContentEnabled;
    ClubBooleanSetting^ m_watchClubTitlesOnly;
    ClubBooleanSetting^ m_isSearchable;
    ClubBooleanSetting^ m_isRecommendable;
    ClubBooleanSetting^ m_requestToJoinEnabled;
    ClubBooleanSetting^ m_leaveEnabled;
    ClubBooleanSetting^ m_transferOwnershipEnabled;
    ClubStringSetting^ m_displayImageUrl;
    ClubStringSetting^ m_backgroundImageUrl;
    ClubStringMultiSetting^ m_tags;
    ClubStringSetting^ m_preferredLocale;
    ClubStringMultiSetting^ m_associatedTitles;
    ClubStringSetting^ m_primaryColor;
    ClubStringSetting^ m_secondaryColor;
    ClubStringSetting^ m_tertiaryColor;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END