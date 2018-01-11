Microsoft.Xbox.Services Source Distribution v. VERSION
-------------------------------------------
The Microsoft.Xbox.Services binaries shipped inside this XDK version was built by source found 
on GitHub at https://github.com/Microsoft/xbox-live-api/tree/GITCOMMIT

To get a local copy of the source, do the following:

git clone --recursive https://github.com/Microsoft/xbox-live-api.git
cd xbox-live-api
git reset --hard GITCOMMIT
git submodule sync
git submodule update --init --recursive

To use this source in your project, follow the guide at https://docs.microsoft.com/en-us/windows/uwp/xbox-live/get-started-with-partner/compile-the-xdk-xbox-live-api-source
