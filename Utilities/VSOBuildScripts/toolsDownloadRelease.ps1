$destRootPath = $args[0]

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
    param([Parameter(Mandatory)] [string]$destRootPath,
          [Parameter(Mandatory)] [string]$Trace_Analyzer_BuildUrl,
          [Parameter(Mandatory)] [string]$Xbox_Tools_Packaging_BuildUrl,
          [Parameter(Mandatory)] [string]$XBL_DevTools_Rolling_BuildUrl
    )

    $traceAnalyzerBuildId = Get-BuildId $Trace_Analyzer_BuildUrl
    $xboxToolsPackagingBuildId = Get-BuildId $Xbox_Tools_Packaging_BuildUrl
    $xblDevToolsRollingBuildId = Get-BuildId $XBL_DevTools_Rolling_BuildUrl

    $destPath = Join-Path -Path $destRootPath -ChildPath "\toolsArtifacts"

    Write-Host ""
    Write-Host "`$traceAnalyzerBuildId: $traceAnalyzerBuildId"
    Write-Host "`$xboxToolsPackagingBuildId: $xboxToolsPackagingBuildId"
    Write-Host "`$xblDevToolsRollingBuildId: $xblDevToolsRollingBuildId"
    Write-Host "`$destPath: $destPath"

    if (Test-Path $destPath)
    {
        Remove-Item -LiteralPath $destPath -Force -Recurse
    }

    mkdir $destPath

    # az login if needed
    az config set extension.use_dynamic_install=yes_without_prompt
    $account = az account show

    if( !$account )
    {
        [Console]::ResetColor()
        az login
    }

    az pipelines runs artifact download --run-id $traceAnalyzerBuildId      --path $destPath --artifact-name Trace_Analyzer_PipelineArtifacts_Release
    az pipelines runs artifact download --run-id $xboxToolsPackagingBuildId --path $destPath --artifact-name Xbox_Tools_Packaging_PipelineArtifacts_Release
    az pipelines runs artifact download --run-id $xblDevToolsRollingBuildId --path $destPath --artifact-name XBL_DevTools_Rolling_PipelineArtifacts_Release
}


if( $destRootPath )
{
    Get-Tools $destRootPath
}
else
{
    Write-Warning "toolsVpackUpdate.ps1 `"OSPath`""
    Write-Warning ""
    Write-Warning "Example:"
    Write-Warning ".\toolsVpackUpdate.ps1 d:\os"
}

