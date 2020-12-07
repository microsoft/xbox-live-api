test = require 'u-test'
common = require 'common'

function TestLeaderboard2017_Handler()
    print("TestLeaderboard_Handler")
    XblTitleManagedStatsWriteAsync()
end

function OnXblTitleManagedStatsWriteAsync()
    XblLeaderboardGetLeaderboardAsync("", "TestStat1", "XblSocialGroupType::People", "XblLeaderboardQueryType::TitleManagedStatBackedGlobal")
end

local leaderboardQueryCompleteCount = 0
function OnXblLeaderboardGetLeaderboardAsync()
    leaderboardQueryCompleteCount = leaderboardQueryCompleteCount + 1
    if leaderboardQueryCompleteCount == 2 then
        test.stopTest()
    else
        XblLeaderboardGetLeaderboardAsync("", "TestStat1", "XblSocialGroupType::People", "XblLeaderboardQueryType::TitleManagedStatBackedSocial")
    end
end

test.skip = true
test.TestLeaderboard2017 = function()
    common.init(TestLeaderboard2017_Handler)
end