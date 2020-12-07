if "%JAVA_HOME%" == "" set JAVA_HOME="C:\Program Files (x86)\Java\jdk1.8.0_181"
if "%ANDROID_HOME%" == "" set ANDROID_HOME=C:\ProgramData\Microsoft\AndroidSDK\25
call %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\gradlew.bat --project-dir=%TFS_SourcesDirectory%\Java\com.microsoft.xboxlive copyNdkLibsToAAR -PInputAar=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\com.microsoft.xboxlive.aar -POutDir=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\ndklibs --info
call %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\Java\com.microsoft.xboxlive publish -PInputAar=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\ndklibs\com.microsoft.xboxlive.aar -PLocalMavenRepositoryPath=%TFS_SourcesDirectory%\Maven --info
call %TFS_SourcesDirectory%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java publish -PaarPath=%TFS_BinariesDirectory%\Release\x86\libHttpClient.141.Android.Java\libHttpClient.aar -PLocalMavenRepositoryPath=%TFS_SourcesDirectory%\Maven --info
rem XAL uses the proper Android SDK installation, not the Visual Studio one
set ANDROID_HOME=C:\Program Files (x86)\Android\android-sdk
call %TFS_SourcesDirectory%\external\xal\source\XalAndroidJava\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\external\xal\source\XalAndroidJava publish -PaarPath=%TFS_BinariesDirectory%\Release\x86\XalAndroidJava\XalAndroidJava.aar -PLocalMavenRepositoryPath=%TFS_SourcesDirectory%\Maven --info

cd %TFS_SourcesDirectory%\Maven\XsapiAndroid\com.microsoft.xboxlive\0.0.0
copy com.microsoft.xboxlive-0.0.0.aar com.microsoft.xboxlive-0.0.0.jar
mkdir com.microsoft.xboxlive-0.0.0 & cd com.microsoft.xboxlive-0.0.0
jar xf ..\com.microsoft.xboxlive-0.0.0.jar