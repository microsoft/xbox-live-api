:: If first parameter is "copy", then copy chms to source_chms
:: If first parameter is "copyonly", then skip doxygen generation and copy the chms to source_chms

if "%1"=="copyonly" goto COPY

if not exist xblsdk_cpp\ mkdir xblsdk_cpp
if not exist xblsdk_winrt\ mkdir xblsdk_winrt

doxygen source_chms\xblsdk_winrt_uwp.config
doxygen source_chms\xblsdk_winrt_Xbox.config
doxygen source_chms\xblsdk_cpp_uwp.config
doxygen source_chms\xblsdk_cpp_Xbox.config

if not "%1"=="copy" goto END

:COPY
echo "Copying chms to source_chms directory."
xcopy xblsdk_winrt\uwp\xblsdk_winrt_uwp.chm source_chms\xblsdk_winrt_uwp.chm /y
xcopy xblsdk_winrt\Xbox\xblsdk_winrt_Xbox.chm source_chms\xblsdk_winrt_Xbox.chm /y
xcopy xblsdk_cpp\uwp\xblsdk_cpp_uwp.chm source_chms\xblsdk_cpp_uwp.chm /y
xcopy xblsdk_cpp\Xbox\xblsdk_cpp_Xbox.chm source_chms\xblsdk_cpp_Xbox.chm /y

:END



