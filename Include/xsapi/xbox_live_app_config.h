// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <set>

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_app_config_internal;
namespace system {
    class user_impl_idp;
}

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
class xbox_live_app_config
{
public:
    /// <summary>
    /// Gets the app config singleton.
    /// </summary>
    static std::shared_ptr<xbox_live_app_config> get_app_config_singleton();

    /// <summary>
    /// Returns the title id of the app.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// Returns the service config id of the app.
    /// </summary>
    _XSAPIIMP string_t scid() const;

    /// <summary>
    /// Returns the Xbox Live environment being used, it is empty before you sign in or using production.
    /// </summary>
    _XSAPIIMP string_t environment() const;

    /// <summary>
    /// Returns the sandbox such as "XDKS.1", it is empty until you sign in.
    /// </summary>
    _XSAPIIMP string_t sandbox() const;

private:
    xbox_live_app_config(_In_ std::shared_ptr<xbox_live_app_config_internal> internalObj);
    std::shared_ptr<xbox_live_app_config_internal> m_internalObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
