test = require 'u-test'
common = require 'common'

-- Test ensuring legacy RTA calling patters are still functional

function SimpleRTALegacy_Handler()
    print("SimpleRTALegacy_Handler");

    -- Test isn't valid on all platforms (i.e. on Win32 we set up a notification service subscription by default so the RTA connection
    -- won't be torn down just by removing the social relationship changed handler).
    isGdk = IsGDKPlatform()
    if isGdk then
        XblRealTimeActivityAddConnectionStateChangeHandler();
        XblRealTimeActivityActivate();
    else
        test.stopTest()
    end
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