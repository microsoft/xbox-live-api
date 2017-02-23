// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"
#include "Utils_WinRT.h"
#include "xsapi/xbox_live_app_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
public ref class XboxLiveAppConfiguration sealed
{
public:
    /// <summary>
    /// Returns the global instance of XboxLiveAppConfiguration
    /// </summary>
    static property XboxLiveAppConfiguration^ SingletonInstance
    {
        XboxLiveAppConfiguration^ get();
    }

    /// <summary>
    /// Returns the title id of the app.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// Returns the service config id of the app.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ServiceConfigurationId, scid);

    /// <summary>
    /// Returns the Xbox Live environment being used.  This is typically blank.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(Environment, environment);

    /// <summary>
    /// Returns the sandbox such as "XDKS.1".
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(Sandbox, sandbox);

internal:
    XboxLiveAppConfiguration(
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> cppObj
        );

    std::shared_ptr<xbox::services::xbox_live_app_config> GetCppObj() { return m_cppObj; }

private:
    std::shared_ptr<xbox::services::xbox_live_app_config> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
