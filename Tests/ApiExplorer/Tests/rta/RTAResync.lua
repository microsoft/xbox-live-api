test = require 'u-test'
common = require 'common'

function RTAResync_Handler()
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    XblTestHooksTriggerRTAResync()
    Sleep(5000)
    XblSocialManagerRemoveLocalUser()
    StopSocialManagerDoWorkLoop()
    test.stopTest();
end

test.RTAResync = function()
    common.init(RTAResync_Handler)
end
