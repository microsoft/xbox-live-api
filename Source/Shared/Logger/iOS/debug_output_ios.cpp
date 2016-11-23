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
#include "debug_output.h"


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void debug_output::write(_In_ const std::string& msg)
{
    printf("%s\n", msg.c_str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
