// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

enum TestSet
{
    SingleDevice,
    MultiDevice,
    SingleDeviceBVTs
};

// Call these by runner
void ApiRunnerSetupApiExplorer();
void ApiRunnerSetupApiExplorerTestsPath(std::string testsPath);
HRESULT ApiRunnerRunTest(std::string testName);
HRESULT ApiRunnerRunTestWithSetup(std::string testName, bool repeat);
void ApiRunnerSetRunTestsParams(int onlyFileNumber, int minFileNumber, int maxFileNumber);
HRESULT ApiRunnerRunTests(TestSet set);
void ApiRunnerProcessCmdLine(const std::string& cmdLine);
std::string ApiRunnerReadFile(std::string fileName);
HRESULT ApiRunnerProcessJsonCmds(std::string json);
bool ApiRunnerIsRunningTests();
HRESULT ApiRunnerGetTestResult();

// These functions need to be implemented by each runner
void LogToFile(_Printf_format_string_ char const* format, ...);
void LogToScreen(_Printf_format_string_ char const* format, ...);

// These functions are specific to APIExplorer on iOS
#if HC_PLATFORM == HC_PLATFORM_IOS
void SetupAPNSRegistrationToken(std::string registrationToken);
#endif

// These functions are specific to APIExplorer on Android
#if HC_PLATFORM == HC_PLATFORM_ANDROID
void SetupAndroidContext( JavaVM *javaVM, jobject context, jclass mainActivityClass, jobject mainActivityInstance, jmethodID getApplicationContext);
#endif
