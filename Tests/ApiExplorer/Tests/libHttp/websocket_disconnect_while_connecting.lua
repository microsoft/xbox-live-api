
test = require 'u-test'
common = require 'common'

--Bug 35915673 Disabling websocket tests due to websocket.org no longer in service
test.skip = true
test.testWebsocketDisconnectWhileConnecting = function()
    print("testWebsocketDisconnectWhileConnecting")

    HCInitialize();
    HCWebSocketCreate();
    HCWebSocketConnectAsync();
	SetCheckHR(0);
    HCWebSocketDisconnect();
	local hr = GetLastError();
	SetCheckHR(1);
	if hr ~= -2147418113 then -- -2147418113 == 0x8000FFFF == E_UNEXPECTED
	    test.equal(hr, 0);
        print("Failure.  hr=" .. hr)
    end
    HCWebSocketCloseHandle();
end

function OnHCWebSocketConnectAsync()
    test.stopTest();
end