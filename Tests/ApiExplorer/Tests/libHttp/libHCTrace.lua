
test = require 'u-test'
common = require 'common'

test.testlibHCTrace = function()
    print("test LHC trace")

    HCInitialize();

    HCTrace();
    HCTraceLarge();

    HCCleanup();

    test.stopTest();
end

