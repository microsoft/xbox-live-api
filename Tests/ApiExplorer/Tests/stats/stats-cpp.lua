test = require 'u-test'
common = require 'common'

initalSubscription = true
callDeactivate = false

function TestStatsCpp_Handler()
    print("TestStatsCpp_Handler")
    SetCallUpdate();
    RealTimeActivityServiceAddConnectionStateChangeHandler();
    RealTimeActivityServiceActivate();
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Connected()
    print("RTA connection connected");
    
    UserStatisticsServiceSubscribeToStatisticChange();
    UserStatisticsServiceAddStatisticChangedHandler();
    UserStatisticsServiceGetSingleUserStatistic();
end

function OnUserStatisticsServiceGetSingleUserStatistic()
    print("OnUserStatisticsServiceGetSingleUserStatistic")
    UserStatisticsServiceGetSingleUserStatistics();
end

function OnUserStatisticsServiceGetSingleUserStatistics()
    print("OnUserStatisticsServiceGetSingleUserStatistics")
    UserStatisticsServiceGetMultipleUserStatistics();
end

function OnUserStatisticsServiceGetMultipleUserStatistics()
    print("OnUserStatisticsServiceGetMultipleUserStatistics")
    UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations();
end

function OnUserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations()
    print("OnUserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations")
    EventsServiceWriteInGameEvent();
    initalSubscription = false;
end

function OnStatisticChangedHandlerCpp()
    print("OnStatisticChangedHandlerCpp")
    StatisticChangeSubscriptionGetStateCpp();
    StatisticChangeSubscriptionGetIdCpp();
    if initalSubscription then
        initalSubscription = false;
    else
        callDeactivate = true;
    end
end

function update()
    if callDeactivate then
        UserStatisticsServiceRemoveStatisticChangedHandler();
        UserStatisticsServiceUnsubscribeFromStatisticChange();
        RealTimeActivityServiceDeactivate();
    end
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disconnected()
    print("RTA connection disconnected");
    test.stopTest();
end

function OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disabled()
    test.stopTest();
end

test.TestStatsCpp = function()
    common.init(TestStatsCpp_Handler)
end