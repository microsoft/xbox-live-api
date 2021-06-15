$xmlResultFile = $args[0]

[xml]$xmlContent = Get-Content -Path $xmlResultFile

Write-Host "Parsing: $xmlResultFile"

# Relevant sections in the .trx
$results     = $xmlContent.TestRun.Results
$aggregate   = $xmlContent.TestRun.ResultSummary
$definitions = $xmlContent.TestRun.TestDefinitions

# Get full run info
$aggregateId          = (Get-Date).ticks
$aggregateDuration    = New-TimeSpan
$aggregateTestsRan    = [int]$aggregate.Counters.executed
$aggregateTimestamp   = $aggregate.RunInfos.RunInfo.timestamp
$aggregateTestsFailed = $aggregateTestsRan - $aggregate.Counters.passed

$recordId = $aggregateId + 1

[System.Collections.ArrayList]$aggregateRecord = @()
[System.Collections.ArrayList]$records = @()

$replaceList = @("\t", "\r", "\n")

# Set up test entries and add TestName and ClassName (TestArea)
$definitions.UnitTest | ForEach-Object {
    $testId = $_.id
    $testName = $_.name
    $testArea = ($_.TestMethod.className -split "\.")[-1]

    $jsonItem = "{}" | ConvertFrom-Json
    $jsonItem | Add-Member -Name "id"        -Value $testId     -MemberType NoteProperty
    $jsonItem | Add-Member -Name "filename"  -Value $testArea   -MemberType NoteProperty
    $jsonItem | Add-Member -Name "testname"  -Value $testName   -MemberType NoteProperty
    $jsonItem | Add-Member -Name "testsuite" -Value "UnitTests" -MemberType NoteProperty
    [void]$records.Add($jsonItem)
}

# Add Timestamp, Duration, and Outcome to each entry and Error Logs for failed tests
$results.UnitTestResult | ForEach-Object {
    $log       = $_.Output.ErrorInfo.Message + $_.Output.ErrorInfo.StackTrace
    $testId    = $_.testId
    $outcome   = $_.outcome
    $duration  = $_.duration
    $startTime = $_.startTime

    $passed = $False

    # Possible outcomes that aren't failures (this list is flexible and can change based on team sentiment)
    if ($outcome -eq "passed" -or `
        $outcome -eq "passedButRunAborted" -or `
        $outcome -eq "notExecuted" -or `
        $outcome -eq "completed")
    {
        $passed = $True
    }

    # Find corresponding test entry and add remaining fields (and alter the id to a unique id instead of the test's guid)
    $record = $records.Where({ $_.id -eq $testId })
    $record | Add-Member -Name "id"          -Value ($recordId++) -MemberType NoteProperty -Force
    $record | Add-Member -Name "passed"      -Value $passed       -MemberType NoteProperty
    $record | Add-Member -Name "outcome"     -Value $outcome      -MemberType NoteProperty
    $record | Add-Member -Name "duration"    -Value $duration     -MemberType NoteProperty
    $record | Add-Member -Name "timestamp"   -Value $startTime    -MemberType NoteProperty
    $record | Add-Member -Name "aggregateid" -Value $aggregateId  -MemberType NoteProperty

    if (-not $passed)
    {
        # Double escape special whitespace characters to avoid json issues when uploading
        foreach ($key in $replaceList)
        {
            $log = $log -replace $key, $key
        }

        $record | Add-Member -Name "log" -Value $log -MemberType NoteProperty
    }

    $aggregateDuration += [timespan]$duration
}

$count = $records.count
Write-Host "Processed $count tests"

# Create the aggregate entry
$jsonItem = "{}" | ConvertFrom-Json
$jsonItem | Add-Member -Name "id"          -Value $aggregateId          -MemberType NoteProperty
$jsonItem | Add-Member -Name "duration"    -Value $aggregateDuration    -MemberType NoteProperty
$jsonItem | Add-Member -Name "testsran"    -Value $aggregateTestsRan    -MemberType NoteProperty
$jsonItem | Add-Member -Name "timestamp"   -Value $aggregateTimestamp   -MemberType NoteProperty
$jsonItem | Add-Member -Name "testsuite"   -Value "UnitTests"           -MemberType NoteProperty
$jsonItem | Add-Member -Name "testsfailed" -Value $aggregateTestsFailed -MemberType NoteProperty
[void]$aggregateRecord.Add($jsonItem)


# Create result files used for data ingestion
$outputDirectory = (Get-Item $xmlResultFile).directory.fullname
$outputFile = Join-Path -Path $outputDirectory -ChildPath "UnitTestResults.json"
$outputJson = "{}" | ConvertFrom-Json
$outputJson | Add-Member -Name "records" -Value $records -MemberType NoteProperty
$outputJson | ConvertTo-Json | Set-Content $outputFile

$outputFile = Join-Path -Path $outputDirectory -ChildPath "UnitTestAggregateResult.json"
$outputJson = "{}" | ConvertFrom-Json
$outputJson | Add-Member -Name "records" -Value $aggregateRecord -MemberType NoteProperty
$outputJson | ConvertTo-Json | Set-Content $outputFile