test = require 'u-test'
common = require 'common'

function SimpleRTA_Handler()
    print("SimpleRTA");

    -- Test isn't valid on all platforms (i.e. on Win32 we set up a notification service subscription by default so the RTA connection
    -- won't be torn down just by removing the social relationship changed handler).
    isGdk = IsGDKPlatform()
    if isGdk then
        XblRealTimeActivityAddConnectionStateChangeHandler();
        -- Add a real-time handler to force RTA connection
        XblSocialAddSocialRelationshipChangedHandler();
    else
        test.stopTest()
    end
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    XblSocialRemoveSocialRelationshipChangedHandler();
    XblContextCloseHandle();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    -- RTA connection will be torn down here since the last handler was removed
    test.stopTest();
end

test.SimpleRTA = function()
    common.init(SimpleRTA_Handler)
end