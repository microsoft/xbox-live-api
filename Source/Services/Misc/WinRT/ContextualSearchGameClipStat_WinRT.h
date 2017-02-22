// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Contains information about a contextual search game clip stat.</summary>
public ref class ContextualSearchGameClipStat sealed
{
public:
    /// <summary>
    /// Name of the stat
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, stat_name);

    /// <summary>
    /// Value of the stat for string stats and integer stats representing a set value.
    /// If the stat changed during the clip, the field will have multiple values separated by a comma.
    /// If the "value" field is present, "min", "max" and "delta" fields will not be present.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Value, value);

    /// <summary>
    /// The minimum value the stat had during the recording of the game clip.
    /// If the "min" field is present, the "value" field will not be present.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(MinValue, min_value);

    /// <summary>
    /// The maximum value the stat had during the recording of the game clip.
    /// If the "max" field is present, the "value" field will not be present.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(MaxValue, max_value);

    /// <summary>
    /// "max"–"min". If the delta field is present, the "value" field will not be present.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DeltaValue, delta_value);

internal:
    ContextualSearchGameClipStat(
        _In_ xbox::services::contextual_search::contextual_search_game_clip_stat cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_game_clip_stat m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
