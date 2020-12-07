test = require 'u-test'
common = require 'common'

initalSubscription = true
function TestStats_Handler()
    print("TestStats_Handler")
    XblUserStatisticsTrackStatistics();
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
    if initalSubscription then
        initalSubscription = false;
    else
        test.stopTest();
    end
end

test.TestStats = function()
    common.init(TestStats_Handler)
end

