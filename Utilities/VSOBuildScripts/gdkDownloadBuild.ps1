$destPath = $args[0]
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

    Write-Host "`$buildId = $buildId"
    Write-Host "`$destPath = $destPath"

    # az login if needed
    az config set extension.use_dynamic_install=yes_without_prompt
    $account = az account show
    if( !$account )
    {
        [Console]::ResetColor()
        az login
    }
    
    az pipelines runs artifact download --run-id $buildId --path $destPath\gdk --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2019_Debug
    az pipelines runs artifact download --run-id $buildId --path $destPath\gdk --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2019_Release
    az pipelines runs artifact download --run-id $buildId --path $destPath\gdk --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2017_Debug
    az pipelines runs artifact download --run-id $buildId --path $destPath\gdk --artifact-name XSAPI_Combined_Full_Build_PipelineArtifacts_GDK2017_Release
}
else
{
    Write-Warning "gdkDownloadBuild.ps1 `"destPath`" `"buildURL`""
    Write-Warning ""
    Write-Warning "Example:"
    Write-Warning ".\gdkDownloadBuild.ps1 c:\test `"https://microsoft.visualstudio.com/Xbox.Services/_build/results?buildId=30866009&view=logs&j=19f53634-0cc9-5f8f-beda-93fbd1295bdf&t=aa6975a6-c137-5e34-3a5f-3ba934582e17&l=66`""
}
