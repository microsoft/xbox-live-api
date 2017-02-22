// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#ifdef XSAPI_I
#include "iOSCll.h"

namespace xbox { namespace services { 

class xbox_cll : public std::enable_shared_from_this<xbox_cll>
{
public:
    static std::shared_ptr<xbox_cll> get_xbox_cll_singleton();

    xbox_cll();
    ~xbox_cll();

    std::shared_ptr<iOSCll> raw_cll();

private:
    std::shared_ptr<iOSCll> m_cll;
    static std::shared_ptr<xbox_cll> s_xboxCll;
};

}}
#endif
