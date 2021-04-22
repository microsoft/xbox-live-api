$destOSPath = $args[0]
$url = $args[1]

if( $url )
{
    if ($url -is [uri]) 
    {
        $url = $url.ToString()
    }
    if ($url.IndexOf('?') -ge 0) 
    {
        $query = ($url -split '\?')[1]
        $query = $query.Split('#')[0]
        foreach ($q in ($query -split '&')) 
        {
            $kv = $($q + '=') -split '='
            $varName  = [uri]::UnescapeDataString($kv[0]).Trim()
            $varValue = [uri]::UnescapeDataString($kv[1])
            New-Variable -Name $varname -Value $varValue -Force
        }
    }

    $destPathTemp = Join-Path -Path $destOSPath -ChildPath "\obj\xsapi-vpack-temp"    
    $destPathXsapi = Join-Path -Path $destOSPath -ChildPath "\src\xbox\devex\vs\extensionSdks\xsapi\c"
    Write-Host ""
    Write-Host "`$buildId = $buildId"
    Write-Host "`$destOSPath = $destOSPath"
    Write-Host "`$destPathTemp: $destPathTemp"
    Write-Host "`$destPathXsapi: $destPathXsapi"
    
    Remove-Item -LiteralPath $destPathTemp -Force -Recurse
    
    # az login if needed
    az config set extension.use_dynamic_install=yes_without_prompt
    $account = az account show
    if( !$account )
    {
        [Console]::ResetColor()
        az login
    }

    az pipelines runs artifact download --run-id $buildId --path $destPathTemp --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2019_VPack_Debug
    az pipelines runs artifact download --run-id $buildId --path $destPathTemp --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2019_VPack_Release
    az pipelines runs artifact download --run-id $buildId --path $destPathTemp --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2017_VPack_Debug
    az pipelines runs artifact download --run-id $buildId --path $destPathTemp --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2017_VPack_Release
    
    $srcPath17D = Join-Path -Path $destPathTemp -ChildPath "\XSAPI.Debug.Gaming.Desktop.x64.VS2017.man"
    $srcPath17R = Join-Path -Path $destPathTemp -ChildPath "\XSAPI.Release.Gaming.Desktop.x64.VS2017.man"
    $srcPath19D = Join-Path -Path $destPathTemp -ChildPath "\XSAPI.Debug.Gaming.Desktop.x64.VS2019.man"
    $srcPath19R = Join-Path -Path $destPathTemp -ChildPath "\XSAPI.Release.Gaming.Desktop.x64.VS2019.man"    
    $srcPathInclude = Join-Path -Path $destPathTemp -ChildPath "\include\*"

    $destPath17D = Join-Path -Path $destPathXsapi -ChildPath "\vpackDebug2017\"
    $destPath17R = Join-Path -Path $destPathXsapi -ChildPath "\vpackRelease2017\"
    $destPath19D = Join-Path -Path $destPathXsapi -ChildPath "\vpackDebug2019\"
    $destPath19R = Join-Path -Path $destPathXsapi -ChildPath "\vpackRelease2019\"
    $destPathInclude = Join-Path -Path $destPathXsapi -ChildPath "\xdkbuild\include\"

    Copy-Item $srcPath17D -Destination $destPath17D
    Copy-Item $srcPath17R -Destination $destPath17R
    Copy-Item $srcPath19D -Destination $destPath19D
    Copy-Item $srcPath19R -Destination $destPath19R
    Copy-Item $srcPathInclude -Destination $destPathInclude  -Recurse -Force
}
else
{
    Write-Warning "gdkVpackUpdate.ps1 `"OSPath`" `"buildURL`""
    Write-Warning ""
    Write-Warning "Example:"
    Write-Warning ".\gdkVpackUpdate.ps1 d:\os `"https://microsoft.visualstudio.com/xsapi/_build/results?buildId=30866009`""
}

