//--------------------------------------------------------------------------------------
// Main.cpp
//
// Entry point for Microsoft Game Development Kit
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "APIRunner.GDK.h"

#include <appnotify.h>
#include <XGameRuntimeInit.h>
#include <XGameErr.h>

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 4365 )
#pragma warning( disable : 4061 )
#pragma warning( disable : 4996 )
#endif
#include <rapidjson/document.h>
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif

#ifdef ATG_ENABLE_TELEMETRY
#include "ATGTelemetry.h"
#endif

using namespace DirectX;

namespace
{
    std::unique_ptr<Sample> g_sample;
#ifdef _GAMING_XBOX
    HANDLE g_plmSuspendComplete = nullptr;
    HANDLE g_plmSignalResume = nullptr;
#endif
};

LPCWSTR g_szAppName = L"APIRunner.GDK";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!XMVerifyCPUSupport())
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: This hardware does not support the required instruction set.\n");
#if defined(_GAMING_XBOX) && defined(__AVX2__)
        OutputDebugStringA("This may indicate a Gaming.Xbox.Scarlett.x64 binary is being run on an Xbox One.\n");
#endif
#endif
        return 1;
    }

    // Initialize COM for WIC usage
    if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED)))
        return 1;

    HRESULT hr = XGameRuntimeInitialize();
    if (FAILED(hr))
    {
        if (hr == E_GAMERUNTIME_DLL_NOT_FOUND || hr == E_GAMERUNTIME_VERSION_MISMATCH)
        {
#ifdef _GAMING_DESKTOP
            (void)MessageBoxW(nullptr, L"Game Runtime is not installed on this system or needs updating.", g_szAppName, MB_ICONERROR | MB_OK);
#endif
        }
        return 1;
    }

#ifdef _GAMING_XBOX
    // Default main thread to CPU 0
    SetThreadAffinityMask(GetCurrentThread(), 0x1);
#endif

    g_sample = std::make_unique<Sample>();

    // Register class and create window
#ifdef _GAMING_XBOX
    PAPPSTATE_REGISTRATION hPLM = {};
#endif
    {
        // Register class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"APIRunner_GDKWindowClass";
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window
#ifdef _GAMING_XBOX
        RECT rc = { 0, 0, 1920, 1080 };
#else
        int w, h;
        g_sample->GetDefaultSize(w, h);

        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
#endif

        HWND hwnd = CreateWindowExW(0, L"APIRunner_GDKWindowClass", g_szAppName, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
        if (!hwnd)
            return 1;

        ShowWindow(hwnd, nCmdShow);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_sample.get()));

        // Sample Usage Telemetry
        //
        // Disable or remove this code block to opt-out of sample usage telemetry
#ifdef ATG_ENABLE_TELEMETRY
        ATG::SendLaunchTelemetry();
#endif
        GetClientRect(hwnd, &rc);

        g_sample->m_cmdLine = lpCmdLine;
        g_sample->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);

#ifdef _GAMING_XBOX
        g_plmSuspendComplete = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        g_plmSignalResume = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        if (!g_plmSuspendComplete || !g_plmSignalResume)
            return 1;

        if (RegisterAppStateChangeNotification([](BOOLEAN quiesced, PVOID context)
        {
            if (quiesced)
            {
                ResetEvent(g_plmSuspendComplete);
                ResetEvent(g_plmSignalResume);

                // To ensure we use the main UI thread to process the notification, we self-post a message
                PostMessage(reinterpret_cast<HWND>(context), WM_USER, 0, 0);

                // To defer suspend, you must wait to exit this callback
                (void)WaitForSingleObject(g_plmSuspendComplete, INFINITE);
            }
            else
            {
                SetEvent(g_plmSignalResume);
            }
        }, hwnd, &hPLM))
            return 1;
#endif
    }

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_sample->Tick();
        }
    }

    g_sample.reset();

#ifdef _GAMING_XBOX
    UnregisterAppStateChangeNotification(hPLM);

    CloseHandle(g_plmSuspendComplete);
    CloseHandle(g_plmSignalResume);
#endif

    XGameRuntimeUninitialize();

    CoUninitialize();

    return (int)msg.wParam;
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _GAMING_DESKTOP
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
#endif

    auto sample = reinterpret_cast<Sample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_ACTIVATEAPP:
        if (sample)
        {
            Mouse::ProcessMessage(message, wParam, lParam);

            if (wParam)
            {
                sample->OnActivated();
            }
            else
            {
                sample->OnDeactivated();
            }
        }
        break;


#ifdef _GAMING_XBOX
    case WM_USER:
        if (sample)
        {
            sample->OnSuspending();

            // Complete deferral
            SetEvent(g_plmSuspendComplete);

            (void)WaitForSingleObject(g_plmSignalResume, INFINITE);

            sample->OnResuming();
        }
        break;
#else
    case WM_PAINT:
        if (s_in_sizemove && sample)
        {
            sample->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_MOVE:
        if (sample)
        {
            sample->OnWindowMoved();
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && sample)
                    sample->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && sample)
                sample->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && sample)
        {
            sample->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (sample)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            sample->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
    {
        auto info = reinterpret_cast<MINMAXINFO*>(lParam);
        info->ptMinTrackSize.x = 320;
        info->ptMinTrackSize.y = 200;
    }
    break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && sample)
                sample->OnSuspending();
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && sample)
                    sample->OnResuming();
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse::ProcessMessage(message, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                if (sample)
                    sample->GetDefaultSize(width, height);

                ShowWindow(hWnd, SW_SHOWNORMAL);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            s_fullscreen = !s_fullscreen;
        }
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
#endif
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Exit helper
void ExitSample()
{
    PostQuitMessage(0);
}
