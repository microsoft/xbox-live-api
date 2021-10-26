
test = require 'u-test'
common = require 'common'

--Bug 35915673 Disabling websocket tests due to websocket.org no longer in service
test.skip = true
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