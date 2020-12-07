
test = require 'u-test'
common = require 'common'

function TestGameInviteSend_Handler()
    XblMultiplayerSessionCreateHandle()
    XblMultiplayerSessionJoin()
    XblMultiplayerWriteSessionAsync()
end

function OnXblMultiplayerWriteSessionAsync()
    XblMultiplayerSessionReferenceCreate();
    XblMultiplayerSetActivityAsync();
end

function OnXblMultiplayerSetActivityAsync()
    XGameUiShowSendGameInviteAsync();
end

function OnXGameUiShowSendGameInviteAsync()
    test.stopTest();
end

test.skip = true
test.TestGameInviteSend = function()
    common.init(TestGameInviteSend_Handler)
end
