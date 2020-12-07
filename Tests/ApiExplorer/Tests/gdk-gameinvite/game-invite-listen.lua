
test = require 'u-test'
common = require 'common'

function TestGameInvite_Handler()
    print("LUA: calling XGameInviteRegisterForEvent")
    XGameInviteRegisterForEvent();
    print("LUA: called XGameInviteRegisterForEvent")
end

function OnXGameInviteRegisterForEvent()
    print("LUA: OnXGameInviteRegisterForEvent");
    XblMultiplayerGetSessionByHandleAsync();
end

test.skip = true
test.TestGameInvite = function()
    common.init(TestGameInvite_Handler)
end
