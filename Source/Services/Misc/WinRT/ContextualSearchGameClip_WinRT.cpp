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
#include "ContextualSearchGameClip_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

ContextualSearchGameClip::ContextualSearchGameClip(
    _In_ xbox::services::contextual_search::contextual_search_game_clip cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_thumbnails = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchGameClipThumbnail, contextual_search_game_clip_thumbnail>(m_cppObj.thumbnails());
    m_uriInfos = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchGameClipUriInfo, contextual_search_game_clip_uri_info>(m_cppObj.game_clip_uris());
    m_stats = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ContextualSearchGameClipStat, contextual_search_game_clip_stat>(m_cppObj.stats());
}

Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipUriInfo^>^
ContextualSearchGameClip::GameClipUris::get()
{
    return m_uriInfos->GetView();
}

Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipThumbnail^>^
ContextualSearchGameClip::Thumbnails::get()
{
    return m_thumbnails->GetView();
}

Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipStat^>^
ContextualSearchGameClip::Stats::get()
{
    return m_stats->GetView();
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
