
test = require 'u-test'
common = require 'common'

function TestGameMpaInviteSend_Handler()
    XblMultiplayerActivitySetActivityAsync()

    print("LUA: calling XGameInviteRegisterForEvent")
    XGameInviteRegisterForEvent();
    print("LUA: called XGameInviteRegisterForEvent")
end

function OnXblMultiplayerActivitySetActivityAsync()
    XGameUiShowMultiplayerActivityGameInviteAsync();
end

function OnXGameUiShowMultiplayerActivityGameInviteAsync()
    print("XGameUiShowMultiplayerActivityGameInviteAsync completed");
end


function OnXGameInviteRegisterForEvent()
    print("LUA: OnXGameInviteRegisterForEvent");
end

test.skip = true
test.TestGameMpaInviteSend = function()
    common.init(TestGameMpaInviteSend_Handler)
end
