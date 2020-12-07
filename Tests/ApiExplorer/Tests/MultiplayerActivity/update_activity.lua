test = require 'u-test'
common = require 'common'

function UpdateActivity_Handler()
    print("UpdateActivity_Handler")
    XblMultiplayerActivitySetActivityAsync()
end

function OnXblMultiplayerActivitySetActivityAsync()
    print("OnXblMultiplayerActivitySetActivityAsync")
    XblMultiplayerActivityGetActivityAsync()
end

function OnXblMultiplayerActivityGetActivityAsync()
    print("OnXblMultiplayerActivityGetActivityAsync")
    XblMultiplayerActivityDeleteActivityAsync()
end

function OnXblMultiplayerActivityDeleteActivityAsync()
    print("OnXblMultiplayerActivityDeleteActivityAsync")
    test.stopTest()
end

test.UpdateActivity = function()
    common.init(UpdateActivity_Handler)
end
