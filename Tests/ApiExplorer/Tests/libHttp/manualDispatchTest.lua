test = require 'u-test'
common = require 'common'

callCompleted=false;

function OnHCHttpCallPerformAsync()
    print("HCHttpCallPerformAsync Complete")
    HCHttpCallCloseHandle();
    callCompleted=true;
    SetCheckHR(1);
end

function OnHCCleanupAsync()
    print("HCCleanupAsync Complete")
    StopManualDispatchThread();
    test.assert(callCompleted)
    test.stopTest();
end

test.skip = true
test.ManualDispatchTest = function()
    HCInitialize();
    XTaskQueueCreate()
    StartManualDispatchThread();
    HCHttpCallCreate();
    HCHttpCallRequestSetUrl();
    HCHttpCallGetRequestUrl();
    HCHttpCallRequestSetRequestBodyString();
    HCHttpCallRequestSetHeader();
    SetCheckHR(0); -- PerformAsync should fail here
    HCHttpCallPerformAsync();
    HCCleanupAsync();
end