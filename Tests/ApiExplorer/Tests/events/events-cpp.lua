test = require 'u-test'
common = require 'common'

function TestWriteEventCpp_Handler()
    print("TestWriteEventCpp_Handler")
    EventsServiceWriteInGameEvent()
    test.stopTest();
end

test.TestWriteEventCpp = function()
    common.init(TestWriteEventCpp_Handler)
end