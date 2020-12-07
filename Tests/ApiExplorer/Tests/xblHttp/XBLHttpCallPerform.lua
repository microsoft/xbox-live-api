test = require 'u-test'
common = require 'common'

function XblHttpCallPerformCompleted()
    test.stopTest();
end

function testXblHttpCallPerform_Handler()
    XblHttpCallCreate();
    XblHttpCallPerform();
end

test.testXblHttpCallPerform = function()
    print("testXblHttpCallPerform")
    common.init(testXblHttpCallPerform_Handler)
end
