test = require 'u-test'
common = require 'common'

function GetAchievementsForTitleIdCpp_Handler()
    print("GetAchievementsForTitleIdCpp_Handler")
    AchievementsServiceGetAchievementsForTitleId()
end

function OnAchievementsServiceGetAchievementsForTitleId()
    hr, hasNext = AchievementsResultHasNextCpp()
    print("hasNext " .. hr)
    print("hr " .. hr)
    if hasNext ~= 0 then
        AchievementsResultGetNextCpp()
    else
        test.stopTest();
    end
end

function OnAchievementsResultGetNextCpp()
    print("OnAchievementsResultGetNextCpp")
    OnAchievementsServiceGetAchievementsForTitleId()
end

test.TestGetAchievementsForTitleCpp = function()
    common.init(GetAchievementsForTitleIdCpp_Handler)
end