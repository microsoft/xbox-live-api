test = require 'u-test'
common = require 'common'

function XblAchievementsUpdateAchievementAsync_Handler()
    print("XblAchievementsUpdateAchievementAsync_Handler")
    SetCheckHR(0)
    XblAchievementsUpdateAchievementAsync()
end

function OnXblAchievementsUpdateAchievementAsync()
    local hr = GetLastError()
    SetCheckHR(1)
    if hr ~= 0 and hr ~= -2145844944 then -- -2145844944 == 0x80190130 == HTTP_E_STATUS_NOT_MODIFIED
        test.equal(hr, 0);
        print("Failure.  hr=" .. hr)
    end
    test.stopTest();
end

test.isbvt = true
test.TestXblAchievementsUpdateAchievementAsync = function()
    common.init(XblAchievementsUpdateAchievementAsync_Handler)
end
