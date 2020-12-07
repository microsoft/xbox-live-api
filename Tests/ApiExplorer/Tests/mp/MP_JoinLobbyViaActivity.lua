test = require 'u-test'
common = require 'common'

function MP_JoinLobbyViaActivity_Handler()
    print("MP_JoinLobbyViaActivity")
    MultiDeviceSyncAndWait("MP_JoinLobbyViaActivity");

    XblRealTimeActivityActivate()
    XblMultiplayerSetSubscriptionsEnabled()
    XblMultiplayerAddSessionChangedHandler()
    XblMultiplayerAddSubscriptionLostHandler()

    if MultiDeviceIsHost() > 0 then 
        XblMultiplayerSessionCreateHandle()
        XblMultiplayerSessionJoin()
        XblMultiplayerSessionSetClosed(0)
        XblMultiplayerSessionPropertiesSetJoinRestriction();
        XblMultiplayerSessionPropertiesSetReadRestriction();
        XblMultiplayerWriteSessionAsync()
    else
        MultiDeviceSyncAndWait("SessionCreated");
        print("Joining lobby of remote player. NOTE: Ensure remote is a friend or join will fail")
        XblMultiplayerGetActivitiesForUsersAsync();
    end
end

function OnXblMultiplayerGetActivitiesForUsersAsync()
    if MultiDeviceIsHost() > 0 then 
    else
        print("Calling XblMultiplayerWriteSessionByHandleAsync");
        XblMultiplayerSessionCreateHandle()
        XblMultiplayerSessionJoin()
        XblMultiplayerWriteSessionByHandleAsync()
    end
end

function OnXblMultiplayerWriteSessionByHandleAsync()
    MultiDeviceSyncAndWait("SessionJoined");
    XblPresenceGetPresenceAsync()
end

function OnXblMultiplayerWriteSessionAsync()
    if MultiDeviceIsHost() > 0 then 
        XblMultiplayerSessionSessionReference();
        XblMultiplayerSetActivityAsync()
    end
end

function OnXblMultiplayerSetActivityAsync()
    if MultiDeviceIsHost() > 0 then 
        MultiDeviceSyncAndWait("SessionCreated");
        MultiDeviceSyncAndWait("SessionJoined");
        XblPresenceGetPresenceAsync()
    end
end

function OnXblPresenceGetPresenceAsync()
    XblPresenceRecordGetXuid()
    XblPresenceRecordGetDeviceRecords()
    XblPresenceRecordGetUserState()
    XblPresenceRecordCloseHandle()
    test.stopTest();
end

test.ismultidevice = true
test.MP_JoinLobbyViaActivity = function()
    common.init(MP_JoinLobbyViaActivity_Handler)
end

