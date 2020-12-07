// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "public_utils.h"
#include "Xal/xal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<xbox_live_app_config> xbox_live_app_config::get_app_config_singleton()
{
    static std::shared_ptr<xbox_live_app_config> instance = std::shared_ptr<xbox_live_app_config>(new xbox_live_app_config());
    return instance;
}

uint32_t xbox_live_app_config::title_id() const
{
    uint32_t titleId{};
    XalGetTitleId(&titleId);
    return titleId;
}

string_t xbox_live_app_config::scid() const
{

    const char* scid{ nullptr };
    XblGetScid(&scid);
    return Utils::StringTFromUtf8(scid);
}

string_t xbox_live_app_config::environment() const
{
    // Not exposed from C APIs.
    return string_t();
}

string_t xbox_live_app_config::sandbox() const
{
    size_t sandboxSize = XalGetSandboxSize();
    string_t sandboxStr;
    char* sandbox = new (std::nothrow) char[sandboxSize];
    if (sandbox != nullptr)
    {
        if (SUCCEEDED(XalGetSandbox(sandboxSize, sandbox, nullptr)))
        {
            sandboxStr = Utils::StringTFromUtf8(sandbox);
        }
        delete[] sandbox;
    }
    return sandboxStr;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
