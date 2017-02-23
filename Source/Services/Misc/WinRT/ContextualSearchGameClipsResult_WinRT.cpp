// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ContextualSearchGameClipsResult_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

ContextualSearchGameClipsResult::ContextualSearchGameClipsResult(
    _In_ xbox::services::contextual_search::contextual_search_game_clips_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchGameClip, contextual_search_game_clip>(m_cppObj.items());
}

Windows::Foundation::Collections::IVectorView<ContextualSearchGameClip^>^ 
ContextualSearchGameClipsResult::Items::get()
{
    return m_items->GetView();
}

IAsyncOperation<ContextualSearchGameClipsResult^>^
ContextualSearchGameClipsResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox_live_result<contextual_search_game_clips_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ContextualSearchGameClipsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
