test = require 'u-test'
common = require 'common'

function SimpleRTA_Handler()
    print("SimpleRTA");
    XblRealTimeActivityAddConnectionStateChangeHandler();
    -- Add a real-time handler to force RTA connection
    XblSocialAddSocialRelationshipChangedHandler();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    XblSocialRemoveSocialRelationshipChangedHandler();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    -- RTA connection will be torn down here since the last handler was removed
    test.stopTest();
end

test.SimpleRTA = function()
    common.init(SimpleRTA_Handler)
end