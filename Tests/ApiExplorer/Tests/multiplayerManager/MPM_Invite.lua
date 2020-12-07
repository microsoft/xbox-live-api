test = require 'u-test'
common = require 'common'

function MPM_InviteTest_Handler()
    print("MPM_InviteTest_Handler")
    MultiDeviceSyncAndWait("MPM_InviteTest_Handler");

    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();
    XblMultiplayerManagerLobbySessionAddLocalUser();
end

function OnXGameUiShowSendGameInviteAsync()
    print("OnXGameUiShowSendGameInviteAsync");
end

function OnXblMultiplayerEventType_UserAdded()
    if MultiDeviceIsHost() then 
        XblMultiplayerManagerSetJoinability();
    else
        XblMultiplayerManagerJoinGame();
    end
end

 function OnXblMultiplayerEventType_JoinabilityStateChanged()
    if MultiDeviceIsHost() then 
        XGameUiShowSendGameInviteAsyncToMPMLobby(); -- TODO change to non-ui version
    end
 end

function OnXblMultiplayerEventType_JoinGameCompleted()
    MultiDeviceSyncAndWait("OnXblMultiplayerEventType_JoinGameCompleted");

    test.stopTest();
end

function OnXblMultiplayerEventType_MemberJoined()
    XblMultiplayerManagerGameSessionMembers();
end

test.skip = true
test.ismultidevice = true
test.MPM_InviteTest = function()
    common.init(MPM_InviteTest_Handler)
end

