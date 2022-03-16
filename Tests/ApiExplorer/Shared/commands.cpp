// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "utils.h"
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "api_explorer.h"
#include "pal.h"
#include "runner.h"
#endif
#include "cpprest/json.h"
#include "mem_hook.h"

#include <iostream>
#include <string>


#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
#define E_NOT_VALID_STATE E_FAIL
#endif

std::shared_ptr<ApiCommand> GetApiFromCmdLine(const std::vector<std::string>& cmdLineTokens, std::string apiName, bool log);
void SetupLua();
void CleanupLua();
void RegisterLuaAPIs();
void SetupCommands();
void ReplayCommands(const std::vector<std::string>& lineLogs);
void ApiRunnerProcessCmdLine(const std::string& cmdLine);
void Log(_Printf_format_string_ char const* format, ...);
void LogInit();
HRESULT CallLuaFunctionWithStringArgs(std::string fnName, std::vector<std::string> strs);
HRESULT RunSetupScript();
#if API_EXPLORER_EDITOR
void StoreCmdLineInLog(std::string cmdLine, const std::vector<std::string>& tokens);
void ClearScreen();
#endif
void MPMStopDoWorkHelper();

std::unique_ptr<ApiExplorerData> g_data;

void InitApiExplorerData()
{
    g_data = std::make_unique<ApiExplorerData>();
    g_data->m_runTestsHR = S_OK;
    g_data->m_runningTests = false;
    g_data->m_checkHR = true;
    g_data->m_lastError = S_OK;
    g_data->m_callUpdate = false;

    if (g_data->m_testsPath.empty())
    {
#if HC_PLATFORM != HC_PLATFORM_ANDROID
        auto rootFile = pal::FindFile("tests/tests.root");
#else
        auto rootFile = pal::FindFile("tests.root");
#endif
        assert(!rootFile.empty());
        g_data->m_testsPath = rootFile.substr(0, rootFile.find_last_of("/"));

    }
}

ApiExplorerData* Data()
{
    return g_data.get();
}

void ReplayCommands(const std::vector<std::string>& lineLogs)
{
    for (auto& cmd : lineLogs)
    {
        Log(">> %s", cmd.c_str());
        ApiRunnerProcessCmdLine(cmd);
    }
}

void ApiRunnerProcessCmdLine(const std::string& cmdLine)
{
    std::vector<std::string> tokens = TokenizeString(cmdLine);
    if (tokens.size() == 0)
    {
        return;
    }

#if API_EXPLORER_EDITOR
    StoreCmdLineInLog(cmdLine, tokens);
#endif

    for (auto& cmd : Data()->m_commands)
    {
        if (cmd.name == tokens[0])
        {
            cmd.cmdCallback(tokens);
            return;
        }
    }
}

#if !API_EXPLORER_EDITOR
void Log(_Printf_format_string_ char const* format, ...)
{
    char message[4096] = {};    

    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 4096, format, varArgs);
    va_end(varArgs);

    LogToScreen(message);
    LogToFile(message);
}
#endif

static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
         {
             return !std::isspace(ch);
         }).base(), s.end());
}

std::vector<std::string> TokenizeString(const std::string& input)
{
    std::vector<std::string> tokens;
    const char seperators[32] = " \r\n";
    char* tok;
    char *next_token = NULL;
    char szInput[1024] = { 0 };
    pal::strcpy(szInput, 1024, input.c_str());
    
    if (strchr(szInput, '\"') != nullptr)
    {
        tok = pal::strtok(szInput, "\"", &next_token);
        while (tok != nullptr)
        {
            std::string strTok = tok;
            rtrim(strTok);
            tokens.push_back(strTok);
            tok = pal::strtok(0, "\"", &next_token);
        }
    }
    else
    {
        tok = pal::strtok(szInput, seperators, &next_token);
        while (tok != nullptr)
        {
            tokens.push_back(tok);
            tok = pal::strtok(0, seperators, &next_token);
        }
    }
    
    return tokens;
}

#if API_EXPLORER_EDITOR
void OnCmdHelp(const std::vector<std::string>& cmdLineTokens)
{
    UNREFERENCED_PARAMETER(cmdLineTokens);
    for (auto& cmd : Data()->m_commands)
    {
        if (!cmd.hideFromHelp) Log("  %s", cmd.name);
    }
}
#endif

void OnCmdQuit(const std::vector<std::string>&)
{
    Data()->m_quit = true;
}

void MultiDeviceInit()
{
    Data()->m_multiDeviceManager->Init(
        [](const std::string& key, const std::string& value) // onStateChangedHandler
        {
            UNREFERENCED_PARAMETER(key);
            UNREFERENCED_PARAMETER(value);
            // note: don't call into LUA as we're not in LUA file processing thread
        });
}

void OnCmdHost(const std::vector<std::string>& cmdLineTokens)
{
    if (cmdLineTokens.size() < 2)
    {
        LogToScreen("Try: host name.  eg host youralias");
        return;
    }

    SetupLua();
    RunSetupScript();
    CleanupLua();

    uint64_t xuid = Data()->xboxUserId;
    char xuidStr[128];
    sprintf_s(xuidStr, "%llu", static_cast<unsigned long long>(xuid));
    std::string gamertag = Data()->gamertag;

    LogToScreen("Local XUID: %s GamerTag: %s", xuidStr, gamertag.c_str());
    MultiDeviceInit();
    bool hostedSession = false;
    std::string name = cmdLineTokens[1];

    Data()->m_multiDeviceManager->HostSession(
        name,
        xuidStr,
        gamertag,
        [&hostedSession](HRESULT hr)
        {
            if (SUCCEEDED(hr))
            {
                hr = Data()->m_multiDeviceManager->WaitTillPeerConnects();
                if (SUCCEEDED(hr))
                {
                    hostedSession = true;
                }
            }
        });

    while (!hostedSession)
    {
        pal::Sleep(100);
    }
}

void OnCmdMemTrack(const std::vector<std::string>& cmdLineTokens)
{
    if (cmdLineTokens.size() < 2)
    {
        LogToScreen("Try: memtrack bool.  eg memtrack true");
        return;
    }

    std::string enabledStr = cmdLineTokens[1];
    bool enabled = (enabledStr == "true");
    Data()->m_trackUnhookedMemory = enabled;
    LogToScreen("TrackUnhookedMemory: %d", enabled);
}

void OnCmdJoin(const std::vector<std::string>& cmdLineTokens)
{
    if (cmdLineTokens.size() < 2)
    {
        LogToScreen("Try: join name.  eg join youralias");
        return;
    }

    SetupLua();
    RunSetupScript();
    CleanupLua();

    uint64_t xuid = Data()->xboxUserId;
    char xuidStr[128];
    sprintf_s(xuidStr, "%llu", static_cast<unsigned long long>(xuid));
    std::string gamertag = Data()->gamertag;

    LogToScreen("Local XUID: %s GamerTag: %s", xuidStr, gamertag.c_str());
    MultiDeviceInit();
    bool joinedSession = false;
    std::string name = cmdLineTokens[1];

    Data()->m_multiDeviceManager->JoinOpenSession(
        name,
        xuidStr,
        gamertag,
        [&joinedSession](HRESULT hr)
        {
            if (SUCCEEDED(hr))
            {
                joinedSession = true;
            }
        });

    while (!joinedSession)
    {
        pal::Sleep(100);
    }
}

bool LoadFile(const std::string& fileNameInput)
{
    std::string strFilePath = pal::FindFile(fileNameInput);
    if (strFilePath.empty())
    {
        std::string testPath = "Tests\\" + fileNameInput;
        strFilePath = pal::FindFile(testPath);
    }

    if (!strFilePath.empty())
    {
        std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
        if (Data()->L == nullptr)
        {
            return false;
        }

        int result = luaL_dofile(Data()->L, strFilePath.c_str()); // load the script
        if (result == LUA_OK)
        {
            return true;
        }
        else
        {
            const char * error = lua_tostring(Data()->L, -1);
            LogToScreen("Couldn't load %s, error = %s", fileNameInput.c_str(), error);
        }
    }
    else
    {
        LogToFile("Couldn't find %s", fileNameInput.c_str());
    }

    return false;
}

HRESULT WaitForTestResult()
{
    while (!Data()->m_stopTest)
    {
        if(Data()->m_callUpdate)
        {
            CallLuaFunction("update");
        }
        pal::Sleep(10);
    }

    Data()->m_callUpdate = false;
    return Data()->m_testHR;
}

void WaitForXalCleanup()
{
    while (Data()->m_isXalInitialized)
    {
        pal::Sleep(10);
    }
}

void APIRunner_CleanupLeakCheck()
{
    auto memHook = GetApiRunnerMemHook();
    memHook->LogStats("CleanupLeakCheck");
    memHook->LogLeaks();
}

HRESULT RunTestWithoutCleanup(const std::string& scriptName)
{
    Data()->m_stopTest = false;
    assert(!scriptName.empty());

    bool testLoaded = LoadFile(scriptName);
    if (!testLoaded)
    {
        std::string scriptFailure = "Failed to load " + scriptName;
        Log(scriptFailure.c_str());
        return E_FAIL;
    }

    return WaitForTestResult();
}

HRESULT RunTestInternal(std::string testName, bool overrideSkip)
{
    if (!testName.empty())
    {
        Data()->m_stopTest = false;
        Data()->m_testHR = S_OK;
        Data()->m_ignoreHRs.clear();
        Data()->m_onXalTryAddFirstUserSilentlyAsync = "";
        Data()->m_onTaskQueueTerminateWithAsyncWait = "";

        if (overrideSkip)
        {
            // Tell test harness not to skip test if because it was directly run
            CallLuaString("api = require 'u-test'; api.skipOverride = true;");
        }
        else
        {
            CallLuaString("api = require 'u-test'; api.skipOverride = false;");
        }

        RunTestWithoutCleanup(testName);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
    CallLuaString("common = require 'common'; common.cleanup()");
    WaitForXalCleanup();

    return S_OK;
}

HRESULT ApiRunnerRunTest(std::string testName)
{
    return RunTestInternal(testName, true);
}

HRESULT RunSetupScript()
{
    char message[4096] = {};
#if HC_PLATFORM != HC_PLATFORM_ANDROID
    std::string sharedFolder = "_luasetup_\\xal";
    sprintf_s(message, "tests\\%s\\setup.lua", sharedFolder.c_str());
#else
    std::string sharedFolder = "xal";
    sprintf_s(message, "%s/setup.lua", sharedFolder.c_str());
#endif
    std::string strFilePath = pal::FindFile(message);
    if (strFilePath.empty() || strFilePath.length() < 5)
    {
        Log("Can't find setup.lua");
        return E_NOINTERFACE;
    }

    return RunTestInternal(message, true);
}

void OnCmdRepeatRunTest(const std::vector<std::string>& cmdLineTokens)
{
    if (cmdLineTokens.size() < 2)
    {
        std::string jsonFileContents = ApiRunnerReadFile("cmds.json");
        ApiRunnerProcessJsonCmds(jsonFileContents);
    }
    else
    {
        ApiRunnerRunTestWithSetup(cmdLineTokens[1], true);
    }
}

void OnCmdRunTest(const std::vector<std::string>& cmdLineTokens)
{
    if (cmdLineTokens.size() < 2)
    {
        LogToScreen("Try: runtest testName");
    }
    else
    {
        ApiRunnerRunTestWithSetup(cmdLineTokens[1], false);
    }
}

void EnableTestSet(TestSet set)
{
    if (set == MultiDevice)
    {
        CallLuaString("api = require 'u-test'; api.enablebvts = false; api.enablemultidevice = true;");
    }
    else if (set == SingleDeviceBVTs)
    {
        CallLuaString("api = require 'u-test'; api.enablebvts = true; api.enablemultidevice = false;");
    }
    else
    {
        CallLuaString("api = require 'u-test'; api.enablebvts = false; api.enablemultidevice = false;");
    }
}

void OnCmdFaultInjection(const std::vector<std::string>& cmdLineTokens)
{
    assert(cmdLineTokens.size() >= 2);
    if (pal::stricmp(cmdLineTokens[1].c_str(), "user") == 0)
    {
        XblEnableFaultInjection(INJECTION_FEATURE_USER);
    }
    if (pal::stricmp(cmdLineTokens[1].c_str(), "http") == 0)
    {
        XblEnableFaultInjection(INJECTION_FEATURE_HTTP);
    }
    if (pal::stricmp(cmdLineTokens[1].c_str(), "options") == 0)
    {
        assert(cmdLineTokens.size() == 5);
        int64_t failFreq = static_cast<int64_t>(atoi(cmdLineTokens[2].c_str()));
        uint64_t freqChangeSpeed = static_cast<uint64_t>(atoi(cmdLineTokens[3].c_str()));
        int64_t freqChangeAmount = static_cast<int64_t>(atoi(cmdLineTokens[4].c_str()));
        XblSetFaultInjectOptions(failFreq, freqChangeSpeed, freqChangeAmount);
    }
}

void OnCmdRunScript(const std::vector<std::string>& cmdLineTokens)
{
    assert(cmdLineTokens.size() == 2);
    SetupLua();
    RunTestWithoutCleanup(cmdLineTokens[1]);
    CleanupLua();
}

bool DoesTestContainsSetMaker(std::string test, TestSet set)
{
    std::string strFilePath = pal::FindFile(test);
    if (strFilePath.empty())
    {
        std::string testPath = "Tests\\" + test;
        strFilePath = pal::FindFile(testPath);
    }
    std::string fileContents = ApiRunnerReadFile(strFilePath);

    std::string marker;
    bool avoid = false;
    switch (set)
    {
        case MultiDevice: marker = "test.ismultidevice"; break;
        case SingleDevice: marker = "test.ismultidevice"; avoid = true; break;
        case SingleDeviceBVTs: marker = "test.isbvt"; break;
    }

    bool found = false;
    if (fileContents.find(marker) != std::string::npos)
    {
        found = true;
    }

    if (avoid) found = !found;
    return found;
}

HRESULT RunTestsHelper(TestSet set)
{
    SetupLua();

    LogToScreen("Signing in silent if possible");
    if (Data()->m_trackUnhookedMemory)
    {
        auto memHook = GetApiRunnerMemHook();
        memHook->StartMemTracking();
    }

    HRESULT hr = RunSetupScript();
    if (FAILED(hr))
    {
        LogToScreen("RunSetupScript() failed with HR = %s", ConvertHR(hr).c_str());
        return hr;
    }
    if (!Data()->gotXalUser)
    {
        LogToScreen("Data()->GotXalUser was invalid");
        return E_NOT_VALID_STATE;
    }

    Data()->m_runTestsHR = S_OK;
    Data()->m_runningTests = true;

    LogToScreen("Running tests in %s", Data()->m_testsPath.c_str());
    LogToScreen("See debug output & log file for detail");
    LogToScreen(" ");

    EnableTestSet(set);
    std::vector<std::string> tests = pal::EnumFilesInFolder(Data()->m_testsPath, "*.lua");

    int iFileNumber = 0;
    for (auto& test : tests)
    {
        if (Data()->m_quit)
            return S_OK;

        if (test.find("u-test.lua") != std::string::npos)
            continue;

        iFileNumber++;
        if (Data()->m_onlyFileNumber > 0) 
        {
            // Skip every file except the one at a specific index
            if (iFileNumber != Data()->m_onlyFileNumber)
                continue;
        }

        if (Data()->m_minFileNumber > 0 || Data()->m_maxFileNumber > 0)
        {
            // Skip files outside range
            if (iFileNumber < Data()->m_minFileNumber ||
                iFileNumber > Data()->m_maxFileNumber)
            {
                continue;
            }
        }

        std::string testName = test;
        std::size_t found = testName.find_last_of('\\');
        if (found == std::string::npos)
        {
            found = testName.find_last_of('/');
        }
        if (found != std::string::npos)
        {
            testName = testName.substr(found + 1);
        }

        if (!DoesTestContainsSetMaker(test, set))
        {
            continue;
        }

        LogToFile("***************************************");
        LogToScreen("[FILE] Processing %s.  File #%d", testName.c_str(), iFileNumber);
        LogToFile("***************************************");
        
        hr = RunTestInternal(test, false);
        if (FAILED(hr))
        {
            LogToScreen("FAILED: hr=%s (0x%0.8x)", ConvertHR(hr).c_str(), hr);
            return hr;
        }

#if HC_PLATFORM != HC_PLATFORM_GDK
        if (!Data()->m_wasTestSkipped)
        {
            pal::Sleep(6000); // For in-proc SDK (Win32/Mobile, etc.), need to delay between each test otherwise you'll get 429 from https://title.mgt.xboxlive.com/titles/current/endpoints which only allows 50 requests per 300 seconds
        }
#endif
        Data()->m_wasTestSkipped = false;
    }

    if (Data()->m_trackUnhookedMemory)
    {
        auto memHook = GetApiRunnerMemHook();
        memHook->LogUnhookedStats();
        APIRunner_CleanupLeakCheck();
    }

    CallLuaString("test.summary()");
    CleanupLua();

    Data()->m_runningTests = false;

    return S_OK;
}

HRESULT WaitTillDone()
{
    Data()->m_quit = true;
    while (
        Data()->m_runningTests ||
        !Data()->m_socialDoWorkDone ||
        !Data()->m_mpmDoWorkDone)
    {
        pal::Sleep(50);
    }

    return S_OK;
}

#if HC_PLATFORM_IS_MICROSOFT
typedef wchar_t char_t;
typedef std::wstring string_t;
typedef std::wstringstream stringstream_t;
#else
typedef char char_t;
typedef std::string string_t;
typedef std::stringstream stringstream_t;
#endif

static int CharTFromUft8(
    _In_z_ const char* inArray,
    _Out_writes_z_(cchOutArray) char_t* outArray,
    _In_ int cchOutArray
)
{
#if HC_PLATFORM_IS_MICROSOFT
    // query for the buffer size
    auto queryResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        inArray, -1,
        nullptr, 0
    );

    if (queryResult > cchOutArray && cchOutArray == 0)
    {
        return queryResult;
    }
    else if (queryResult == 0 || queryResult > cchOutArray)
    {
        throw std::exception("char_t_from_utf8 failed");
    }

    auto conversionResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        inArray, -1,
        outArray, cchOutArray
    );
    if (conversionResult == 0)
    {
        throw std::exception("char_t_from_utf8 failed");
    }

    return conversionResult;
#else
    int len = (int)strlen(inArray);
    if (len < cchOutArray && outArray != nullptr)
    {
        strlcpy(outArray, inArray, len + 1);
    }
    else if (cchOutArray > 0)
    {
        return 0;
    }
    return len + 1;
#endif
}

static string_t StringTFromUtf8(_In_z_ const char* utf8)
{
#if HC_PLATFORM_IS_MICROSOFT
    uint64_t cchOutString = static_cast<uint64_t>(CharTFromUft8(utf8, nullptr, 0));
    string_t out(static_cast<unsigned int>(cchOutString - 1), '\0');
    CharTFromUft8(utf8, &out[0], static_cast<int>(cchOutString));
    return out;
#else
    return string_t(utf8);
#endif
}


static int Utf8FromCharT(
    _In_z_ const char_t* inArray,
    _Out_writes_z_(cchOutArray) char* outArray,
    _In_ int cchOutArray
)
{
#if HC_PLATFORM_IS_MICROSOFT
    // query for the buffer size
    auto queryResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        inArray, -1,
        nullptr, 0,
        nullptr, nullptr
    );

    if (queryResult > cchOutArray && cchOutArray == 0)
    {
        return queryResult;
    }
    else if (queryResult == 0 || queryResult > cchOutArray)
    {
        throw std::exception("utf8_from_char_t failed");
    }

    auto conversionResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        inArray, -1,
        outArray, cchOutArray,
        nullptr, nullptr
    );
    if (conversionResult == 0)
    {
        throw std::exception("utf8_from_char_t failed");
    }

    return conversionResult;
#else
    int len = (int)strlen(inArray);
    if (len < cchOutArray && outArray != nullptr)
    {
        strlcpy(outArray, inArray, len + 1);
    }
    else if (cchOutArray > 0)
    {
        return 0;
    }
    return len + 1;
#endif
}

#if HC_PLATFORM != HC_PLATFORM_IOS
web::json::value extract_json_field(
    _In_ const web::json::value& json,
    _In_ const string_t& name)
{
    if (json.is_object())
    {
        auto& jsonObj = json.as_object();
        auto it = jsonObj.find(name);
        if (it != jsonObj.end())
        {
            return it->second;
        }
    }

    return web::json::value::null();
}

web::json::array extract_json_array(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& arrayName
)
{
    web::json::value field(extract_json_field(jsonValue, arrayName));
    if ((!field.is_array()) || field.is_null()) { return web::json::value::array().as_array(); }
    return field.as_array();
}
#endif

std::string ApiRunnerReadFile(std::string fileName)
{
    assert(Data() != nullptr); // call ApiRunnerSetupApiExplorer() first

#if HC_PLATFORM == HC_PLATFORM_ANDROID
    // In Android, the test files are considered assets.
    // These files are copied from the assets folder into the external storage
    // when the app is installed and launched on a device.
    // Searching for the file is then done using the filename and doesn't use paths like other platforms
    std::string fileNameEdited = fileName.substr(0, fileName.find_last_of("\\"));
    std::string strFilePath = pal::FindFile(fileNameEdited);
#else
    std::string strFilePath = pal::FindFile(fileName);
#endif
    if (strFilePath.empty())
    {
        std::string testPath = "Tests\\" + fileName;
        strFilePath = pal::FindFile(testPath);
    }

    if (strFilePath.empty())
    {
        return "";
    }

    std::ifstream fileStream(strFilePath);
    std::string str;

    fileStream.seekg(0, std::ios::end);
    str.reserve(static_cast<unsigned int>(fileStream.tellg()));
    fileStream.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return str;
}

HRESULT ApiRunnerProcessJsonCmds(std::string json)
{
#if HC_PLATFORM == HC_PLATFORM_IOS
    ApiRunnerProcessCmdLine("runtests");
#else
    assert(Data() != nullptr); // call ApiRunnerSetupApiExplorer() first

    web::json::value jsonCmds = web::json::value::parse(StringTFromUtf8(json.c_str()));
    auto commands = extract_json_array(jsonCmds, _T("commands"));

    do
    {
        for (const auto& command : commands)
        {
            char cmdStr[1024] = { 0 };
            Utf8FromCharT(command.as_string().data(), cmdStr, sizeof(cmdStr));
            ApiRunnerProcessCmdLine(std::string(cmdStr));
        }
    } while (Data()->m_repeatJsonCmds);
#endif
    return S_OK;
}

void ApiRunnerSetRunTestsParams(int onlyFileNumber, int minFileNumber, int maxFileNumber)
{
    Data()->m_onlyFileNumber = onlyFileNumber;
    Data()->m_minFileNumber = minFileNumber;
    Data()->m_maxFileNumber = maxFileNumber;
}

HRESULT ApiRunnerRunTests(TestSet set)
{
    assert(Data() != nullptr); // call ApiRunnerSetupApiExplorer() first

    if (Data()->m_runningTests)
    {
        LogToFile("Tests in progress");
        return S_OK;
    }

    HRESULT hr = RunTestsHelper(set);
    Data()->m_runTestsHR = hr;
    Data()->m_runningTests = false;
    
    return hr;
}

void OnCmdMultiDeviceTests(const std::vector<std::string>&)
{
    ApiRunnerRunTests(TestSet::MultiDevice);
}

void OnCmdRunBvts(const std::vector<std::string>&)
{
    ApiRunnerRunTests(TestSet::SingleDeviceBVTs);
}

void OnCmdRunTests(const std::vector<std::string>&)
{
    ApiRunnerRunTests(TestSet::SingleDevice);
}

HRESULT ApiRunnerRunTestWithSetup(std::string testName, bool repeat)
{
    if (Data()->m_runningTests)
    {
        LogToFile("Tests in progress");
        return E_UNEXPECTED;
    }


    Data()->m_runningTests = true;
    SetupLua();
    if (Data()->m_trackUnhookedMemory)
    {
        auto memHook = GetApiRunnerMemHook();
        memHook->StartMemTracking();
    }
    HRESULT hr = RunSetupScript();
    if (SUCCEEDED(hr))
    {
        auto testFullPath = pal::FindFile(testName);
        if (testFullPath.empty())
        {
            LogToScreen("Can't find %s", testName.c_str());
        }
        if (repeat)
        {
            while (true)
            {
                hr = RunTestInternal(testName, true);
                if (FAILED(hr))
                {
                    break;
                }
            }
        }
        else
        {
            hr = RunTestInternal(testName, true);
        }
    }

    if (Data()->m_trackUnhookedMemory)
    {
        auto memHook = GetApiRunnerMemHook();
        memHook->LogUnhookedStats();
        APIRunner_CleanupLeakCheck();
    }

    CleanupLua();
    Data()->m_runningTests = false;

    return hr;
}

void OnCmdClear(const std::vector<std::string>&)
{
#if API_EXPLORER_EDITOR
    ClearScreen();
#endif
}

void OnCmdLoop(const std::vector<std::string>&)
{
    Data()->m_repeatJsonCmds = true;
}

bool IsFailHr(HRESULT hr)
{
    if (SUCCEEDED(hr))
    {
        return false;
    }

    for (auto hrIgnore : Data()->m_ignoreHRs)
    {
        if (hrIgnore == hr)
        {
            return false;
        }
    }

    return true;
}

void OutputDebugStackTrace(std::vector<std::string> stackTrace)
{   
    LogToScreen("----------- BEGIN STACK TRACE -----------");
    for (std::string stack_string : stackTrace)
    {
        LogToScreen(stack_string.c_str());
    }
    LogToScreen("----------- END STACK TRACE -----------");
}

void LuaStopTestIfFailed(HRESULT hr)
{
    Data()->m_lastError = hr;
    if (FAILED(hr) && Data()->m_checkHR)
    {
        char text[1024];

        auto memHook = GetApiRunnerMemHook();
        std::vector<std::string> stackTrace = memHook->GetStackLogLine();
        OutputDebugStackTrace(stackTrace);

        sprintf_s(text, "local hr = 0x%0.8x; test.equal_no_log(hr, 0); test.stopTest();", hr);
        std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
        if (Data()->L != nullptr)
        {
            luaL_dostring(Data()->L, text);

            StopTestFile_Lua(Data()->L);
        }
    }
}

int LuaReturnHR(lua_State *L, HRESULT hr, int extraParams)
{
    LuaStopTestIfFailed(hr);
    lua_pushnumber(L, hr);
    return 1 + extraParams;
}

HRESULT CallLuaStringWithDefault(std::string customFn, std::string defaultFn)
{
    Log(customFn.c_str());
    if (!customFn.empty()) 
    { 
        return CallLuaString(customFn);
    } 
    else 
    { 
        return CallLuaString(defaultFn);
    }
}


HRESULT CallLuaString(std::string str)
{
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    if (Data()->L != nullptr)
    {
        luaL_dostring(Data()->L, str.c_str());
    }
    return S_OK;
}

HRESULT CallLuaFunctionWithHr(HRESULT hr, std::string fnName)
{
    LuaStopTestIfFailed(hr);
    if (FAILED(hr) && Data()->m_checkHR)
    {
        return hr;
    }

    return CallLuaFunction(fnName);
}

HRESULT CallLuaFunction(std::string fnName)
{
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    if (Data()->L == nullptr)
    {
        return E_NOINTERFACE;
    }

    lua_getglobal(Data()->L, fnName.c_str()); // get function 
    if (lua_isfunction(Data()->L, lua_gettop(Data()->L)))
    {
        lua_call(Data()->L, 0, 0); // call the function with 0 arguments, return 0 results
    }
    else
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CallLuaFunctionWithStringArgs(std::string fnName, std::vector<std::string> strs)
{
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    if (Data()->L == nullptr)
    {
        return E_NOINTERFACE;
    }

    lua_getglobal(Data()->L, fnName.c_str()); // get function 
    if (lua_isfunction(Data()->L, lua_gettop(Data()->L)))
    {
        for (const auto& s : strs)
        {
            lua_pushstring(Data()->L, s.c_str());
        }
        lua_call(Data()->L, static_cast<int>(strs.size()), 0);
    }
    else
    {
        return E_FAIL;
    }

    return S_OK;
}

void ApiRunnerSetupApiExplorerTestsPath(std::string testsPath)
{
    Data()->m_testsPath = testsPath;
}

void CleanupLua()
{
    MPMStopDoWorkHelper();
    if (Data()->m_multiDeviceManager)
    {
        Data()->m_multiDeviceManager->StopSessionStateChangePolling();
    }

    {
        std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
        lua_close(Data()->L); // cleanup Lua
        Data()->L = nullptr;
    }
}

void SetupLua()
{
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    Data()->L = luaL_newstate();
    assert(Data()->L != nullptr);
    luaL_openlibs(Data()->L); // load Lua base libraries
    
    // Setup paths
    std::string luaPath = pal::GetLuaPath();
    
    lua_getglobal(Data()->L, "package");
    lua_getfield(Data()->L, -1, "path"); // get field "path" from table at top of stack (-1)
    std::string cur_path = lua_tostring(Data()->L, -1); // grab path string from top of stack
    cur_path.append(";");
    cur_path.append(luaPath);
    lua_pop(Data()->L, 1); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring(Data()->L, cur_path.c_str()); // push the new one
    lua_setfield(Data()->L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
    lua_pop(Data()->L, 1); // get rid of package table from top of stack
    
    RegisterLuaAPIs();
}

void ApiRunnerSetupApiExplorer()
{
    LogInit();
    InitApiExplorerData();
    SetupCommands();

    Data()->m_multiDeviceManager = std::make_shared<ApiRunnerMultiDeviceManager>();
}

void SetupCommands()
{
#if API_EXPLORER_EDITOR
    Data()->m_commands.push_back({ "help", OnCmdHelp, true });
    Data()->m_commands.push_back({ "?", OnCmdHelp, true });
#endif
    Data()->m_commands.push_back({ "exit", OnCmdQuit, false });
    Data()->m_commands.push_back({ "quit", OnCmdQuit, true });
    Data()->m_commands.push_back({ "q", OnCmdQuit, true });
    Data()->m_commands.push_back({ "cls", OnCmdClear, true });
    Data()->m_commands.push_back({ "clear", OnCmdClear, false });
    Data()->m_commands.push_back({ "loop", OnCmdLoop, false });
      
    Data()->m_commands.push_back({ "runtest", OnCmdRunTest, true });
    Data()->m_commands.push_back({ "rt", OnCmdRunTest, false });
    Data()->m_commands.push_back({ "repeat", OnCmdRepeatRunTest, true });

    Data()->m_commands.push_back({ "runtests", OnCmdRunTests, false });
    Data()->m_commands.push_back({ "rts", OnCmdRunTests, true });
    Data()->m_commands.push_back({ "run", OnCmdRunTests, true });
    Data()->m_commands.push_back({ "runbvts", OnCmdRunBvts, false });
    Data()->m_commands.push_back({ "runbarescript", OnCmdRunScript, true });
    Data()->m_commands.push_back({ "faultinjection", OnCmdFaultInjection, true });

    Data()->m_commands.push_back({ "host", OnCmdHost, true });
    Data()->m_commands.push_back({ "join", OnCmdJoin, true });
    Data()->m_commands.push_back({ "runmultidevicetests", OnCmdMultiDeviceTests, true });
    Data()->m_commands.push_back({ "memtrack", OnCmdMemTrack, true });
}

bool ApiRunnerIsRunningTests()
{
    return !Data()->m_runningTests;
}

HRESULT ApiRunnerGetTestResult()
{
    return Data()->m_runTestsHR;
}

uint64_t GetUint64FromLua(lua_State *L, int paramNum, uint64_t defaultArg)
{
    uint64_t t = defaultArg;
    if (lua_gettop(L) >= paramNum)
    {
        t = (uint64_t)lua_tointeger(L, paramNum);
    }
    return t;
}

uint32_t GetUint32FromLua(lua_State *L, int paramNum, uint32_t defaultArg)
{
    return (uint32_t)GetUint64FromLua(L, paramNum, defaultArg);
}

bool GetBoolFromLua(lua_State *L, int paramNum, bool defaultArg)
{
    bool t = defaultArg;
    if (lua_gettop(L) >= paramNum)
    {
        t = (bool)lua_tonumber(L, paramNum);
    }
    return t;
}

std::string GetStringFromLua(lua_State *L, int paramNum, std::string defaultArg)
{
    std::string t = defaultArg;
    if (lua_gettop(L) >= paramNum)
    {
        t = lua_tostring(L, paramNum);
    }
    return t;
}

void LogCat(bool logToFile, _Printf_format_string_ char const* format, ...)
{
    char message[8000] = {};

    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 8000, format, varArgs);
    va_end(varArgs);

    std::string catMessage = message;
    {
        std::lock_guard<std::mutex> lock(Data()->m_catMessageLock);
        Data()->m_catMessage += catMessage;
        if (Data()->m_catMessage.find_first_of('\n') != std::string::npos)
        {
            std::vector<std::string> tokens;
            const char seperators[32] = "\n";
            char* tok;
            char *next_token = NULL;
            char szInput[16000] = { 0 };
            pal::strcpy(szInput, 16000, Data()->m_catMessage.c_str());

            tok = pal::strtok(szInput, seperators, &next_token);
            while (tok != nullptr)
            {
                tokens.push_back(tok);
                tok = pal::strtok(0, seperators, &next_token);
            }

            for (size_t iToken = 0; iToken < tokens.size(); iToken++)
            {
                if (logToFile)
                {
                    LogToFile(tokens[iToken].c_str());
                }
                else
                {
                    LogToScreen(tokens[iToken].c_str());
                }
            }
            Data()->m_catMessage = "";
        }
    }
}

#if HC_PLATFORM == HC_PLATFORM_IOS
void SetupAPNSRegistrationToken(std::string registrationToken)
{
    Data()->apnsToken = registrationToken;
}
#endif

#if HC_PLATFORM == HC_PLATFORM_ANDROID
void SetupAndroidContext( JavaVM *javaVM, jobject context, jclass mainActivityClass, jobject mainActivityInstance, jmethodID getApplicationContext)
{
    Data()->javaVM = javaVM;
    Data()->m_mainActivityClass = mainActivityClass;
    Data()->m_getApplicationContext = getApplicationContext;
    Data()->m_mainActivityClassInstance = mainActivityInstance;
    Data()->applicationContext = context;
    Data()->initArgs = {};
    Data()->initArgs.applicationContext = context;
    Data()->initArgs.javaVM = javaVM;
}

#endif
