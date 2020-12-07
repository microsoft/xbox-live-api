
test = require 'u-test'
common = require 'common'

test.testWebsocketCloseHandleWhileConnected = function()
    print("testWebsocketCloseHandleWhileConnected")

    HCInitialize();
    HCWebSocketCreate();
    HCWebSocketConnectAsync();
end

function OnHCWebSocketConnectAsync()
    print("OnHCWebSocketConnectAsync")
    HCWebSocketCloseHandle();
    test.stopTest();
end