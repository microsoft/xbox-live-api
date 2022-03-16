
test = require 'u-test'
common = require 'common'

-- Requires manually running local echo server
test.skip = true
test.websocketCleanupWhileConnected = function()
    print("websocketCleanupWhileConnected")

    HCInitialize();
    HCWebSocketCreate();
    HCWebSocketConnectAsync();
end

function OnHCWebSocketConnectAsync()
    print("OnHCWebSocketConnectAsync")
    HCCleanupAsync();
end

function OnHCWebsocketClosed()
    print("OnHCWebsocketClosed");  
    HCWebSocketCloseHandle();
end

function OnHCCleanupAsync()
    print("OnHCCleanupAsync");
    test.stopTest();
end