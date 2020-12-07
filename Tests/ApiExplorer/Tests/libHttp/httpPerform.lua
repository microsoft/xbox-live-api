
test = require 'u-test'
common = require 'common'

function OnHCHttpCallPerformAsync()
    print("OnHCHttpCallPerformAsync")
    HCHttpCallResponseGetResponseString();
    HCHttpCallResponseGetStatusCode();
    HCHttpCallResponseGetNetworkErrorCode();
    HCHttpCallResponseGetHeader();
    HCHttpCallResponseGetNumHeaders();
    HCHttpCallResponseGetHeaderAtIndex();
    --HCHttpCallResponseGetResponseBodyBytes
    --HCHttpCallResponseGetResponseBodyBytesSize

    HCHttpCallCloseHandle();
    test.stopTest();
end

test.testHttpPerform = function()
    print("testHttpPerform")

    HCInitialize();
    --HCAddCallRoutedHandler
    --HCRemoveCallRoutedHandler
    HCHttpCallCreate();
    HCHttpCallRequestSetUrl();
    HCHttpCallGetRequestUrl();
    HCHttpCallRequestSetRequestBodyString();
    HCHttpCallRequestSetHeader();
    HCHttpCallPerformAsync();
end

