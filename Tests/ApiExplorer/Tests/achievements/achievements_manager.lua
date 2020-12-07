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

function AchievementsManager_Handler()
    print("AchievementsManager_Handler")
    StartAchievementsManagerDoWorkLoop()
    XblAchievementsManagerAddLocalUser()
end

function OnXblAchievementsManagerDoWork_LocalUserAddedEvent()
    print("OnXblAchievementsManagerDoWork_LocalUserAddedEvent")
    handle, hr = XblAchievementsManagerGetAchievement("1")
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements(handle)
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    
    handle, hr = XblAchievementsManagerGetAchievements()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements(handle)
    if count ~= 2 then
       test.equal(hr, 0);
       print("Failure. Number of achievements found (" .. count .. ") different from expected (2). hr=" .. hr)
       test.stopTest()
    end
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    handle, hr = XblAchievementsManagerGetAchievements()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements()
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)
    
    handle, hr = XblAchievementsManagerGetAchievementsByState()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements()
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    XblAchievementsManagerRemoveLocalUser()
    StopAchievementsManagerDoWorkLoop()
    test.stopTest()
end

function OnXblAchievementsManagerDoWork_AchievementProgressUpdatedEvent()
    print("OnXblAchievementsManagerDoWork_AchievementProgressUpdatedEvent")
end

function OnXblAchievementsManagerDoWork_AchievementUnlockedEvent()
    print("OnXblAchievementsManagerDoWork_AchievementUnlockedEvent")
end

test.isbvt = true
test.TestAchievementsManager = function()
    common.init(AchievementsManager_Handler)
end