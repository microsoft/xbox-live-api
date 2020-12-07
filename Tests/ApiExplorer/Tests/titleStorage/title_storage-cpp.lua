test = require 'u-test'
common = require 'common'

function TitleStorageCpp()
    print("TitleStorageCpp");
    TitleStorageServiceGetQuota();
end

function OnTitleStorageServiceGetQuota()
    print('OnTitleStorageServiceGetQuota')
    print('Calling TitleStorageServiceUploadBlob')
    TitleStorageServiceUploadBlob(
        "title_storage_type::universal", 
        "apirunner/test/path.txt", 
        "title_storage_blob_type::binary",
        "Test Binary Blob Upload", 
        "title_storage_e_tag_match_condition::not_used"
    );
end

function OnTitleStorageServiceUploadBlob()
    print('OnTitleStorageServiceUploadBlob')
    print('Getting blob metadata')
    TitleStorageServiceGetBlobMetadata(
        "title_storage_type::universal",
        "",
        0,
        0,
        2
    );
end

function OnTitleStorageServiceGetBlobMetadata()
    print('OnTitleStorageServiceGetBlobMetadata')
    hr, hasNext = TitleStorageBlobMetadataResultHasNextCpp()
    print("hasNext " .. hr)
    print("hr " .. hr)
    if hasNext ~= 0 then
        TitleStorageBlobMetadataResultGetNextCpp(2)
    else
        OnTitleStorageBlobMetadataResultGetNextCpp()
    end
end

function OnTitleStorageBlobMetadataResultGetNextCpp()
    print('OnTitleStorageBlobMetadataResultGetNextCpp')
    print('Calling TitleStorageServiceDownloadBlob')
    TitleStorageServiceDownloadBlob("")
end

function OnTitleStorageServiceDownloadBlob()
    print('OnTitleStorageServiceDownloadBlob')
    print('Calling TitleStorageServiceDeleteBlobAsync')
    TitleStorageServiceDeleteBlob();
end

function OnTitleStorageServiceDeleteBlob()
    print('OnTitleStorageServiceDeleteBlob')
    test.stopTest();
end

test.TitleStorageCpp = function()
    common.init(TitleStorageCpp)
end