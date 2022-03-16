test = require 'u-test'
common = require 'common'

function RTASuspendResume_Handler()
    isGdk = IsGDKPlatform()
    if isGdk then
        XblRealTimeActivityAddConnectionStateChangeHandler();
        -- Add a real-time handler to force RTA connection
        XblSocialAddSocialRelationshipChangedHandler();
    else
        test.stopTest()
    end
end

connectCount = 0
function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    connectCount = connectCount + 1

    if connectCount == 1 then
        --HCWinHttpSuspend();
    else
        --XblRealTimeActivityRemoveConnectionStateChangeHandler();
        --XblSocialRemoveSocialRelationshipChangedHandler();
        test.stopTest()
    end
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    HCWinHttpResume();
end

test.skip = true;
test.RTASuspendResume = function()
    common.init(RTASuspendResume_Handler)
end