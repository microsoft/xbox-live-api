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
