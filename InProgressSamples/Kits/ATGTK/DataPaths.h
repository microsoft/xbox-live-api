// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Helpers for finding standard data paths at runtime
//
#pragma once

#include <exception>
#include <string>

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#include <Windows.ApplicationModel.h>
#include <Windows.Storage.h>
#elif !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#include <shlobj.h>
#endif


namespace DX
{
    class DataPaths
    {
    public:
        DataPaths() :
            m_appDataFolder{},
            m_tempFolder{},
            m_installedFolder{}
        {
#if defined(_XBOX_ONE) && defined(_TITLE)
            // There's not always an app data folder, so we just use the non-retail scratch drive for this helper
            wcscpy_s(m_appDataFolder, L"D:\\");

            wcscpy_s(m_tempFolder, L"T:\\");
            wcscpy_s(m_installedFolder, L"G:\\");
#elif defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)

            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;

            ComPtr<ABI::Windows::Storage::IApplicationDataStatics> appStatics;
            DX::ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_ApplicationData).Get(), appStatics.GetAddressOf()));

            ComPtr<ABI::Windows::Storage::IApplicationData> appData;
            DX::ThrowIfFailed(appStatics->get_Current(appData.GetAddressOf()));

            // Temporary folder
            {
                ComPtr<ABI::Windows::Storage::IStorageFolder> folder;
                DX::ThrowIfFailed(appData->get_TemporaryFolder(folder.GetAddressOf()));

                ComPtr<ABI::Windows::Storage::IStorageItem> item;
                DX::ThrowIfFailed(folder.As(&item));

                HString folderName;
                DX::ThrowIfFailed(item->get_Path(folderName.GetAddressOf()));

                unsigned int len;
                PCWSTR szPath = folderName.GetRawBuffer(&len);
                if (wcscpy_s(m_tempFolder, MAX_PATH, szPath) > 0
                    || wcscat_s(m_tempFolder, L"\\") > 0)
                {
                    throw std::exception("TemporaryFolder");
                }
            }

            // Application folder
            {
                ComPtr<ABI::Windows::Storage::IStorageFolder> folder;
                DX::ThrowIfFailed(appData->get_LocalFolder(folder.GetAddressOf()));

                ComPtr<ABI::Windows::Storage::IStorageItem> item;
                DX::ThrowIfFailed(folder.As(&item));

                HString folderName;
                DX::ThrowIfFailed(item->get_Path(folderName.GetAddressOf()));

                unsigned int len;
                PCWSTR szPath = folderName.GetRawBuffer(&len);
                if (wcscpy_s(m_appDataFolder, MAX_PATH, szPath) > 0
                    || wcscat_s(m_appDataFolder, L"\\") > 0)
                {
                    throw std::exception("LocalFolder");
                }
            }

            // Installed location
            ComPtr<ABI::Windows::ApplicationModel::IPackageStatics> pkgStatics;
            DX::ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_ApplicationModel_Package).Get(), pkgStatics.GetAddressOf()));

            ComPtr<ABI::Windows::ApplicationModel::IPackage> package;
            DX::ThrowIfFailed(pkgStatics->get_Current(package.GetAddressOf()));

            {
                ComPtr<ABI::Windows::Storage::IStorageFolder> folder;
                DX::ThrowIfFailed(package->get_InstalledLocation(folder.GetAddressOf()));

                ComPtr<ABI::Windows::Storage::IStorageItem> item;
                DX::ThrowIfFailed(folder.As(&item));

                HString folderName;
                DX::ThrowIfFailed(item->get_Path(folderName.GetAddressOf()));

                unsigned int len;
                PCWSTR szPath = folderName.GetRawBuffer(&len);
                if (wcscpy_s(m_installedFolder, MAX_PATH, szPath) > 0
                    || wcscat_s(m_installedFolder, L"\\") > 0)
                {
                    throw std::exception("InstalledLocation");
                }
            }
#else
            // Relies on an exported global from Main.cpp
            extern LPCWSTR g_szAppName;

            // Temporary folder
            DWORD nc = GetTempPath(MAX_PATH, m_tempFolder);
            if (nc > MAX_PATH || nc == 0)
            {
                throw std::exception("GetTempPath");
            }

            #pragma warning(suppress : 6054)
            if (wcscat_s(m_tempFolder, g_szAppName) > 0)
            {
                throw std::exception("Invalid temporary folder");
            }

            if (!CreateDirectory(m_tempFolder, nullptr))
            {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS)
                {
                    throw std::exception("Invalid temporary folder");
                }
            }

            if (wcscat_s(m_tempFolder, L"\\") > 0)
            {
                throw std::exception("Invalid temporary folder");
            }

            // Application folder
            Microsoft::WRL::ComPtr<IKnownFolderManager> mgr;
            DX::ThrowIfFailed(CoCreateInstance(CLSID_KnownFolderManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(mgr.GetAddressOf())));

            Microsoft::WRL::ComPtr<IKnownFolder> folder;
            DX::ThrowIfFailed(mgr->GetFolder(FOLDERID_LocalAppData, folder.GetAddressOf()));

            LPWSTR path = nullptr;
            DX::ThrowIfFailed(folder->GetPath(0, &path));
            wcscpy_s(m_appDataFolder, path);
            CoTaskMemFree(path);

            if (wcscat_s(m_appDataFolder, L"\\") > 0
                || wcscat_s(m_appDataFolder, g_szAppName) > 0)
            {
                throw std::exception("Invalid application folder");
            }

            if (!CreateDirectory(m_appDataFolder, nullptr))
            {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS)
                {
                    throw std::exception("Invalid application folder");
                }
            }

            if (wcscat_s(m_appDataFolder, L"\\") > 0)
            {
                throw std::exception("Invalid application folder");
            }

            // Installed location
            wchar_t exePath[MAX_PATH];
            nc = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
            if (nc > MAX_PATH || nc == 0)
            {
                throw std::exception("GetModuleFileNameW");
            }

            wchar_t drive[_MAX_DRIVE];
            wchar_t dir[_MAX_DIR];
            if (_wsplitpath_s(exePath, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0) != 0)
            {
                throw std::exception("Invalid installed folder");
            }

            if (_wmakepath_s(m_installedFolder, drive, dir, nullptr, nullptr) != 0)
            {
                throw std::exception("Invalid installed folder");
            }
#endif
        }

        const wchar_t* AppDataFolder() const { return m_appDataFolder; }
        const wchar_t* TempFolder() const { return m_tempFolder; }
        const wchar_t* InstalledFolder() const { return m_installedFolder; }

    private:
        wchar_t m_appDataFolder[MAX_PATH];
        wchar_t m_tempFolder[MAX_PATH];
        wchar_t m_installedFolder[MAX_PATH];
    };
}