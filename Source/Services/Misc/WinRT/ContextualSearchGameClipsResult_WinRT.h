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
#include "xsapi/contextual_search_service.h"
#include "ContextualSearchGameClip_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Contains information about a contextual search game clips result.</summary>
public ref class ContextualSearchGameClipsResult sealed
{
public:
    /// <summary>
    /// Returns the next page of game clips.
    /// </summary>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    Windows::Foundation::IAsyncOperation<ContextualSearchGameClipsResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Array of game clips returned.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<ContextualSearchGameClip^>^ Items { Windows::Foundation::Collections::IVectorView<ContextualSearchGameClip^>^ get(); }

internal:
    ContextualSearchGameClipsResult(
        _In_ xbox::services::contextual_search::contextual_search_game_clips_result cppObj
        );
        
private:
    xbox::services::contextual_search::contextual_search_game_clips_result m_cppObj;
    Windows::Foundation::Collections::IVector<ContextualSearchGameClip^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
