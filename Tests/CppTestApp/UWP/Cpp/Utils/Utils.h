//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once

#include "pch.h"
#include <stdio.h>
#include "collection.h"


class Utils
{
public:
    static Platform::String^ Utils::FormatString( LPCWSTR strMsg, ... );

    static Platform::String^ ConvertHResultToErrorName( HRESULT hr );
    static Platform::String^ GetErrorString( HRESULT hr );

    static bool IsStringEqualCaseInsenstive( Platform::String^ xboxUserId1, Platform::String^ xboxUserId2 );
};