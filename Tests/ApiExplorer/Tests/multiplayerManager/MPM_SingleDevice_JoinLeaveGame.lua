test = require 'u-test'
common = require 'common'

function MPM_SingleDevice_JoinLeaveGame_Handler()
    print("MPM_SingleDevice_JoinLeaveGame_Handler")
    XblMultiplayerManagerInitialize();
    XblMultiplayerManagerLobbySessionAddLocalUser();
    XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress("1234567890");
    StartDoWorkLoop();
end

function OnXblMultiplayerEventType_UserAdded()
    print(" ");
    print("Received OnXblMultiplayerEventType_UserAdded")
    XblMultiplayerManagerLobbySessionCorrelationId();
    XblMultiplayerManagerLobbySessionLocalMembers();
    XblMultiplayerManagerJoinability();

    XblMultiplayerManagerJoinGameFromLobby();
end

function OnXblMultiplayerEventType_JoinGameCompleted()
    print(" ");
    print("Received OnXblMultiplayerEventType_JoinGameCompleted")

    XblMultiplayerManagerGameSessionActive()
    XblMultiplayerManagerGameSessionCorrelationId();
    XblMultiplayerManagerGameSessionMembers();
    XblMultiplayerManagerGameSessionConstants();
    XblMultiplayerManagerGameSessionSessionReference();

    XblMultiplayerManagerLeaveGame();
end

function OnXblMultiplayerEventType_LeaveGameCompleted()
    print(" ");
    print("Recived OnXblMultiplayerEventType_LeaveGameCompleted")
    XblMultiplayerManagerLobbySessionRemoveLocalUser();
end

function OnXblMultiplayerEventType_UserRemoved()
    print(" ");
    print("Received OnXblMultiplayerEventType_UserRemoved")
    test.stopTest();
end

test.MPM_SingleDevice_JoinLeaveGame = function()
    common.init(MPM_SingleDevice_JoinLeaveGame_Handler)
end
