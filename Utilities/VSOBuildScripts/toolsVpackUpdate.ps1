$destOSPath = $args[0]

function Get-BuildId
{
    param([Parameter(Mandatory)] [string]$url)

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
            
            if ($varName -eq "buildId")
            {
                return $varValue
            }
        }
    }
}

function Get-Tools 
{
    param([Parameter(Mandatory)] [string]$destOSPath,
          [Parameter(Mandatory)] [string]$Trace_Analyzer_BuildUrl,
          [Parameter(Mandatory)] [string]$Xbox_Tools_Packaging_BuildUrl,
          [Parameter(Mandatory)] [string]$XBL_DevTools_Rolling_BuildUrl
    )

    $traceAnalyzerBuildId = Get-BuildId $Trace_Analyzer_BuildUrl
    $xboxToolsPackagingBuildId = Get-BuildId $Xbox_Tools_Packaging_BuildUrl
    $xblDevToolsRollingBuildId = Get-BuildId $XBL_DevTools_Rolling_BuildUrl

    $destPathTemp = Join-Path -Path $destOSPath -ChildPath "\obj\tools-vpack-temp"
    $destPath = Join-Path -Path $destOSPath -ChildPath "\src\xbox\devex\vs\extensionSdks\xsapi\tools"

    Write-Host ""
    Write-Host "`$traceAnalyzerBuildId: $traceAnalyzerBuildId"
    Write-Host "`$xboxToolsPackagingBuildId: $xboxToolsPackagingBuildId"
    Write-Host "`$xblDevToolsRollingBuildId: $xblDevToolsRollingBuildId"
    Write-Host "`$destOSPath: $destOSPath"
    Write-Host "`$destPathTemp: $destPathTemp"
    Write-Host "`$destPath: $destPath"

    if (Test-Path $destPathTemp)
    {
        Remove-Item -LiteralPath $destPathTemp -Force -Recurse
    }

    mkdir $destPathTemp

    # az login if needed
    az config set extension.use_dynamic_install=yes_without_prompt
    $account = az account show

    if( !$account )
    {
        [Console]::ResetColor()
        az login
    }

    az pipelines runs artifact download --run-id $traceAnalyzerBuildId      --path $destPathTemp --artifact-name Trace_Analyzer_PipelineArtifacts_VPack_Release
    az pipelines runs artifact download --run-id $xboxToolsPackagingBuildId --path $destPathTemp --artifact-name Xbox_Tools_Packaging_PipelineArtifacts_VPack_Release
    az pipelines runs artifact download --run-id $xblDevToolsRollingBuildId --path $destPathTemp --artifact-name XBL_DevTools_Rolling_PipelineArtifacts_VPack_Release

    $srcPathTraceAnalyzerMan      = Join-Path -Path $destPathTemp -ChildPath "\TraceAnalyzer.man"
    $srcPathXboxToolsPackagingMan = Join-Path -Path $destPathTemp -ChildPath "\XboxLiveTools.man"
    $srcPathXblDevToolsRollingMan = Join-Path -Path $destPathTemp -ChildPath "\XboxLiveDeveloperTools.Release.man"

    $srcPathXblConfig             = Join-Path -Path $destPathTemp -ChildPath "\XblConfig.exe"
    $srcPathXblPCSandbox          = Join-Path -Path $destPathTemp -ChildPath "\XblPCSandbox.exe"
    $srcPathXblDevAccount         = Join-Path -Path $destPathTemp -ChildPath "\XblDevAccount.exe"
    $srcPathXblTestAccountGui     = Join-Path -Path $destPathTemp -ChildPath "\XblTestAccountGui.exe"
    $srcPathXblPlayerDataReset    = Join-Path -Path $destPathTemp -ChildPath "\XblPlayerDataReset.exe"
    $srcPathXboxLiveTraceAnalyzer = Join-Path -Path $destPathTemp -ChildPath "\XboxLiveTraceAnalyzer.exe"

    $destPathVPack = Join-Path -Path $destPath -ChildPath "\vpack"
    $destPathTools = Join-Path -Path $destPath -ChildPath "\xdkbuild"

    Copy-Item $srcPathTraceAnalyzerMan -Destination $destPathVPack
    Copy-Item $srcPathXboxToolsPackagingMan -Destination $destPathVPack
    Copy-Item $srcPathXblDevToolsRollingMan -Destination $destPathVPack

    Copy-Item $srcPathXblConfig             -Destination $destPathTools
    Copy-Item $srcPathXblPCSandbox          -Destination $destPathTools
    Copy-Item $srcPathXblDevAccount         -Destination $destPathTools
    Copy-Item $srcPathXblTestAccountGui     -Destination $destPathTools
    Copy-Item $srcPathXblPlayerDataReset    -Destination $destPathTools
    Copy-Item $srcPathXboxLiveTraceAnalyzer -Destination $destPathTools
}


if( $destOSPath )
{
    Get-Tools $destOSPath
}
else
{
    Write-Warning "toolsVpackUpdate.ps1 `"OSPath`""
    Write-Warning ""
    Write-Warning "Example:"
    Write-Warning ".\toolsVpackUpdate.ps1 d:\os"
}

