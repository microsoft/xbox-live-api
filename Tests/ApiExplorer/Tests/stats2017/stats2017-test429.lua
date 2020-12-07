test = require 'u-test'
common = require 'common'

function OnXblTitleManagedStatsWriteAsync()
    -- this fast loop will trigger 429s
    XblTitleManagedStatsWriteAsync();
end

function TitleManagedStatsNoSVD_Handler()
    XblTitleManagedStatsWriteAsync()
end

test.skip = true
test.TitleManagedStatsNoSVD = function()
    SetCheckHR(0)
    common.init(TitleManagedStatsNoSVD_Handler)
end
