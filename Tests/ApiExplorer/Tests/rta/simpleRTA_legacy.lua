test = require 'u-test'
common = require 'common'

-- Test ensuring legacy RTA calling patters are still functional

function SimpleRTALegacy_Handler()
    print("SimpleRTALegacy_Handler");
    XblRealTimeActivityAddConnectionStateChangeHandler();
    XblRealTimeActivityActivate();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    XblRealTimeActivityDeactivate();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    test.stopTest();
end

test.SimpleRTALegacy = function()
    common.init(SimpleRTALegacy_Handler)
end