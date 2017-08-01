// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/achievements.h"
#include "AchievementOrderBy_winrt.h"
#include "AchievementProgression_winrt.h"
#include "AchievementType_winrt.h"
#include "MediaAsset_winrt.h"
#include "TimeWindow_winrt.h"
#include "TitleAssociation_winrt.h"
#include "Reward_winrt.h"
#include "ParticipationType_winrt.h"
#include "ProgressState_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents an achievement, a system-wide mechanism for directing and
/// rewarding users' in-game actions consistently across all games.
/// </summary>
public ref class Achievement sealed
{
public:
    /// <summary>
    /// The achievement ID. Can be a uint or a guid.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// The ID of the service configuration set associated with the achievement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The localized achievement name.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The game/app titles associated with the achievement.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<AchievementTitleAssociation^>^ TitleAssociations { Windows::Foundation::Collections::IVectorView<AchievementTitleAssociation^>^ get(); }

    /// <summary>
    /// The state of a user's progress towards the earning of the achievement.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ProgressState, progress_state, Microsoft::Xbox::Services::Achievements::AchievementProgressState);

    /// <summary>
    /// The progression object containing progress details about the achievement,
    /// including requirements.
    /// </summary>
    property Microsoft::Xbox::Services::Achievements::AchievementProgression^ Progression { Microsoft::Xbox::Services::Achievements::AchievementProgression^ get(); }

    /// <summary>
    /// The media assets associated with the achievement, such as image IDs.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<AchievementMediaAsset^>^ MediaAssets { Windows::Foundation::Collections::IVectorView<AchievementMediaAsset^>^ get(); }

    /// <summary>
    /// The collection of platforms that the achievement is available on.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ PlatformsAvailableOn { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

    /// <summary>
    /// Whether or not the achievement is secret.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsSecret, is_secret, bool);

    /// <summary>
    /// The description of the unlocked achievement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(UnlockedDescription, unlocked_description);

    /// <summary>
    /// The description of the locked achievement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(LockedDescription, locked_description);

    /// <summary>
    /// The ProductId the achievement was released with. This is a globally unique identifier that
    /// may correspond to an application, downloadable content, etc.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ProductId, product_id);

    /// <summary>
    /// The type of achievement, such as a challenge achievement.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(AchievementType, type, Microsoft::Xbox::Services::Achievements::AchievementType);

    /// <summary>
    /// The participation type for the achievement, such as group or individual.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ParticipationType, participation_type, Microsoft::Xbox::Services::Achievements::AchievementParticipationType);

    /// <summary>
    /// The time window during which the achievement is available. Applies to Challenges.
    /// </summary>
    property AchievementTimeWindow^ Available { AchievementTimeWindow^ get(); }

    /// <summary>
    /// The collection of rewards that the player earns when the achievement is unlocked.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<AchievementReward^>^ Rewards { Windows::Foundation::Collections::IVectorView<AchievementReward^>^ get(); }

    /// <summary>
    /// The estimated time that the achievement takes to be earned.
    /// </summary>
    property Windows::Foundation::TimeSpan EstimatedUnlockTime { Windows::Foundation::TimeSpan get(); }

    /// <summary>
    /// A deeplink for clients that enables the title to launch at a desired starting point
    /// for the achievement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DeepLink, deep_link);

    /// <summary>
    /// A value that indicates whether or not the achievement is revoked by enforcement.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsRevoked, is_revoked, bool);

internal:
    Achievement(
        _In_ xbox::services::achievements::achievement cppObj
        );

private:
    xbox::services::achievements::achievement m_cppObj;
    Windows::Foundation::Collections::IVector<AchievementTitleAssociation^>^ m_titleAssociations;
    Windows::Foundation::Collections::IVector<AchievementMediaAsset^>^ m_mediaAssets;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_platformsAvailableOn;
    Microsoft::Xbox::Services::Achievements::AchievementType m_achievementType;
    Microsoft::Xbox::Services::Achievements::AchievementParticipationType m_participationType;
    AchievementTimeWindow^ m_available;
    Windows::Foundation::Collections::IVector<AchievementReward^>^ m_rewards;
    Windows::Foundation::TimeSpan m_estimatedUnlockTime;
    AchievementProgression^ m_progression;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END