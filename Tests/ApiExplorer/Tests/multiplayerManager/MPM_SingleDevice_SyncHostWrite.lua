test = require 'u-test'
common = require 'common'

maxRetries = 3
syncGameHostRetries = 0
syncGamePropRetries = 0

function MPM_SingleDeviceWriteTest_Handler()
    print("MPM_SingleDeviceWriteTest_Handler")
    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();
    XblMultiplayerManagerLobbySessionAddLocalUser();
end

function OnXblMultiplayerEventType_UserAdded()
    print(" ");
    print("Received OnXblMultiplayerEventType_UserAdded")
    XblMultiplayerManagerJoinGameFromLobby();
end

function OnXblMultiplayerEventType_JoinGameCompleted()
    print(" ");
    print("Received OnXblMultiplayerEventType_JoinGameCompleted")
	
    -- this call often results in a 412 due to a AdvertiseGameSessionDoWork which is writing the transfer handle to the MPSD doc.
    XblMultiplayerManagerGameSessionSetSynchronizedHost();
end

function OnXblMultiplayerEventType_SynchronizedHostWriteCompleted_412_GameSession()
    print(" ");
    print("Received OnXblMultiplayerEventType_SynchronizedHostWriteCompleted_412_GameSession")
    syncGameHostRetries = syncGameHostRetries + 1
    print("GameSessionSetSynchronizedHost Retry " .. syncGameHostRetries)
    test.assert(syncGameHostRetries <= maxRetries)
    if( syncGameHostRetries <= maxRetries ) then
        XblMultiplayerManagerGameSessionSetSynchronizedHost();
    else
        test.stopTest();
    end
end

function OnXblMultiplayerEventType_SynchronizedHostWriteCompleted()
    print(" ");
    print("Received OnXblMultiplayerEventType_SynchronizedHostWriteCompleted")
    XblMultiplayerManagerGameSessionSetSynchronizedProperties();
end

function OnXblMultiplayerEventType_SessionSynchronizedPropertyWriteCompleted_412_GameSession()
    print("Received OnXblMultiplayerEventType_SessionSynchronizedPropertyWriteCompleted_412_GameSession")
    syncGamePropRetries = syncGamePropRetries + 1
    print("GameSessionSynchronizedPropertyWriteCompleted Retry " .. syncGamePropRetries)
    test.assert(syncGamePropRetries <= maxRetries)
    if( syncGamePropRetries <= maxRetries ) then
        XblMultiplayerManagerGameSessionSetSynchronizedProperties();
    else
        test.stopTest();
    end
end

function OnXblMultiplayerEventType_GameSessionPropertyChanged()
    print("Received OnXblMultiplayerEventType_GameSessionPropertyChanged")
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

test.MPM_SingleDeviceWriteTest = function()
    common.init(MPM_SingleDeviceWriteTest_Handler)
end
