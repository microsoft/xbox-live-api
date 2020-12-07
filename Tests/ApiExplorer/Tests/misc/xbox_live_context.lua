
test = require 'u-test'
common = require 'common'

function TestXblContextCreate_Handler()
    XblAddServiceCallRoutedHandler()
    XblRemoveServiceCallRoutedHandler()
    XblContextSettingsGetLongHttpTimeout()
    XblContextSettingsSetLongHttpTimeout()
    XblContextSettingsGetHttpRetryDelay()
    XblContextSettingsSetHttpRetryDelay()
    XblContextSettingsGetHttpTimeoutWindow()
    XblContextSettingsSetHttpTimeoutWindow()
    XblContextSettingsGetWebsocketTimeoutWindow()
    XblContextSettingsSetWebsocketTimeoutWindow()
    XblContextSettingsGetUseCrossPlatformQosServers()
    XblContextSettingsSetUseCrossPlatformQosServers()
    test.stopTest();
end

test.TestXblContextCreate = function()
    common.init(TestXblContextCreate_Handler)
end
