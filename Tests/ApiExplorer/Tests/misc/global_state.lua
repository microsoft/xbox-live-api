
test = require 'u-test'

-- Test that initializes XAL + XSAPI, adds a user and then shuts down
-- without calling cleanup, making sure the global state cleanup happens correctly
test.skip = true
test.globalState = function()
    XalPlatformWebSetEventHandler()
    XalPlatformStorageSetEventHandlers()
    XalInitialize()
    XblInitialize()
    XalAddUserWithUiAsync()
end

function OnXalAddUserWithUiAsync()
    test.stopTest()
end