// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "UnitTestBase.h"
#include "DefineTestMacros.h"
#include "WexTestClass.h"

#define VERIFY_INVALIDARG(x) \
    VERIFY_ARE_EQUAL(E_INVALIDARG, x);