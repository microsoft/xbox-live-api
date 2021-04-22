test = require 'u-test'
common = require 'common'

function TitleStorageRestAPI()
    print("TitleStorage");
    RestCallForTMSMetadata();
end

function OnXblTitleStorageRestTMSMetadata()
    print('Calling RestCallForEachBlob')
    RestCallForEachBlob();
end

function OnRestCallForEachBlob()
    print('OnRestCallForEachBlob')
    test.stopTest();
end

test.skip = true
test.TitleStorageRestAPI = function()
    common.init(TitleStorageRestAPI)
end