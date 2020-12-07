test = require 'u-test'
common = require 'common'

function GetAchievementsForTitleId_Handler()
    print("GetAchievementsForTitleId_Handler")
    XblAchievementsGetAchievementsForTitleIdAsync()
end

function OnXblAchievementsGetAchievementsForTitleIdAsync()
    XblAchievementsResultGetAchievements()
    hr, hasNext = XblAchievementsResultHasNext()
    print("hasNext " .. hr)
    print("hr " .. hr)
    if hasNext ~= 0 then
        XblAchievementsResultGetNextAsync()
    else
        XblAchievementsResultCloseHandle();
        test.stopTest();
    end
end

function OnXblAchievementsResultGetNextAsync()
    print("OnXblAchievementsResultGetNextAsync")
    XblAchievementsGetAchievementsForTitleIdAsync()
end

test.isbvt = true
test.TestGetAchievementsForTitle = function()
    common.init(GetAchievementsForTitleId_Handler)
end
