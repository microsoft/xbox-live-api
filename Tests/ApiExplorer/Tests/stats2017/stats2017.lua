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

function TitleManagedStats_Handler()
    ClearSVD()
    XblTitleManagedStatsWriteAsyncWithSVD()
end

test.TitleManagedStats = function()
    common.init(TitleManagedStats_Handler)
end
