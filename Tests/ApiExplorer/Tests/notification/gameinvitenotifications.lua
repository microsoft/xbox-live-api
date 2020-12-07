test = require 'u-test'
common = require 'common'

function TestGameInviteNotifications_Handler()
    print("TestGameInviteNotifications_Handler")
    MultiDeviceSyncAndWait("TestGameInviteNotifications");
    
    if MultiDeviceIsHost() > 0 then 
        XblGameInviteRegisterForEventAsync()
    else
        MultiDeviceSyncAndWait("SendInvite");
        XblMultiplayerSessionReferenceCreate();
        session, hr = XblMultiplayerSessionCreateHandle("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 0)
        XblMultiplayerSessionJoin(session)
        XblMultiplayerWriteSessionAsync(session)
    end    
end

-- First Device - Notification Receiver --
function OnXblGameInviteRegisterForEventAsync()
    print("OnXblGameInviteRegisterForEventAsync")
    XblGameInviteAddNotificationHandler();
    MultiDeviceSyncAndWait("SendInvite");
end

function OnXblGameInviteAddNotificationHandler()
    print("OnXblGameInviteAddNotificationHandler")
    MultiDeviceSyncAndWait("WaitForInvite");
    XblGameInviteRemoveNotificationHandler();
    XblGameInviteUnregisterForEventAsync();
end

function OnXblGameInviteUnregisterForEventAsync()
    print("OnXblGameInviteUnregisterForEventAsync")
    test.stopTest();
end

-- Second Device - Invite Sender --
function OnXblMultiplayerWriteSessionAsync()
    XblMultiplayerSendInvitesAsync("", "", MultiDeviceGetRemoteXuid())
end

function OnXblMultiplayerSendInvitesAsync()
    MultiDeviceSyncAndWait("WaitForInvite");
    test.stopTest();
end

test.ismultidevice = true;
test.TestGameInviteNotifications = function()
    common.init(TestGameInviteNotifications_Handler)
end

