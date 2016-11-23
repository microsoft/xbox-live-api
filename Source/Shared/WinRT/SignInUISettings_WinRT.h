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
#include "Macros_WinRT.h"
#include "xsapi/xbox_live_app_config.h"
#include "SignInUISettings_WinRT.h"
#include "SignInUIGameCategory_WinRT.h"
#include "SignInUIEmphasisFeature_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
public ref class SignInUISettings sealed
{
public:
    /// <summary>
    /// Current SignInUIGameCategory setting 
    /// </summary>
    property SignInUIGameCategory TitleCategory
    {
        SignInUIGameCategory get();

        void set(SignInUIGameCategory);
    }

    /// <summary>
    /// Specify which emphasized Xbox Live feature you want to show on the sign in page, up to 3.
    /// </summary>
    void AddEmphasisFeature(SignInUIEmphasisFeature feature);

    /// <summary>
    /// Retrieve the emphasis features added to show on the UI, only first 3 will be displayed.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<SignInUIEmphasisFeature>^ EmphasisFeatures
    {
        Windows::Foundation::Collections::IVectorView<SignInUIEmphasisFeature>^ get();
    }

    /// <summary>
    /// Specify the background image you want to show in the sign in page, in raw binary form.
    /// </summary>
    /// <remarks>
    /// An Image can be one of the following format: jpeg, jpg, png, bmp, gif, tiff and ico. 
    /// The input source will always be stretched to 300X400 pixels , thus the file larger than that 
    /// is not recommended.
    /// </remarks>
    void SetBackgroundImage(const Platform::Array<byte>^ image);

    /// <summary>
    /// The background color of sign in page the title has set. Example: "0F2C55".
    /// </summary>
    property Platform::String^ BackgroundHexColor
    {
        Platform::String^ get();
        void set(Platform::String^ color);
    }

internal:
    SignInUISettings(std::shared_ptr<xbox::services::xbox_live_app_config> cppObj);

private:
    std::shared_ptr<xbox::services::xbox_live_app_config> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
