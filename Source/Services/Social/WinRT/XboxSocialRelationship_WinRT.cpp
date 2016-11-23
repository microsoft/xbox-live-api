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
#include "SocialRelationship_WinRT.h"
#include "XboxSocialRelationship_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace xbox::services::social;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

XboxSocialRelationship::XboxSocialRelationship(
    _In_ ::xbox_social_relationship cppObj
    ) : m_cppObj(cppObj)
{
    Vector<String^>^ responseVector = ref new Vector<String^>();
    const std::vector<string_t>& cppSocialNetworks = m_cppObj.social_networks();
    for (auto& cppUserProfile : cppSocialNetworks)
    {
        responseVector->Append(ref new String(cppUserProfile.c_str()));
    }
    m_socialNetworksView = responseVector->GetView();
}

IVectorView<Platform::String^>^
XboxSocialRelationship::SocialNetworks::get()
{ 
    return m_socialNetworksView;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END