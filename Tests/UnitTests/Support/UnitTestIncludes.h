// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#ifdef USING_TAEF
#include "TAEF/UnitTestIncludes_TAEF.h"
#else
#include "TE/UnitTestIncludes_TE.h"
#include "DefineTestMacros.h"
#endif

#include "xbox_live_context_internal.h"
#include "event.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-cpp/xbox_live_context.h"
#include "mock_rta_service.h"
#include "http_mock.h"
#include "mock_web_socket.h"
#include "unit_test_helpers.h"
#include "perf_tester.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
