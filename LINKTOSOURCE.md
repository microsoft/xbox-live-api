## How to link your project against source

You might want to link against the XSAPI source if you want to debug an issue, or understand where an error code is coming from.  

Jump to the guide that matches the platform and API type you are using:
- [How to link against the C++ UWP source](LINKTOSOURCE.md#how-to-link-against-the-xsapi-c-uwp-source)
- [How to link against the WinRT UWP source](LINKTOSOURCE.md#how-to-link-against-the-xsapi-winrt-uwp-source)
- [How to link against the C++ XDK source](LINKTOSOURCE.md#how-to-link-against-the-xsapi-c-xdk-source)
- [How to link against the WinRT XDK source](LINKTOSOURCE.md#how-to-link-against-the-xsapi-winrt-xdk-source)

### How to link against the XSAPI C++ UWP source

- If your project references the pre-built NuGet package, you need to remove the reference. 
   - In Visual Studio, right click on project and choose "Manage NuGet Packages...", and if Microsoft.Xbox.Live.SDK.Cpp.UWP is installed, click "Uninstall" and click "OK" and wait until its removed.
   
- In Visual Studio, choose "File->Add->Existing Project..." in Visual Studio to add the following two projects to your application's solution. The vcxproj files will be located in the folder you extracted the source to.   

For Visual Studio 2015:
```
  \Build\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj
  \External\libHttpClient\Build\libHttpClient.140.UWP.C\libHttpClient.140.UWP.C.vcxproj
  \External\cpprestsdk\Release\src\build\vs14.uwp\cpprestsdk140.uwp.static.vcxproj
```

For Visual Studio 2017:
```
  \Build\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj
  \External\libHttpClient\Build\libHttpClient.141.UWP.C\libHttpClient.141.UWP.C.vcxproj
  \External\cpprestsdk\Release\src\build\vs15.uwp\cpprestsdk141.uwp.static.vcxproj
```

- Add the source projects as a reference by choosing Project->References... and select "Add Reference". Under "Solution->Projects", check the entries for both projects above then click OK.
- Add the props file to your project by clicking "View->Other Windows->Property Manager", right clicking on your project, selecting "Add Existing Property Sheet", then finally selecting the xsapi.staticlib.props file in the SDK source root.
- Include the services.h file to your app source
```
   #include "xsapi\services.h"
```
- Rebuild your Visual Studio solution

### How to link against the XSAPI C++ XDK source
- If your project references the pre-built DLL the comes with the XDK, you need to remove the reference. 
  - In Visual Studio: Choose “Project->Add References…” in Visual Studio. If Xbox Services API is checked, uncheck it. Click “OK” and save the project file.
- In Visual Studio, choose "File->Add->Existing Project..." in Visual Studio to add the following two projects to your application's solution. The vcxproj files will be located in the folder you extracted the source to.   

For Visual Studio 2015:
```
  \Build\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj
  \External\libHttpClient\Build\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.vcxproj
  \External\cpprestsdk\Release\src\build\vs14.xbox\casablanca140.Xbox.vcxproj
```

For Visual Studio 2017:
```
  \Build\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj
  \External\libHttpClient\Build\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.vcxproj
  \External\cpprestsdk\Release\src\build\vs15.xbox\casablanca141.Xbox.vcxproj
```
- Add the source projects as a reference by choosing Project->References... and select "Add Reference". Under "Solution->Projects", check the entries for both projects above then click OK.
- Add the props file to your project by clicking "View->Other Windows->Property Manager", right clicking on your project, selecting "Add Existing Property Sheet", then finally selecting the xsapi.staticlib.props file in the SDK source root.
- Include the services.h file to your app source
```
   #include "xsapi\services.h"
```
- Rebuild your Visual Studio solution

### How to link against the XSAPI WinRT UWP source

- If you are using VS2017 for WinRT, we only have a single WinRT project that supports both VS2015 and VS2017. To use it from source, you'll need to install 140 support in VS2017. Run the VS2017 installer, modify, and go to Individual components tab and choose "VS++ 2015.3 v140 toolset". Then you should be able to open \Build\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj by itself in VS2017 and build it and follow the steps below.
- If your project references the pre-built NuGet package, you need to remove the reference. 
  - In Visual Studio, right click on project and choose "Manage NuGet Packages...", and if Microsoft.Xbox.Live.SDK.WinRT.UWP is installed, click "Uninstall" and click "OK" and wait until its removed.   

- In Visual Studio, choose "File->Add->Existing Project..." in Visual Studio to add the following two projects to your application's solution. The vcxproj files will be located in the folder you extracted the source to.

For Visual Studio 2015 & Visual Studio 2017 (with v140 toolset):
```
\Build\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj
\External\libHttpClient\Build\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.vcxproj
\External\cpprestsdk\Release\src\build\vs14.uwp\cpprestsdk140.uwp.vcxproj
```

- Add the source projects as a reference by choosing Project->References... and select "Add Reference". Under "Solution->Projects", check the entries for both projects above then click OK.
- Ensure that the "Output Folder" of both the application project and the projects above are the same. This setting can be found in Visual Studio project Properties->Configuration Properties->General->Output Directory.
- Rebuild your Visual Studio solution

### How to link against the XSAPI WinRT XDK source

- If your project references the pre-built DLL the comes with the XDK, you need to remove the reference.
  - For Visual Studio: Choose “Project->Add References…” in Visual Studio. If Xbox Services API is checked, uncheck it. Click “OK” and save the project file.

- In Visual Studio, choose "File->Add->Existing Project..." in Visual Studio to add the following two projects to your application's solution. The vcxproj files will be located in the folder you extracted the source to.

For Visual Studio 2015:
```
\Build\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.140.XDK.WinRT.vcxproj
\External\libHttpClient\Build\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.vcxproj
\External\cpprestsdk\Release\src\build\vs14.uwp\cpprestsdk140.uwp.vcxproj
```
For Visual Studio 2017:
```
  \Build\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj
  \External\libHttpClient\Build\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.vcxproj
  \External\cpprestsdk\Release\src\build\vs15.xbox\casablanca141.Xbox.vcxproj
```

- In Visual Studio add the references:
  - For Visual Studio 2015: Choose “Project->Add References…” in Visual Studio. Under Projects, check the entries for both projects above and click OK.
- Ensure that the "Output Folder" of both the application project and the Xbox Services Project are the same. This setting can be found in Visual Studio project Properties->Configuration Properties->General->Output Directory.
- Rebuild your Visual Studio solution

