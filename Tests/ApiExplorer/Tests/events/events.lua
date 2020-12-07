test = require 'u-test'
common = require 'common'

function TestWriteEvent_Handler()
    print("TestWriteEvent_Handler")
    XblEventsWriteInGameEvent()
    test.stopTest();
end

test.TestWriteEvent = function()
    common.init(TestWriteEvent_Handler)
end

