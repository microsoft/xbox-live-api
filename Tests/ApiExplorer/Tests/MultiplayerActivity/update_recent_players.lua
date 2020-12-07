test = require 'u-test'
common = require 'common'

function UpdateRecentPlayers_Handler()
    print("UpdateRecentPlayers_Handler")
    XblMultiplayerActivityUpdateRecentPlayers(0)
    XblMultiplayerActivityUpdateRecentPlayers(1)
    XblMultiplayerActivityUpdateRecentPlayers(2)
    XblMultiplayerActivityFlushRecentPlayersAsync()
end

function OnXblMultiplayerActivityFlushRecentPlayersAsync()
    print("OnXblMultiplayerActivityFlushRecentPlayersAsync")
    test.stopTest();
end

test.UpdateRecentPlayers = function()
    common.init(UpdateRecentPlayers_Handler)
end