test = require 'u-test'
common = require 'common'

function TestPresenceRTA_Handler()
    print("TestPresenceRTA_Handler")
    XblPresenceTrackUsers(2814656696817462)
    XblPresenceAddDevicePresenceChangedHandler()
    XblPresenceAddTitlePresenceChangedHandler()()
end

function OnDevicePresenceChanged()
    print("DevicePresenceChanged")
end

function OnTitlePresenceChanged()
    print("TitlePresenceChanged")
end

test.skip = true
test.TestPresenceRTA = function()
    common.init(TestPresenceRTA_Handler)
end