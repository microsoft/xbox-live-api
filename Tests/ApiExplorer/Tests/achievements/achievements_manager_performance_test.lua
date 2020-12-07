test = require 'u-test'
common = require 'common'

local function VerifySuccess(hr)
    SetCheckHR(1)
    if hr ~= 0 then
        test.equal(hr, 0)
        print("Failure. hr=" .. hr)
        test.stopTest()
    end
    return true
end

function AchievementsManagerPerformance_Handler( ... )
    print("AchievementsManagerPerformance_Handler")
    -- start test
    SetupAchievementsManagerPerformanceTestMock()
    StartAchievementsManagerDoWorkLoop()
    XblAchievementsManagerAddLocalUser()
end

function OnXblAchievementsManagerDoWork_LocalUserAddedEvent()
    print("OnXblAchievementsManagerDoWork_LocalUserAddedEvent")
    -- call get achievements performance test

    handle, hr = XblAchievementsManagerGetAchievementsPerfTest()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements(handle)
    if count ~= 100 then
       test.equal(hr, 0);
       print("Failure. Number of achievements found (" .. count .. ") different from expected (100). hr=" .. hr)
       test.stopTest()
    end
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    XblAchievementsManagerRemoveLocalUser()
    StopAchievementsManagerDoWorkLoop()
    test.stopTest()    
end


test.isbvt = true
test.TestAchievementsManager = function()
    common.init(AchievementsManagerPerformance_Handler)
end