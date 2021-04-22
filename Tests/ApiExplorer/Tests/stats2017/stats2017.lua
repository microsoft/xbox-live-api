test = require 'u-test'
common = require 'common'

function OnXblTitleManagedStatsWriteAsyncWithSVD()
    XblTitleManagedStatsUpdateStatsAsync()
end

function OnXblTitleManagedStatsUpdateStatsAsync()
    XblTitleManagedStatsDeleteStatsAsync()
end

function OnXblTitleManagedStatsDeleteStatsAsync()
    ValidateSVD()
    test.stopTest()
end

function OnXblTitleManagedStatsUnableToGetTokenAndSignature()
    print("OnXblTitleManagedStatsUnableToGetTokenAndSignature")
    test.stopTest()
end

function TitleManagedStats_Handler()
    if ClearSVD() == 1 then
        OnXblTitleManagedStatsUnableToGetTokenAndSignature()
    else
        XblTitleManagedStatsWriteAsyncWithSVD()
    end
end

test.TitleManagedStats = function()
    common.init(TitleManagedStats_Handler)
end
