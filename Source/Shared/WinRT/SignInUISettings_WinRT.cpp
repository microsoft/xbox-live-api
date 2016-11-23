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
#include "SignInUISettings_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

SignInUISettings::SignInUISettings(std::shared_ptr<xbox::services::xbox_live_app_config> cppObj):
    m_cppObj(std::move(cppObj))
{}


SignInUIGameCategory 
SignInUISettings::TitleCategory::get()
{
    return static_cast<SignInUIGameCategory>(m_cppObj->app_signin_ui_settings().title_category());
}

void
SignInUISettings::TitleCategory::set(SignInUIGameCategory category)
{
    return m_cppObj->app_signin_ui_settings().set_game_category(static_cast<signin_ui_settings::game_category>(category));
}

void
SignInUISettings::AddEmphasisFeature(SignInUIEmphasisFeature feature)
{
    return m_cppObj->app_signin_ui_settings().add_emphasis_feature(static_cast<signin_ui_settings::emphasis_feature>(feature));
}

Windows::Foundation::Collections::IVectorView<SignInUIEmphasisFeature>^ 
SignInUISettings::EmphasisFeatures::get()
{
    Vector<SignInUIEmphasisFeature>^ features = ref new Vector<SignInUIEmphasisFeature>();
    for (auto featrue : m_cppObj->app_signin_ui_settings().emphasis_features())
    {
        features->Append(static_cast<SignInUIEmphasisFeature>(featrue));
    }

    return features->GetView();
}

void 
SignInUISettings::SetBackgroundImage(const Platform::Array<byte>^ image)
{
    byte* bufferBytes = image->Data;
    std::vector<unsigned char> imageBuffer(bufferBytes, bufferBytes + image->Length);
    return m_cppObj->app_signin_ui_settings().set_background_image(imageBuffer);
}

String^
SignInUISettings::BackgroundHexColor::get()
{
    return ref new String(m_cppObj->app_signin_ui_settings().background_hex_color().c_str());
}

void SignInUISettings::BackgroundHexColor::set(Platform::String^ color)
{
    m_cppObj->app_signin_ui_settings().set_background_hex_color(color->Data());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
