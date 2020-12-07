//--------------------------------------------------------------------------------------
// File: ATGTelemetry.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright(c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------
#include "pch.h"

#ifdef ATG_ENABLE_TELEMETRY
#include "ATGTelemetry.h"

#include <XGame.h>
#include <XSystem.h>

#include "httpClient/httpClient.h"
#include "StringUtil.h"
#include "Json.h"
#include <sstream>

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#include <fstream>
#include "Shlwapi.h"
#include "Shlobj.h"
#endif

#ifdef _GAMING_XBOX
#pragma warning(disable : 4365)
#include <ws2def.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#endif

namespace {
    const char *LOGIN_URL = "https://3436.playfabapi.com/Client/LoginWithCustomID";
    const char *EVENT_URL = "https://3436.playfabapi.com/Event/WriteTelemetryEvents";
}

using namespace ATG;

class ATGTelemetry
{
public:
    ~ATGTelemetry()
    {
#if _GAMING_XBOX
        if (m_notificationChangedHandle)
        {
            CancelMibChangeNotify2(m_notificationChangedHandle);
            m_notificationChangedHandle = nullptr;
        }
#endif
    }

    void SendTelemetry()
    {
#if _GAMING_XBOX
        NotifyNetworkConnectivityHintChange([](void* context, NL_NETWORK_CONNECTIVITY_HINT connectivityHint)
            {
                auto atgTelemetry = static_cast<ATGTelemetry*>(context);
                if (connectivityHint.ConnectivityLevel == NL_NETWORK_CONNECTIVITY_LEVEL_HINT::NetworkConnectivityLevelHintInternetAccess
                    || connectivityHint.ConnectivityLevel == NL_NETWORK_CONNECTIVITY_LEVEL_HINT::NetworkConnectivityLevelHintConstrainedInternetAccess)
                {
                    atgTelemetry->SendTelemetryInternal();
                }
            }, this, true, &m_notificationChangedHandle);
#else
        SendTelemetryInternal();
#endif
    }

private:
    void SendTelemetryInternal()
    {
        static bool sent = false;

        if (!sent)
        {
            sent = true;
            // Ensure the HTTP library is initialized
            if (FAILED(HCInitialize(nullptr)))
            {
                return;
            }

            auto *async = new XAsyncBlock{};
            async->queue = nullptr;

            CollectBaseTelemetry();
            UploadTelemetry(async);
        }
    }

    std::string NewGuid()
    {
        GUID id = {};
        char buf[64] = {};

        CoCreateGuid(&id);

        sprintf_s(buf, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
            id.Data1, id.Data2, id.Data3,
            id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
            id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

        return std::string(buf);
    }

    std::string GetTelemetryId()
    {
        using namespace std;
        const int BUFSIZE = 64;
        char buf[BUFSIZE] = {};

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
        size_t ignoreField;
        XSystemGetConsoleId(XSystemConsoleIdBytes, buf, &ignoreField);

        std::string clientId = buf;
        clientId.erase(std::remove(clientId.begin(), clientId.end(), '.'), clientId.end());
        return clientId;
#else
        wchar_t *folderPath;
        string telemetryId;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_NO_PACKAGE_REDIRECTION, nullptr, &folderPath)))
        {

            // Path information
            wstring path(folderPath);
            path += L"\\Microsoft\\ATGSamples\\";
            wstring fullPath = path + L"telemetry.txt";

            bool createFile = CreateDirectoryW(path.c_str(), nullptr);

            if (!createFile)
            {
                DWORD error = GetLastError();

                // Sanity checking - Directory failed to create but that should mean it already exists
                if (error != ERROR_ALREADY_EXISTS)
                {
                    return NewGuid();
                }
            }

            int attempts = 0;
            while (attempts++ < 2)
            {
                if (createFile)
                {
                    ofstream telFile;
                    telFile.open(fullPath, ios::out | ios::trunc);
                    telemetryId = NewGuid();

                    telFile << telemetryId;
                    telFile.close();
                    break;
                }
                else
                {
                    ifstream telFile;
                    telFile.open(fullPath);
                    if (telFile.good())
                    {
                        telFile.get(buf, BUFSIZE);
                        telFile.close();
                        telemetryId = buf;

                        if (telemetryId.size() >= 32)
                        {
                            break;
                        }
                    }

                    createFile = true;
                }
            }
        }

        return telemetryId;
#endif
    }

    // Collect the base telemetry for the system
    void CollectBaseTelemetry()
    {
        const int BUFSIZE = 256;
        size_t ignore;
        char buf[BUFSIZE] = {};

        m_telemetry = std::make_unique<TelemetryData>();

        TelemetryData &data = *m_telemetry;

        // OS and client information
        {
            auto info = XSystemGetAnalyticsInfo();

            data.osVersion = XVersionToString(info.osVersion);
            data.hostingOsVersion = XVersionToString(info.hostingOsVersion);
            data.family = info.family;
            data.form = info.form;

            data.clientId = GetTelemetryId();

            data.sandbox.reserve(XSystemXboxLiveSandboxIdMaxBytes);
            XSystemGetXboxLiveSandboxId(XSystemXboxLiveSandboxIdMaxBytes, buf, &ignore);
            data.sandbox = buf;
        }

        // Sample information
        {
            uint32_t titleId = 0;
            XGameGetXboxTitleId(&titleId);
            data.titleId = ToHexString(titleId);

            wchar_t exePath[MAX_PATH + 1] = {};

            if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH))
            {
                data.exeName = "Unknown";
            }
            else
            {
                auto path = std::wstring(exePath);
                auto off = path.find_last_of(L"\\");
                auto exeOnly = path.substr(off + 1);
                data.exeName = DX::WideToUtf8(exeOnly);
            }
        }

        // This run information
        data.sessionId = NewGuid();

    }

    void UploadTelemetry(XAsyncBlock *async)
    {
        if (m_authToken.length() == 0)
        {
            AuthPlayfab(async);
            return;
        }

        async->callback = [](XAsyncBlock *async)
        {
            HCCallHandle httpCall = static_cast<HCCallHandle>(async->context);
            auto response = PlayFabResponse::FromCall(httpCall);
            HCHttpCallCloseHandle(httpCall);
            delete async;
        };


        std::string payload = CreateEventPayload(*m_telemetry);

        auto httpCall = CreateEventRequest(m_authToken, payload);

        async->context = httpCall;
        if (FAILED(HCHttpCallPerformAsync(httpCall, async)))
        {
            delete async;
        }
    }

    void AuthPlayfab(XAsyncBlock *async)
    {
        struct AuthContext
        {
            ATGTelemetry *_this;
            HCCallHandle httpCallHandle;
        };

        async->callback = [](XAsyncBlock *async)
        {
            auto ctx = static_cast<AuthContext *>(async->context);

            HCCallHandle httpCall = ctx->httpCallHandle;

            auto response = PlayFabResponse::FromCall(httpCall);

            HCHttpCallCloseHandle(httpCall);

            if (response.IsSuccessStatus())
            {
                auto respJs = response.AsJson();
                ctx->_this->m_authToken = respJs["data"]["EntityToken"]["EntityToken"].get<std::string>();
                ctx->_this->UploadTelemetry(async);
            }
            else
            {
                delete async;
            }
        };

        HCCallHandle httpCall = CreateAuthRequest(m_telemetry->clientId);
        auto ctx = new AuthContext({ this, httpCall });
        async->context = ctx;

        if (FAILED(HCHttpCallPerformAsync(httpCall, async)))
        {
            delete async;
        }
    }

private:
    // Base telemetry data to capture and serialize
    struct TelemetryData
    {
        std::string osVersion;
        std::string hostingOsVersion;
        std::string family;
        std::string form;
        std::string clientId;
        std::string sandbox;
        std::string titleId;
        std::string exeName;
        std::string sessionId;

        json ToJson() const
        {
            json js;
            js["osVersion"] = osVersion;
            js["hostingOsVersion"] = hostingOsVersion;
            js["family"] = family;
            js["form"] = form;
            js["clientId"] = clientId;
            js["sandboxId"] = sandbox;
            js["titleId"] = titleId;
            js["exeName"] = exeName;
            js["sessionId"] = sessionId;

            return js;
        }
    };

private:
    // HTTP response from a PlayFab API request
    class PlayFabResponse
    {
    public:
        static PlayFabResponse FromCall(HCCallHandle httpCall)
        {
            PlayFabResponse response;

            HCHttpCallResponseGetStatusCode(httpCall, &response.status);

            const char *responseString;
            HCHttpCallResponseGetResponseString(httpCall, &responseString);
            response.dataString = responseString;

            return response;
        }

    public:
        const std::string & AsString() const
        {
            return dataString;
        }

        json AsJson() const
        {
            return json::parse(dataString);
        }

        bool IsSuccessStatus() { return status >= 200 && status < 300; }

    private:
        PlayFabResponse() = default;

        uint32_t status;
        std::string dataString;
    };

private:
    static std::string CreateAuthPayload(const std::string& clientId)
    {
        json payload;

        payload["CreateAccount"] = true;
        payload["CustomId"] = clientId;
        payload["TitleId"] = "3436";

        return payload.dump();
    }

    static HCCallHandle CreateAuthRequest(const std::string& clientId)
    {
        HCCallHandle httpCall;
        HCHttpCallCreate(&httpCall);
        HCHttpCallRequestSetUrl(httpCall, "POST", LOGIN_URL);

        std::string payload = CreateAuthPayload(clientId);

        HCHttpCallRequestSetRequestBodyString(httpCall, payload.c_str());
        HCHttpCallRequestSetHeader(httpCall, "Content-Type", "application/json", true);

        return httpCall;
    }

    static std::string CreateEventPayload(const TelemetryData& data)
    {
        json ev;
        ev["EventNamespace"] = "com.playfab.events.samples";
        ev["Name"] = "launch";
        ev["Payload"] = data.ToJson();

        json evJs;
        evJs["Events"] = json::array({ ev });

        return evJs.dump();
    }

    static HCCallHandle CreateEventRequest(const std::string &authToken, const std::string &payload)
    {
        HCCallHandle httpCall;

        HCHttpCallCreate(&httpCall);

        HCHttpCallRequestSetUrl(httpCall, "POST", EVENT_URL);

        HCHttpCallRequestSetRequestBodyString(httpCall, payload.c_str());
        HCHttpCallRequestSetHeader(httpCall, "Content-Type", "application/json", true);
        HCHttpCallRequestSetHeader(httpCall, "X-EntityToken", authToken.c_str(), false);

        return httpCall;
    }

    static std::string XVersionToString(XVersion version)
    {
        std::stringstream ss;
        ss << version.major << "." << version.minor << "." << version.build << "." << version.revision;
        return ss.str();
    }

    static std::string ToHexString(uint32_t value)
    {
        std::stringstream ss;
        ss << std::hex << value;
        return ss.str();
    }

private:
    std::string m_authToken;

    std::unique_ptr<TelemetryData> m_telemetry;

#if _GAMING_XBOX
    HANDLE m_notificationChangedHandle = nullptr;
#endif
};

static std::unique_ptr<ATGTelemetry> s_atgTelem;
void ATG::SendLaunchTelemetry()
{
    if (!s_atgTelem)
    {
        s_atgTelem = std::make_unique<ATGTelemetry>();
    }

    s_atgTelem->SendTelemetry();
}
#endif
