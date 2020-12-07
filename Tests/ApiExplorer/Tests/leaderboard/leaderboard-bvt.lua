test = require 'u-test'
common = require 'common'

function TestLeaderboardBvt_Handler()
    print("TestLeaderboardBvt_Handler")
    XblLeaderboardGetLeaderboardAsync()
end

function OnXblLeaderboardGetLeaderboardAsync()
    print('OnXblLeaderboardGetLeaderboardAsync')
    test.stopTest();
end

test.isbvt = true
test.TestLeaderboardBvt = function()
    common.init(TestLeaderboardBvt_Handler)
end

