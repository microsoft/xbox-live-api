test = require 'u-test'
common = require 'common'

function AchievementsServiceUpdateAchievement_Handler()
    print("AchievementsServiceUpdateAchievement_Handler")
    SetCheckHR(0)
    AchievementsServiceUpdateAchievement()
end

function OnAchievementsServiceUpdateAchievement()
    local hr = GetLastError()
    SetCheckHR(1)
    if hr ~= 0 and hr ~= -2145844944 then -- -2145844944 == 0x80190130 == HTTP_E_STATUS_NOT_MODIFIED
        test.equal(hr, 0);
        print("Failure.  hr=" .. hr)
    end
    test.stopTest();
end

test.TestAchievementsServiceUpdateAchievementCpp = function()
    common.init(AchievementsServiceUpdateAchievement_Handler)
end
