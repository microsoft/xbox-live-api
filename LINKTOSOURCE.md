## How to link your project against source

You might want to link against the XSAPI source if you want to debug an issue, or understand where an error code is coming from.  

Jump to the guide that matches the platform and API type you are using:
- [How to link against the C GDK source](LINKTOSOURCE.md#how-to-link-against-the-xsapi-gdk-source)

### How to link against the XSAPI GDK source

- In Visual Studio, choose "File->Add->Existing Project..." in Visual Studio to add the following projects to your application's solution. The project paths below are relative to the SDK source root.

For Visual Studio 2017:
```
  \Build\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.vcxproj
  \External\xal\External\libHttpClient\Build\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.vcxproj
```

For Visual Studio 2019:
```
  \Build\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.vcxproj
  \External\xal\External\libHttpClient\Build\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.vcxproj
```

- Add the XSAPI props file to your project by clicking "View->Other Windows->Property Manager", right clicking on your project, selecting "Add Existing Property Sheet", then finally selecting the xsapi.staticlib.props file in the SDK source root. This will automatically update your projects XSAPI header include paths, add references to the relevant XSAPI projects, and prevent your project from linking with the installed GDK binaries.

- Rebuild your Visual Studio solution. Note that in some cases the XSAPI project references won't be fully added until you reload your project after adding the XSAPI property sheet.



