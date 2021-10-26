
test = require 'u-test'
common = require 'common'

--Bug 35915673 Disabling websocket tests due to websocket.org no longer in service
test.skip = true
test.testWebsocketSend = function()
    print("testWebsocketSend")

    HCInitialize();
    HCWebSocketCreate();
    HCWebSocketConnectAsync();
end

function OnHCWebSocketConnectAsync()
    print("OnHCWebSocketConnectAsync")
    HCWebSocketSendMessageAsync();
end

function OnHCWebsocketMessageReceived()
    HCWebSocketDisconnect();
end

function OnHCWebsocketClosed()
    HCWebSocketCloseHandle();
    HCCleanup();
    test.stopTest();
end