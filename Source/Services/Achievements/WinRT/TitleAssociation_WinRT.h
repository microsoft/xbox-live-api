//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents the association between a title and achievements.
/// </summary>
public ref class AchievementTitleAssociation sealed
{
public:
    /// <summary>
    /// The localized name of the title.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The title ID.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

internal:
    AchievementTitleAssociation(
        _In_ xbox::services::achievements::achievement_title_association cppObj
        ) :m_cppObj(std::move(cppObj))
    {
    }

private:
    xbox::services::achievements::achievement_title_association m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
