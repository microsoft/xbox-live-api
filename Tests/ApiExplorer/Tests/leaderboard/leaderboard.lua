test = require 'u-test'
common = require 'common'

function TestLeaderboard_Handler()
    print("TestLeaderboard_Handler")
    XblLeaderboardGetLeaderboardAsync("TotalPuzzlesSolvedLB")
end

local leaderboardQueryCompleteCount = 0
function OnXblLeaderboardGetLeaderboardAsync()
    print('OnXblLeaderboardGetLeaderboardAsync')
    hasNext, hr = XblLeaderboardResultHasNext()
    if hasNext == 0 then print("hasNext: false"); else print("hasNext: true"); end
    print("hr " .. hr)
    if hasNext ~= 0 then
        XblLeaderboardResultGetNextAsync()
    else
        leaderboardQueryCompleteCount = leaderboardQueryCompleteCount + 1
        if leaderboardQueryCompleteCount == 2 then
            test.stopTest();
        else
            XblLeaderboardGetLeaderboardAsync("", "TotalPuzzlesSolved")
        end
    end
end

function OnXblLeaderboardResultGetNextAsync()
    print("OnXblLeaderboardResultGetNextAsync")
    OnXblLeaderboardGetLeaderboardAsync()
end

test.TestLeaderboard = function()
    common.init(TestLeaderboard_Handler)
end

