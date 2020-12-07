test = require 'u-test'
common = require 'common'

function SetActivity_Handler()
    XblMultiplayerActivitySetActivityAsync()
end

function OnXblMultiplayerActivitySetActivityAsync()
    print("SetActivity call complete")
end

test.skip = true
test.SetActivity = function()
    common.init(SetActivity_Handler)
end