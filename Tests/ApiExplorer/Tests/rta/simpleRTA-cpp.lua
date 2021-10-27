test = require 'u-test'
common = require 'common'

function RtaActivateDeactivateCpp()
    print("RtaActivateDeactivateCpp");

    -- Test isn't valid on all platforms (i.e. on Win32 we set up a notification service subscription by default so the RTA connection
    -- won't be torn down just by removing the social relationship changed handler).
    isGdk = IsGDKPlatform()
    if isGdk then
        RealTimeActivityServiceAddConnectionStateChangeHandler();
        RealTimeActivityServiceActivate();
    else
        test.stopTest()
    end
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