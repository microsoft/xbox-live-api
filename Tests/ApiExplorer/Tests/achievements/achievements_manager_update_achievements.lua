test = require 'u-test'
common = require 'common'

-- Note: This test requires an account with achievement id 2 locked with no progress, and can only be run 
-- one time on that account as we currently have no way to programmatically reset the unlock
-- status of an achievement from API Explorer. To successfully run the test again you will have 
-- to run the XblPlayerDataReset tool to reset the user or sign in with a different account.

local function VerifySuccess(hr)
    SetCheckHR(1)
    if hr ~= 0 then
        test.equal(hr, 0)
        print("Failure. hr=" .. hr)
        test.stopTest()
    end
    return true
end

local unlockeReceived = false
local updateReceived = 0
local originalUnlockCount = 0
local testUpdate = true -- if false, then Unlock is being tested

local function IsUpdateTestCompleted()
    if updateReceived >= 2 and testUpdate then
        updateReceived = 0
        return true
    end
    return false
end

local function IsUnlockTestCompleted()
    if unlockReceived and updateReceived >= 1 and not testUpdate then
        unlockReceived = false
        updateReceived = 0
        return true
    end
    return false
end

-- Order of unlock and update events getting processed isn't guaranteed, so moving
-- the test completion logic to a separate function.
local function EndTestIfUnlockComplete()
    if IsUnlockTestCompleted() then
        XblAchievementsManagerRemoveLocalUser()
        StopAchievementsManagerDoWorkLoop()
        test.stopTest()
    end
end

function AchievementsManagerUpdateAchievements_Handler()
    print("AchievementsManagerUpdateAchievements_Handler")
    StartAchievementsManagerDoWorkLoop()
    XblAchievementsManagerAddLocalUser()
end

function OnXblAchievementsManagerDoWork_LocalUserAddedEvent()
    print("OnXblAchievementsManagerDoWork_LocalUserAddedEvent")
    handle, hr = XblAchievementsManagerGetAchievementsByState()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements()
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    originalUnlockCount = count
    testUpdate = true
    
    XblAchievementsManagerUpdateAchievement("2", 90)
    XblAchievementsManagerUpdateAchievement("1", 50)
end

function OnXblAchievementsManagerDoWork_AchievementProgressUpdatedEvent()
    print("OnXblAchievementsManagerDoWork_AchievementProgressUpdatedEvent")
    handle, hr = XblAchievementsManagerGetAchievement("2")
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements()
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    updateReceived = updateReceived + 1
    
    if IsUpdateTestCompleted() then
        updateReceived = 0
        testUpdate = false
        XblAchievementsManagerUpdateAchievement("2", 100)
    end
    EndTestIfUnlockComplete()
end

function OnXblAchievementsManagerDoWork_AchievementUnlockedEvent()
    print("OnXblAchievementsManagerDoWork_AchievementUnlockedEvent")
    handle, hr = XblAchievementsManagerGetAchievementsByState()
    VerifySuccess(hr)
    count, hr = XblAchievementsManagerResultGetAchievements()
    VerifySuccess(hr)
    XblAchievementsManagerResultCloseHandle(handle)

    unlockReceived = true
    if count ~= originalUnlockCount + 1 then
        test.equal(count, originalUnlockCount + 1)
        print("Failure. Number of unlocked achievements (" .. count .. ") differs from expected (" .. originalUnlockCount + 1 .. ").")
        test.stopTest()
    end
    EndTestIfUnlockComplete()
end

test.skip = true
test.TestAchievementsManager = function()
    common.init(AchievementsManagerUpdateAchievements_Handler)
end
