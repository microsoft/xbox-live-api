test = require 'u-test'
common = require 'common'

-- Test ensuring legacy RTA calling patters are still functional

initalSubscription = true
callDeactivate = false
function TestStatsLegacy_Handler()
    print("TestStatsLegacy_Handler")
    SetCallUpdate();
    XblRealTimeActivityAddConnectionStateChangeHandler();
    XblRealTimeActivityActivate();
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    
    XblUserStatisticsSubscribeToStatisticChange();
    XblUserStatisticsAddStatisticChangedHandler();
    
    XblUserStatisticsGetSingleUserStatisticAsync();
end

function OnXblUserStatisticsGetSingleUserStatisticAsync()
    print("OnXblUserStatisticsGetSingleUserStatisticAsync")
    XblUserStatisticsGetSingleUserStatisticsAsync();
end

function OnXblUserStatisticsGetSingleUserStatisticsAsync()
    print("OnXblUserStatisticsGetSingleUserStatisticsAsync")
    XblUserStatisticsGetMultipleUserStatisticsAsync();
end

function OnXblUserStatisticsGetMultipleUserStatisticsAsync()
    print("OnXblUserStatisticsGetMultipleUserStatisticsAsync")
    XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync();
end

function OnXblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync()
    print("OnXblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync")
    XblEventsWriteInGameEvent();
    initalSubscription = false;
end

function OnStatisticChangedHandler()
    print("OnStatisticChangedHandler")
    XblRealTimeActivitySubscriptionGetState();
    XblRealTimeActivitySubscriptionGetId();
    if initalSubscription then
        initalSubscription = false;
    else
        callDeactivate = true;
    end
end

function update()
    if callDeactivate then
        XblUserStatisticsRemoveStatisticChangedHandler();
        XblUserStatisticsUnsubscribeFromStatisticChange();
        XblRealTimeActivityDeactivate();
    end
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    test.stopTest();
end

test.TestStatsLegacy = function()
    common.init(TestStatsLegacy_Handler)
end