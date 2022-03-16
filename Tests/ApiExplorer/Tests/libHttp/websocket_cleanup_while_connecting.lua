
test = require 'u-test'
common = require 'common'

-- Requires manually running local echo server
test.skip = true
test.websocketCleanupWhileConnecting = function()
    print("websocketCleanupWhileConnecting")

    HCInitialize();
    HCWebSocketCreate();
    HCWebSocketConnectAsync();
    HCCleanupAsync();
end

function OnHCWebSocketConnectAsync()
    print("OnHCWebSocketConnectAsync")
    HCWebSocketCloseHandle();
end

function OnHCWebsocketClosed()
    print("OnHCWebsocketClosed");  
end

function OnHCCleanupAsync()
    print("OnHCCleanupAsync");
    test.stopTest();
end