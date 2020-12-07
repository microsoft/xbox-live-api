:: Script isn't quite working yet, mostly keeping here so I can cut and paste the commands to quickly build the chms.

GOTO END

if NOT exist C:\xbox.services.xboxlivesdk\Tools\CHMMerger\CHMMerger\bin\Debug\CHMMerger.exe goto NEED_BUILD

:: Build Xbox Live SDK chm

cd C:\xbox.services.xboxlivesdk\Tools\CHMMerger\CHMMerger\bin\Debug

CHMMerger.exe -t "sdk" -w "C:\xbox.services.xboxlivesdk\Docs\working" -m "C:\xbox.services.xboxlivesdk\Docs\CombinedChmTemplateUWP" -p "C:\xbox.services.xboxlivesdk\Docs\source_chms\Xbox Live programming guide.chm" -n "C:\xbox.services.xboxlivesdk\Docs\source_chms\xblsdk_winrt_uwp.chm" -c "C:\xbox.services.xboxlivesdk\Docs\source_chms\xblsdk_cpp_uwp.chm" -r "C:\xbox.services.xboxlivesdk\Docs\source_chms\XboxLiveREST.chm" -e "C:\xbox.services.xboxlivesdk\Docs\source_chms\xbl_platform_extensions_sdk.chm"

cd c:\xbox.services.xboxlivesdk\Docs

xcopy XboxLiveSDK.chm XboxLiveSDK_uwp.chm /y

:: Build XDK chm

cd C:\xbox.services.xboxlivesdk\Tools\CHMMerger\CHMMerger\bin\Debug

CHMMerger.exe -t "xdk" -w "C:\xbox.services.xboxlivesdk\Docs\working" -m "C:\xbox.services.xboxlivesdk\Docs\CombinedChmTemplateXDK" -p "C:\xbox.services.xboxlivesdk\Docs\source_chms\Xbox Live programming guide.chm" -n "C:\xbox.services.xboxlivesdk\Docs\source_chms\xblsdk_winrt_Xbox.chm" -c "C:\xbox.services.xboxlivesdk\Docs\source_chms\xblsdk_cpp_Xbox.chm" -r "C:\xbox.services.xboxlivesdk\Docs\source_chms\XboxLiveREST.chm"

cd c:\xbox.services.xboxlivesdk\Docs

xcopy XboxLiveSDK.chm XboxLiveSDK_xdk.chm /y

GOTO CLEANUP

:CLEANUP

del C:\xbox.services.xboxlivesdk\Docs\CombinedChmTemplateUWP_working /Q
del C:\xbox.services.xboxlivesdk\Docs\CombinedChmTemplateXDK_working /Q
del XboxLiveSDK.chm

GOTO END

:NEED_BUILD

echo "You must first build the CHMMerger tool."

:END
