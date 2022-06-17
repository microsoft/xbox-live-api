test = require 'u-test'
common = require 'common'

function TestGameInviteNotifications_Handler()
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    print("OnXblSocialManagerDoWork_LocalUserAddedEvent")
    Sleep(5000)
    XblSocialManagerRemoveLocalUser()
    StopSocialManagerDoWorkLoop()
    --print("stopping test")
    --test.stopTest();
end

test.skip = true;
test.TestMultipleNotificationSubscriptions = function()
    common.init(TestGameInviteNotifications_Handler)
end