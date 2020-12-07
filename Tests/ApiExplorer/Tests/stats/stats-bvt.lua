test = require 'u-test'
common = require 'common'

userStatistic1 = 0;
changeHandleCalledCount = 0;
userStatResultCount = 0;

function TestStatsBvt_Handler()
    print("TestStatsBvt_Handler")

    XblUserStatisticsTrackStatistics();
    XblUserStatisticsAddStatisticChangedHandler();
    XblUserStatisticsGetSingleUserStatisticAsync();
end

function OnXblUserStatisticsGetSingleUserStatisticAsync()
    print("OnXblUserStatisticsGetSingleUserStatisticAsync")

    userStatResultCount = userStatResultCount + 1
    if userStatResultCount == 1 then

        userStatistic1 = GetLastStat();
        print("UserStatistic1 " .. userStatistic1 );

        XblEventsWriteInGameEvent();

    elseif userStatResultCount == 2 then

        local userStatistic2 = GetLastStat();
        print("UserStatistic1 " .. userStatistic1 );
        print("UserStatistic2 " .. userStatistic2 );
        test.assert(userStatistic2 > userStatistic1);
        test.stopTest();
    end
end

function OnStatisticChangedHandler()
    print("OnStatisticChangedHandler")

    changeHandleCalledCount = changeHandleCalledCount + 1
    if changeHandleCalledCount == 2 then
        XblUserStatisticsGetSingleUserStatisticAsync();
    end
end

test.isbvt = true;
test.TestStatsBvt = function()
    common.init(TestStatsBvt_Handler)
end


