set CMAKE_FOLDER=%ROOT_FOLDER%\Utilities\CMake
rmdir /q /s %CMAKE_FOLDER%\build
mkdir %CMAKE_FOLDER%\build
mkdir %CMAKE_FOLDER%\vcxprojs
cd %CMAKE_FOLDER%\build
echo This file is created on build server > build.cpp

set CMAKE_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if NOT EXIST %CMAKE_EXE% set CMAKE_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if NOT EXIST %CMAKE_EXE% set CMAKE_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if NOT EXIST %CMAKE_EXE% set CMAKE_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
 
%CMAKE_EXE% -G "Visual Studio 15 2017" %* CMakeLists.txt %CMAKE_FOLDER%
call :subCopy
goto done

:subCopy
del ALL_BUILD.*
del *.sln
del CMakeCache.txt
del cmake_install.cmake
rmdir /q /s %CMAKE_FOLDER%\build\CMakeFiles
cd %CMAKE_FOLDER%
move %CMAKE_FOLDER%\build\*.vcxproj %CMAKE_FOLDER%\vcxprojs
move %CMAKE_FOLDER%\build\*.vcxproj.filters %CMAKE_FOLDER%\vcxprojs
cd %CMAKE_FOLDER%\build
goto:EOF

echo end

goto done

:done
del %CMAKE_FOLDER%\build\build.cpp
cd %CMAKE_FOLDER%
rmdir /q /s %CMAKE_FOLDER%\build


