test = require 'u-test'
common = require 'common'

function SocialSubUnSub_Handler()
    XblRealTimeActivityAddConnectionStateChangeHandler();
    XblPresenceAddDevicePresenceChangedHandler()
    XblPresenceAddTitlePresenceChangedHandler()
    XblRealTimeActivityActivate();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    TestLoop()
end

function OnDevicePresenceChanged()
end

function OnTitlePresenceChanged()
end

function TestLoop()
    SubscribeToTitleAndDevicePresenceChangeForFriends();
end

loopCount = 0
function OnSubscribeToTitleAndDevicePresenceChangeForFriends()
    loopCount = loopCount + 1
    if loopCount == 3 then
        Sleep(500);
        UnsubscribeToTitleAndDevicePresenceChangeForFriends();
        Sleep(500);
        test.stopTest();
    else
        print("OnSubscribeToTitleAndDevicePresenceChangeForFriends")
        Sleep(500);
        UnsubscribeToTitleAndDevicePresenceChangeForFriends();
        Sleep(500);
        TestLoop()
    end
end

test.SocialSubUnSub = function()
    common.init(SocialSubUnSub_Handler)
end
