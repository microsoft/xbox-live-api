test = require 'u-test'
common = require 'common'

function TestLeaderboardCpp_Handler()
    print("TestLeaderboardCpp_Handler")
    LeaderboardServiceGetLeaderboard()
end

function OnLeaderboardServiceGetLeaderboard()
    print('OnLeaderboardServiceGetLeaderboard')
    hasNext, hr = LeaderboardResultHasNextCpp()
    if hasNext == 0 then print("hasNext: false"); else print("hasNext: true"); end
    print("hr " .. hr)
    if hasNext ~= 0 then
        LeaderboardResultGetNextCpp()
    else
        test.stopTest();
    end
end

function OnLeaderboardResultGetNextCpp()
    print("OnLeaderboardResultGetNextCpp")
	test.stopTest();
end

test.TestLeaderboardCpp = function()
    common.init(TestLeaderboardCpp_Handler)
end

