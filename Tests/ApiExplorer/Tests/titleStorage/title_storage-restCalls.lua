test = require 'u-test'
common = require 'common'

function TitleStorageRestAPI()
    print("TitleStorage");
    RestCallToUploadJsonBlob(
        "{\"difficulty\":1,\"level\":[{\"number\":\"1\",\"quest\":\"swords\"},{\"number\":\"2\",\"quest\":\"iron\"},{\"number\":\"3\",\"quest\":\"gold\"}],\"weapon\":{\"name\":\"poison\",\"timeleft\":\"2mins\"}}"
    );
end

function OnXblTitleStorageRestUpload()
    print('Calling RestCallToUploadJsonBlob')
    RestCallForJsonMetadata();
end

function OnDownloadMetadataBlobs()
    print('OnUploadBlobs')
    RestCallToDownloadJsonBlob();
end

function OnDownloadBlobs()
    print('OnDownloadBlobs')
    test.stopTest();
end

test.TitleStorageRestAPI = function()
    common.init(TitleStorageRestAPI)
end