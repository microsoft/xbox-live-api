test = require 'u-test'
common = require 'common'

function BlockListChanged_Handler()
    print("BlockListChanged_Handler")
    XblPrivacyAddBlockListChangedHandler()
end

function OnBlockListChanged()
    print("OnBlockListChanged")
    XblPrivacyRemoveBlockListChangedHandler()
    test.stopTest()
end

test.skip = true
test.BlockListChanged = function()
    common.init(BlockListChanged_Handler)
end
