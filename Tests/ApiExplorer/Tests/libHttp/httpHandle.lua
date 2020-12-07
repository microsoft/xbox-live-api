
test = require 'u-test'
common = require 'common'

test.testHttpHandle = function()
    print("testHttpHandle")

    HCInitialize();
    HCGetLibVersion();
    HCHttpCallCreate();
    HCHttpCallDuplicateHandle();
    HCHttpCallGetId();
    HCHttpCallSetTracing();
    HCHttpCallRequestSetUrl();
    HCHttpCallGetRequestUrl();
    HCHttpCallRequestSetRequestBodyString();
    HCHttpCallRequestSetHeader();
    HCHttpCallRequestSetRetryAllowed();
    HCHttpCallRequestSetRetryCacheId();
    HCHttpCallRequestSetTimeout();
    HCHttpCallRequestSetRetryDelay();
    HCHttpCallRequestSetTimeoutWindow();
    -- HCHttpCallRequestSetRequestBodyBytes
    HCHttpCallCloseHandle();
    HCCleanup();

    test.stopTest();
end

