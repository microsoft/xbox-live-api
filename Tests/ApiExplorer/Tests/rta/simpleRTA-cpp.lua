test = require 'u-test'
common = require 'common'

function RtaActivateDeactivateCpp()
    print("RtaActivateDeactivateCpp");
    RealTimeActivityServiceAddConnectionStateChangeHandler();
    RealTimeActivityServiceActivate();
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    RealTimeActivityServiceDeactivate();
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    test.stopTest();
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disabled()
    test.stopTest();
end

test.SimpleRTACpp = function()
    common.init(RtaActivateDeactivateCpp)
end