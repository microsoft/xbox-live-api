test = require 'u-test'
common = require 'common'


function testXblHttpCall_Handler()
    XblHttpCallCreate();
    XblHttpCallDuplicate();
    XblHttpCallValidateSetters();
    XblHttpCallValidateGetters();

    test.stopTest();
end

test.testXblHttpCall = function()
    print("testXblHttpCall")
    common.init(testXblHttpCall_Handler)
end
