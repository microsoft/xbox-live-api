test = require 'u-test'
common = require 'common'

function TitleStorage()
    print("TitleStorage");
    XblTitleStorageGetQuotaAsync();
end

function OnXblTitleStorageGetQuotaAsync()
    print('Calling XblTitleStorageUploadJsonBlobAsync')
    XblTitleStorageUploadJsonBlobAsync(
        "Json Blob Upload",
        "apirunner/test/path/json.txt",
        "{\"difficulty\":1,\"level\":[{\"number\":\"1\",\"quest\":\"swords\"},{\"number\":\"2\",\"quest\":\"iron\"},{\"number\":\"3\",\"quest\":\"gold\"}],\"weapon\":{\"name\":\"poison\",\"timeleft\":\"2mins\"}}",
        "XblTitleStorageType::Universal", 
        "XblTitleStorageBlobType::Json", 
        1024, 
        "XblTitleStorageETagMatchCondition::NotUsed"
    );

end

function OnXblTitleStorageUploadJsonBlobAsync()
    print('OnXblTitleStorageUploadJsonBlobAsync')
    print('Calling XblTitleStorageUploadBinaryBlobAsync')
    XblTitleStorageUploadBinaryBlobAsync(
        "Test Name 2", 
        "apirunner/test/path.txt", 
        "XblTitleStorageType::Universal", 
        "XblTitleStorageBlobType::Binary", 
        "XblTitleStorageETagMatchCondition::NotUsed"
    );
end

function OnXblTitleStorageUploadBinaryBlobAsync()
    print('OnXblTitleStorageUploadBinaryBlobAsync')
    print('Getting blob metadata')
    XblTitleStorageGetBlobMetadataAsync(
        "XblTitleStorageType::Universal",
        "",
        0,
        0,
        2
    );
end

function OnXblTitleStorageGetBlobMetadataAsync()
    print('OnXblTitleStorageGetBlobMetadataAsync')
    XblTitleStorageBlobMetadataResultHasNext();
    print('Calling XblTitleStorageBlobMetadataResultGetNextAsync')
    XblTitleStorageBlobMetadataResultGetNextAsync(2);
end

function OnXblTitleStorageBlobMetadataResultGetNextAsync()
    print('OnXblTitleStorageBlobMetadataResultGetNextAsync')
    XblTitleStorageBlobMetadataResultGetItems();
    XblTitleStorageBlobMetadataResultDuplicateHandle();

    print('Calling XblTitleStorageDownloadJsonBlobAsync')
    XblTitleStorageDownloadJsonBlobAsync("weapon.name");
end

function OnXblTitleStorageDownloadJsonBlobAsync()
    print('OnXblTitleStorageDownloadJsonBlobAsync')
    print('Calling XblTitleStorageDownloadBinaryBlobAsync')
    XblTitleStorageDownloadBinaryBlobAsync();
end

function OnXblTitleStorageDownloadBinaryBlobAsync()
    print('OnXblTitleStorageDownloadBinaryBlobAsync')
    print('Calling XblTitleStorageDeleteJsonBlobAsync')
    XblTitleStorageDeleteJsonBlobAsync();
end

function OnXblTitleStorageDeleteJsonBlobAsync()
    print('OnXblTitleStorageDeleteJsonBlobAsync')
    print('Calling XblTitleStorageDeleteBinaryBlobAsync')
    XblTitleStorageDeleteBinaryBlobAsync();
end

function OnXblTitleStorageDeleteBinaryBlobAsync()
    print('OnXblTitleStorageDeleteBinaryBlobAsync')
    XblTitleStorageBlobMetadataResultCloseHandle();
    test.stopTest();
end

test.TitleStorage = function()
    common.init(TitleStorage)
end