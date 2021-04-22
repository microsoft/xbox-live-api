test = require 'u-test'
common = require 'common'

-- Note: This test requires an account with achievement id 1 locked, and can only be run 
-- one time on that account as we currently have no way to programmatically reset the unlock
-- status of an achievement from API Explorer. To successfully run the test again you will have 
-- to run the XblPlayerDataReset tool to reset the user or sign in with a different account.

function achievement_unlock_notification_test()
    print("achievement_unlock_notification_test")
	
	local achievementId = "1";

	local isLocked = IsAchievementLocked(achievementId);

	-- make sure there is something to do
	test.is_true(isLocked);
	
	if not isLocked then
		print("achievement is already unlocked");
		test.stopTest();
		return;
	end

    -- register achievement unlock handler invite handler
    local id = XblAchievementUnlockAddNotificationHandler()
    XblGameInviteAddNotificationHandler()

	-- unlock achievement
	RunAchievementUnlock(achievementId);
	
	local status;

	for i=1,10 do
		Sleep(500);
		status = CheckStatus();
		if status == 0 then
			break;
		end
	end

	-- anything other than 0 is an error
	test.equal(status,0);
	
	-- unregister handlers
	XblAchievementUnlockRemoveNotificationHandler(id);
    XblGameInviteRemoveNotificationHandler()

	test.stopTest();
end


function achievement_unlock_notification_test_end()
    print("Ending test");
    test.stopTest();
end


test.skip = true; -- skipping because of special account requirements
test.TestAchievementUnlockNotification = function()
    common.init(achievement_unlock_notification_test)
end
