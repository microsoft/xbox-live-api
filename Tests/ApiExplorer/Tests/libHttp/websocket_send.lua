
test = require 'u-test'
common = require 'common'

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