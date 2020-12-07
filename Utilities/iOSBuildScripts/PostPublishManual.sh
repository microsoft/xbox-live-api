#!/bin/sh
# bash command colors
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

#Error Handling
detect_error()
{
    exit_code=$?
    buildStep=$1
    flag=$2
    celebrate_success=$3
    if [ $exit_code -ne 0 ];
    then
        echo "${RED}${buildStep} failed.${NC}"
        if [ $flag -eq 1 ];
        then
            exit $exit_code
        fi
    else
if [ $celebrate_success -eq 1 ];
        then
            echo "${GREEN}${buildStep} was successful.${NC}"
        fi
    fi
}

#Copy Folder Function
copy_folder() {
    source_folder=$1
    target_folder=$2
    except_file=$3
    echo "Copying contents from $source_folder to $target_folder"
    cp -R "$source_folder" "$target_folder"
    rm "$target_folder/$except_file"
    detect_error "Copying contents from $1 to $2" 1 0
}

#Directory and File Names
DefaultBuildDirValue="../../DerivedData/Microsoft.Xbox.Services/Build/Products"
DefaultOutputDirValue="Output"
BuildDirRoot=${1:-$DefaultBuildDirValue}
OutputFolderName=${2:-$DefaultOutputDirValue}
SimulatorDebugFolderName="Debug-iphonesimulator"
iPhoneDebugFolderName="Debug-iphoneos"
SimulatorReleaseFolderName="Release-iphonesimulator"
iPhoneReleaseFolderName="Release-iphoneos"

SimulatorDebugBuildDir="$BuildDirRoot/$SimulatorDebugFolderName"
SimulatorReleaseBuildDir="$BuildDirRoot/$SimulatorReleaseFolderName"
iPhoneDebugBuildDir="$BuildDirRoot/$iPhoneDebugFolderName"
iPhoneReleaseBuildDir="$BuildDirRoot/$iPhoneReleaseFolderName"
OutputRootDir="$BuildDirRoot/$OutputFolderName"
OutputDebugDir="$OutputRootDir/Debug"
OutputReleaseDir="$OutputRootDir/Release"

XSAPIFrameworkDir="xsapi-c.framework"
XSAPIExecutable="$XSAPIFrameworkDir/xsapi-c"
XALFrameworkDir="Xal.framework"
XALExecutable="$XALFrameworkDir/XAL"
HttpClientFrameworkDir="HttpClient.framework"
HttpClientExecutable="$HttpClientFrameworkDir/HttpClient"

# Delete Output Folder if it exists
echo "Deleting $OutputRootDir and its contents"
if [ -d "$OutputRootDir" ]; then
    rm -Rf "$OutputRootDir"
fi

# Creating Output Folder if it doesn't exist
echo "Creating $OutputRootDir"
mkdir "$OutputRootDir"
detect_error "Creating Output Root Directory" 1 0

# Creating Debug SubFolder
echo "Creating $OutputDebugDir"
mkdir "$OutputDebugDir"
detect_error "Creating Output Debug Directory" 1 0

# Create FAT for LibHttpClient
echo "Creating FAT LibHttpClient"
mkdir "$OutputDebugDir/$HttpClientFrameworkDir"

copy_folder "$iPhoneDebugBuildDir/$HttpClientFrameworkDir" "$OutputDebugDir" "$HttpClientExecutable"

lipo -create \
"$SimulatorDebugBuildDir/$HttpClientExecutable" \
"$iPhoneDebugBuildDir/$HttpClientExecutable" \
-output "$OutputDebugDir/$HttpClientExecutable"

detect_error "Creating FAT LibHttpClient" 0 1

# Create FAT for XAL
echo "Creating FAT XAL"
mkdir "$OutputDebugDir/$XALFrameworkDir"

copy_folder "$iPhoneDebugBuildDir/$XALFrameworkDir" "$OutputDebugDir" "$XALExecutable"

lipo -create \
"$SimulatorDebugBuildDir/$XALExecutable" \
"$iPhoneDebugBuildDir/$XALExecutable" \
-output "$OutputDebugDir/$XALExecutable"

detect_error "Creating FAT XAL" 0 1

# Create FAT for XSAPI
echo "Creating FAT XSAPI"
mkdir "$OutputDebugDir/$XSAPIFrameworkDir"

copy_folder "$iPhoneDebugBuildDir/$XSAPIFrameworkDir" "$OutputDebugDir" "$XSAPIExecutable"

lipo -create \
    "$SimulatorDebugBuildDir/$XSAPIExecutable" \
    "$iPhoneDebugBuildDir/$XSAPIExecutable" \
    -output "$OutputDebugDir/$XSAPIExecutable"
detect_error "Creating FAT XSAPI" 0 1

# Creating Release SubFolder
echo "Creating $OutputReleaseDir"
mkdir "$OutputReleaseDir"
detect_error "Creating Output Release Directory" 1 0

# Create FAT for LibHttpClient
echo "Creating FAT LibHttpClient"
mkdir "$OutputReleaseDir/$HttpClientFrameworkDir"

copy_folder "$iPhoneReleaseBuildDir/$HttpClientFrameworkDir" "$OutputReleaseDir" "$HttpClientExecutable"

lipo -create \
"$SimulatorReleaseBuildDir/$HttpClientExecutable" \
"$iPhoneReleaseBuildDir/$HttpClientExecutable" \
-output "$OutputReleaseDir/$HttpClientExecutable"

detect_error "Creating FAT LibHttpClient" 0 1

# Create FAT for XAL
echo "Creating FAT XAL"
mkdir "$OutputReleaseDir/$XALFrameworkDir"

copy_folder "$iPhoneReleaseBuildDir/$XALFrameworkDir" "$OutputReleaseDir" "$XALExecutable"

lipo -create \
"$SimulatorReleaseBuildDir/$XALExecutable" \
"$iPhoneReleaseBuildDir/$XALExecutable" \
-output "$OutputReleaseDir/$XALExecutable"

detect_error "Creating FAT XAL" 0 1

# Create FAT for XSAPI
echo "Creating FAT XSAPI"
mkdir "$OutputReleaseDir/$XSAPIFrameworkDir"

copy_folder "$iPhoneReleaseBuildDir/$XSAPIFrameworkDir" "$OutputReleaseDir" "$XSAPIExecutable"

lipo -create \
"$SimulatorReleaseBuildDir/$XSAPIExecutable" \
"$iPhoneReleaseBuildDir/$XSAPIExecutable" \
-output "$OutputReleaseDir/$XSAPIExecutable"
detect_error "Creating FAT XSAPI" 0 1
