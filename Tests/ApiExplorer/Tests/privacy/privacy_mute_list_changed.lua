test = require 'u-test'
common = require 'common'

function MuteListChanged_Handler()
    print("MuteListChanged_Handler")
    XblPrivacyAddMuteListChangedHandler()
end

function OnMuteListChanged()
    print("OnMuteListChanged")
    XblPrivacyRemoveMuteListChangedHandler()
    test.stopTest()
end

test.skip = true
test.MuteListChanged = function()
    common.init(MuteListChanged_Handler)
end
