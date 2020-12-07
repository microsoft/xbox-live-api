// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace pal
{
    void strcpy(char * dst, size_t size, const char* src)
    {
        strcpy_s(dst, size, src);
    }

    int stricmp(const char* left, const char* right)
    {
        return _stricmp(left, right);
    }

    int vsprintf(char* message, size_t size, char const* format, va_list varArgs)
    {
        int result = vsnprintf(message, size, format, varArgs);
        result = max(result, 0); // result is negative on error
        result = min(static_cast<int>(size) - 1, result); // don't let it go past size-1
        message[result] = 0;
        return result;
    }

    char* strtok(char* str, char const* delimiter, char** context)
    {
        return strtok_s(str, delimiter, context);
    }

    void Sleep(unsigned long duration)
    {
        ::Sleep(duration);
    }

    bool FileExists(std::string fileName)
    {
        struct stat fileInfo;
        return stat(fileName.c_str(), &fileInfo) == 0;
    }

    std::vector<std::string> EnumFolders(std::string folder)
    {
        std::vector<std::string> folders;
        std::string searchFolder = folder + "\\*";

        folders.push_back(folder);

        WIN32_FIND_DATAA fd;
        HANDLE hFind = ::FindFirstFileA(searchFolder.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    std::string subFolder = fd.cFileName;
                    if (subFolder != "." &&
                        subFolder != ".." &&
                        subFolder != "shared")
                    {
                        subFolder = folder;
                        subFolder += "\\";
                        subFolder += fd.cFileName;
                        folders.push_back(subFolder);
                    }
                }
            } while (::FindNextFileA(hFind, &fd));
            ::FindClose(hFind);
        }

        return folders;
    }

    std::vector<std::string> EnumFilesInFolderHelper(std::string folder, std::string spec)
    {
        std::vector<std::string> files;
        std::string searchFolder = folder + "\\" + spec;

        WIN32_FIND_DATAA fd;
        HANDLE hFind = ::FindFirstFileA(searchFolder.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    std::string filePath = folder;
                    filePath += "\\";
                    filePath += fd.cFileName;
                    files.push_back(filePath);
                }
            } while (::FindNextFileA(hFind, &fd));
            ::FindClose(hFind);
        }

        return files;
    }


    std::vector<std::string> EnumFilesInFolder(std::string folder, std::string spec)
    {
        std::vector<std::string> folders = EnumFolders(folder);
        std::vector<std::string> files;
        for (std::string iFolder : folders)
        {
            std::vector<std::string> filesHelper;
            filesHelper = EnumFilesInFolderHelper(iFolder, spec);
            for (std::string f : filesHelper)
            {
                files.push_back(f);
            }
        }

        return files;
    }

    std::string FindFile(std::string fileName)
    {
        if (FileExists(fileName))
        {
            return fileName;
        }

        char strBasePath[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, strBasePath, MAX_PATH);
        char* lastSlash = strrchr(strBasePath, '\\');
        if (lastSlash != nullptr)
        {
            *lastSlash = 0;
        }

        std::string testFilePath = Data()->m_testsPath + "\\" + fileName;
        if (FileExists(testFilePath))
        {
            return testFilePath;
        }

        testFilePath = Data()->m_testsPath + "\\..\\" + fileName;
        if (FileExists(testFilePath))
        {
            return testFilePath;
        }

        std::string fullPath;
        std::string newPath = strBasePath;
        newPath += "\\";
        for (int i = 0; i < 5; i++)
        {
            newPath += "..\\";
            testFilePath = newPath + fileName;
            if (FileExists(testFilePath))
            {
                return testFilePath;
            }

            testFilePath = newPath + "Tests\\APIExplorer\\" + fileName;
            if (FileExists(testFilePath))
            {
                return testFilePath;
            }

            testFilePath = newPath + "..\\..\\..\\..\\Tests\\APIExplorer\\" + fileName;
            if (FileExists(testFilePath))
            {
                return testFilePath;
            }
        }
        return fullPath;
    }

    std::string GetLuaPath()
    {
        std::string path{};

        auto utestPath = FindFile("_luasetup_\\u-test\\u-test.lua");
        path += utestPath.substr(0, utestPath.find_last_of('\\')) + "\\?.lua";

        auto commonPath = FindFile("_luasetup_\\xal\\common.lua");
        path += ";" + commonPath.substr(0, commonPath.find_last_of('\\')) + "\\?.lua";

        return path;
    }
}
