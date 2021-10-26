test = require 'u-test'
common = require 'common'

function AchievementProgressNotifications_Handler()
    print ('AchievementProgressNotifications_Handler')
    XblAchievementsAddAchievementProgressChangeHandler()
    SetCheckHR(0)
	Sleep(5000)
    XblAchievementsUpdateAchievementAsync("2", 20)
end

function OnXblAchievementsAddAchievementProgressHandler()
    print ('OnXblAchievementsAddAchievementProgressHandler')
    XblAchievementsRemoveAchievementProgressChangeHandler()
    test.stopTest();
end

function OnXblAchievementsUpdateAchievementAsync()
    print ("OnXblAchievementsUpdateAchievementAsync")
    local hr = GetLastError()
    SetCheckHR(1)
	if hr == -2145844944 then -- -2145844944 == 0x80190130 == HTTP_E_STATUS_NOT_MODIFIED
		print("Achievement not modified.")
		XblAchievementsRemoveAchievementProgressChangeHandler()
		test.stopTest()
    elseif hr ~= 0 then 
        test.equal(hr, 0);
        print("Failure.  hr=" .. hr)
        test.stopTest()
    else
	    print ("Waiting to receive notification.")
	end
end

test.skip = true;
test.TestAchievementProgressNotification = function()
    common.init(AchievementProgressNotifications_Handler)
end