// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "title_storage_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char quotaResponse[] = R"(
{
    "quotaInfo": {
        "usedBytes": 777,
        "quotaBytes": 268435456
    }
})";

const char blobMetadataPage1[] = R"(
{
    "blobs": [
        {
            "fileName": "test/sample.json,json",
            "etag": "\"0x8D1C76581F12853\"",
            "size": 67
        }
    ],
    "pagingInfo": {
        "totalItems": 2,
        "continuationToken": "gF1nbDcy7loA_BWANNku1Go-6lM1~qH499VFFeHwow0UUKyXpSETnrEY1"
    }
})";

const char blobMetadataPage2[] = R"(
{
    "blobs": [
        {
            "fileName": "test/sample.json,json",
            "etag": "\"0x8D1C76581F12853\"",
            "size": 67
        }
    ],
    "pagingInfo": {
        "totalItems": 2
    }
})";

const char jsonBlob[] = R"(
{
    "isThisJson": 1,
    "monstersKilled": 10,
    "playerClass": "warrior"
})";

const uint8_t binaryBlob[] = { 
    10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 
    0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 
    55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100 
};

const char continuationTokenJson[] = R"(
{
    "continuationToken":"257e3886-cf73-4b52-8e89-2d7088d60bab-1"
})";

// A blobs array whose SECOND entry is malformed: "fileName" has no ',' separating the blob path
// from the blob type, which DeserializeXblTitleStorageBlobMetadata rejects with
// WEB_E_INVALID_JSON_STRING. The first entry is valid and carries distinctive, easily recognized
// field values.
//
// This exercises the failure path of the in-place deserializer (AB#63508132): because metadata is
// now written directly into caller-owned storage instead of a discarded temporary, the failure must
// still propagate cleanly rather than yielding a truncated or partially populated result.
const char blobMetadataMalformedSecondEntry[] = R"(
{
    "blobs": [
        {
            "fileName": "test/first-valid.json,json",
            "etag": "\"0xFIRSTVALIDETAG\"",
            "size": 4242
        },
        {
            "fileName": "test/malformed-no-comma",
            "etag": "\"0x8D1C76581F12853\"",
            "size": 67
        }
    ],
    "pagingInfo": {
        "totalItems": 2
    }
})";

// A blobs array with a single entry that omits every optional field. Nothing in this entry causes a
// write to blobPath, blobType, length, or clientTimestamp, so all of them must read back zeroed
// purely on the strength of the defensive memset in DeserializeXblTitleStorageBlobMetadata.
const char blobMetadataSparseEntry[] = R"(
{
    "blobs": [
        {
            "etag": "\"0xSPARSEETAG\""
        }
    ],
    "pagingInfo": {
        "totalItems": 1
    }
})";

DEFINE_TEST_CLASS(TitleStorageTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(TitleStorageTests)

    // RAII wrapper for XblTitleStorageBlobMetadataResultHandle 
    class BlobMetadataResult
    {
    public:
        explicit BlobMetadataResult(XblTitleStorageBlobMetadataResultHandle h) noexcept : handle{ h }
        {
            assert(handle);
        }

        BlobMetadataResult(const BlobMetadataResult& other) noexcept
        {
            auto hr = XblTitleStorageBlobMetadataResultDuplicateHandle(other.handle, &handle);
            VERIFY_SUCCEEDED(hr);
        }

        BlobMetadataResult& operator=(BlobMetadataResult other)
        {
            std::swap(other.handle, handle);
            return *this;
        }

        ~BlobMetadataResult() noexcept
        {
            XblTitleStorageBlobMetadataResultCloseHandle(handle);
        }

        std::vector<XblTitleStorageBlobMetadata> Items() const noexcept
        {
            const XblTitleStorageBlobMetadata* items{ nullptr };
            size_t count{ 0 };
            VERIFY_SUCCEEDED(XblTitleStorageBlobMetadataResultGetItems(handle, &items, &count));

            return std::vector<XblTitleStorageBlobMetadata>(items, items + count);
        }

        bool HasNext() const noexcept
        {
            bool hasNext{ false };
            VERIFY_SUCCEEDED(XblTitleStorageBlobMetadataResultHasNext(handle, &hasNext));
            return hasNext;
        }

        void GetNext() noexcept
        {
            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblTitleStorageBlobMetadataResultGetNextAsync(handle, 0, &async));
            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

            XblTitleStorageBlobMetadataResultCloseHandle(handle);
            VERIFY_SUCCEEDED(XblTitleStorageBlobMetadataResultGetNextResult(&async, &handle));
        }

    private:
        XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
    };

    // Returns true if a fixed-size buffer is zero across its entire width. Used to assert that
    // fields absent from a service response are left fully zeroed when metadata is deserialized in
    // place into caller-owned storage.
    static bool IsBufferZeroed(const char* buffer, size_t size) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (buffer[i] != '\0')
            {
                return false;
            }
        }
        return true;
    }

    void VerifyBlobMetadata(const XblTitleStorageBlobMetadata& actual, JsonValue& expected)
    {
        VERIFY_IS_TRUE(expected.IsObject());

        String filename = expected["fileName"].GetString();
        auto tokens{ utils::string_split_internal(filename, ',') };
        VERIFY_ARE_EQUAL_UINT(2, tokens.size());
        VERIFY_ARE_EQUAL_STR(tokens[0].data(), actual.blobPath);

        switch (actual.blobType)
        {
        case XblTitleStorageBlobType::Binary:
        {
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(tokens[1].data(), "binary");
            break;
        }
        case XblTitleStorageBlobType::Json:
        {
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(tokens[1].data(), "json");
            break;
        }
        case XblTitleStorageBlobType::Config: // Unsupported?
        case XblTitleStorageBlobType::Unknown:
        default:
        {
            VERIFY_FAIL();
        }
        }

        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(expected["etag"].GetString(), actual.eTag);
    }

    void VerifyBlobMetadataResult(const BlobMetadataResult& actual, JsonValue& expected)
    {
        auto actualBlobs{ actual.Items() };
        auto expectedBlobs{ expected["blobs"].GetArray() };
        VERIFY_ARE_EQUAL_UINT(expectedBlobs.Size(), actualBlobs.size());

        for (uint32_t i = 0; i < actualBlobs.size(); ++i)
        {
            VerifyBlobMetadata(actualBlobs[i], expectedBlobs[i]);
        }
    }

    xsapi_internal_string ExpectedUriPath(
        XblTitleStorageType storageType,
        uint64_t xuid,
        const xsapi_internal_string& scid = MOCK_SCID
    )
    {
        xsapi_internal_stringstream ss;
        switch (storageType)
        {
        case XblTitleStorageType::GlobalStorage:
        {
            ss << "/global";
            break;
        }
        case XblTitleStorageType::Universal:
        {
            ss << "/universalplatform/users/xuid(" << xuid << ")";
            break;
        }
        case XblTitleStorageType::TrustedPlatformStorage:
        {
            ss << "/trustedplatform/users/xuid(" << xuid << ")";
            break;
        }
        }
        ss << "/scids/" << scid;

        return ss.str();
    }

    xsapi_internal_string ExpectedUriPath(
        XblTitleStorageType storageType,
        uint64_t xuid,
        XblTitleStorageBlobType blobType,
        const xsapi_internal_string& blobPath = "blobPath",
        const xsapi_internal_string& scid = MOCK_SCID
    )
    {
        xsapi_internal_stringstream ss;
        ss << ExpectedUriPath(storageType, xuid, scid);
        ss << "/data/" << blobPath << ",";

        switch (blobType)
        {
        case XblTitleStorageBlobType::Binary:
        {
            ss << "binary";
            break;
        }
        case XblTitleStorageBlobType::Json:
        {
            ss << "json";
            break;
        }
        case XblTitleStorageBlobType::Config: // Unsupported?
        case XblTitleStorageBlobType::Unknown:
        default:
        {
            assert(false);
            break;
        }
        }
        return ss.str();
    }

    void UploadBlob(
        XblContextHandle xboxLiveContext,
        XblTitleStorageType storageType,
        XblTitleStorageBlobType blobType,
        const uint8_t* uploadData,
        size_t uploadDataSize,
        uint32_t bufferSizeMultiplier
    )
    {
        XblTitleStorageBlobMetadata metadata
        {
            "blobPath",
            blobType,
            storageType,
            "Name",
            "0x52345234e3",
            1, // Use a non-zero client time so that we don't ignore it
            0,
            MOCK_SCID,
            xboxLiveContext->Xuid()
        };

        auto mock = std::make_shared<HttpMock>( "PUT", "https://titlestorage.xboxlive.com" );

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                requestWellFormed &= !requestBody.empty();

                auto queryParams = xbox::services::uri::split_query(xbox::services::uri{ requestUrl.data() }.query());

                requestWellFormed &= queryParams["clientFileTime"] == "Thu,%2001%20Jan%201970%2000:00:01%20GMT";
                requestWellFormed &= queryParams["displayName"] == metadata.displayName;
                if (blobType == XblTitleStorageBlobType::Binary)
                {
                    requestWellFormed &= queryParams["finalBlock"] == "true";
                }
            }
        );

        std::vector<uint8_t> buffer(uploadDataSize * bufferSizeMultiplier);
        memcpy(buffer.data(), uploadData, uploadDataSize);
        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageUploadBlobAsync(
            xboxLiveContext,
            metadata,
            buffer.data(),
            uploadDataSize * bufferSizeMultiplier,
            XblTitleStorageETagMatchCondition::NotUsed,
            1000,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        VERIFY_SUCCEEDED(XblTitleStorageUploadBlobResult(&async, &metadata));
    }

    void DownloadBlob(
        XblContextHandle xboxLiveContext,
        XblTitleStorageType storageType,
        XblTitleStorageBlobType blobType,
        uint32_t bufferSizeMultiplier
    )
    {
        auto mock = std::make_shared<HttpMock>( "GET", "https://titlestorage.xboxlive.com" );
        const uint8_t* responseBody{ nullptr };
        size_t responseBodySize{ 0 };
        bool requestWellFormed{ true };

        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            requestWellFormed &= requestBody.empty();
            requestWellFormed &= (HttpMock::GetUriPath(requestUrl) == ExpectedUriPath(storageType, xboxLiveContext->Xuid(), blobType));

            switch (blobType)
            {
            case XblTitleStorageBlobType::Binary:
            {
                responseBody = binaryBlob;
                responseBodySize = sizeof(binaryBlob);
                break;
            }
            case XblTitleStorageBlobType::Json:
            {
                responseBody = reinterpret_cast<const uint8_t*>(jsonBlob);
                responseBodySize = sizeof(jsonBlob);
                break;
            }
            case XblTitleStorageBlobType::Config: // Unsupported?
            case XblTitleStorageBlobType::Unknown:
            default:
            {
                assert(false);
                break;
            }
            }

            mock->SetResponseBody(responseBody, responseBodySize);
        }
        );

        size_t blobSize{ 0 };
        switch (blobType)
        {
        case XblTitleStorageBlobType::Binary:
        {
            blobSize = sizeof(binaryBlob);
            break;
        }
        case XblTitleStorageBlobType::Json:
        {
            blobSize = sizeof(jsonBlob);
            break;
        }
        case XblTitleStorageBlobType::Config: // Unsupported?
        case XblTitleStorageBlobType::Unknown:
        default:
        {
            assert(false);
            break;
        }
        }

        XblTitleStorageBlobMetadata metadata
        {
            "blobPath",
            blobType,
            storageType,
            "Name",
            "0x52345234e3",
            0,
            blobSize,
            MOCK_SCID,
            xboxLiveContext->Xuid()
        };

        std::vector<uint8_t> retreivedBlob(blobSize * bufferSizeMultiplier);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageDownloadBlobAsync(
            xboxLiveContext,
            metadata,
            retreivedBlob.data(),
            retreivedBlob.size(),
            XblTitleStorageETagMatchCondition::NotUsed,
            nullptr,
            0,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        VERIFY_SUCCEEDED(XblTitleStorageDownloadBlobResult(&async, &metadata));
        VERIFY_ARE_EQUAL_UINT(responseBodySize, metadata.length);
        VERIFY_IS_TRUE(memcmp(responseBody, retreivedBlob.data(), retreivedBlob.size() / bufferSizeMultiplier) == 0);
    }

    void DeleteBlob(
        XblContextHandle xboxLiveContext,
        XblTitleStorageType storageType,
        XblTitleStorageBlobType blobType
    )
    {
        auto mock = std::make_shared<HttpMock>( "DELETE", "https://titlestorage.xboxlive.com" );

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                requestWellFormed &= requestBody.empty();
                requestWellFormed &= (HttpMock::GetUriPath(requestUrl) == ExpectedUriPath(storageType, xboxLiveContext->Xuid(), blobType));
            }
        );

        XblTitleStorageBlobMetadata metadata
        {
            "blobPath",
            blobType,
            storageType,
            "Name",
            "0x52345234e3",
            0,
            0,
            MOCK_SCID,
            xboxLiveContext->Xuid()
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageDeleteBlobAsync(
            xboxLiveContext,
            metadata,
            true,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    void GetBlobMetadata(
        XblContextHandle xboxLiveContext,
        XblTitleStorageType type
    )
    {
        auto mock = std::make_shared<HttpMock>( "GET", "https://titlestorage.xboxlive.com" );
        JsonDocument responseJson;

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                requestWellFormed &= requestBody.empty();

                xsapi_internal_string expectedPath{ ExpectedUriPath(type, xboxLiveContext->Xuid()) };
                expectedPath += "/data/blobPath";

                requestWellFormed &= (HttpMock::GetUriPath(requestUrl) == expectedPath);

                if (HttpMock::GetUriQuery(requestUrl).find("continuationToken") != string_t::npos)
                {
                    responseJson.Parse(blobMetadataPage2);
                }
                else
                {
                    responseJson.Parse(blobMetadataPage1);
                }
                mock->SetResponseBody(responseJson);
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageGetBlobMetadataAsync(
            xboxLiveContext,
            MOCK_SCID,
            type,
            "blobPath",
            xboxLiveContext->Xuid(),
            0,
            0,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
        VERIFY_SUCCEEDED(XblTitleStorageGetBlobMetadataResult(&async, &handle));

        BlobMetadataResult result{ handle };
        VerifyBlobMetadataResult(result, responseJson);

        while (result.HasNext())
        {
            result.GetNext();
            VERIFY_IS_TRUE(requestWellFormed);
            VerifyBlobMetadataResult(result, responseJson);
        }
    }

    void GetQuota(
        XblContextHandle xboxLiveContext,
        XblTitleStorageType type
    )
    {
        auto mock = std::make_shared<HttpMock>( "GET", "https://titlestorage.xboxlive.com" );
        mock->SetResponseBody(quotaResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                requestWellFormed &= requestBody.empty();
                requestWellFormed &= (HttpMock::GetUriPath(requestUrl) == ExpectedUriPath(type, xboxLiveContext->Xuid()));
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageGetQuotaAsync(xboxLiveContext, MOCK_SCID, type, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t usedBytes{ 0 };
        size_t quotaBytes{ 0 };
        VERIFY_SUCCEEDED(XblTitleStorageGetQuotaResult(&async, &usedBytes, &quotaBytes));

        JsonDocument quotaJson;
        quotaJson.Parse(quotaResponse);
        VERIFY_ARE_EQUAL_UINT(quotaJson["quotaInfo"]["usedBytes"].GetUint64(), usedBytes);
        VERIFY_ARE_EQUAL_UINT(quotaJson["quotaInfo"]["quotaBytes"].GetUint64(), quotaBytes);
    }

    DEFINE_TEST_CASE(GetQuotaTest)
    {
        TEST_LOG(L"Test starting: GetQuotaTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        GetQuota(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage);
        GetQuota(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage);
        GetQuota(xboxLiveContext.get(), XblTitleStorageType::Universal);
    }

    DEFINE_TEST_CASE(GetBlobMetadataTest)
    {
        TEST_LOG(L"Test starting: GetBlobMetadataTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        GetBlobMetadata(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage);
        GetBlobMetadata(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage);
        GetBlobMetadata(xboxLiveContext.get(), XblTitleStorageType::Universal);
    }

    DEFINE_TEST_CASE(DeleteBlobTest)
    {
        TEST_LOG(L"Test starting: DeleteBlobTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        DeleteBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Json);
        DeleteBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Binary);
        DeleteBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Json);
        DeleteBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Binary);
    }

    DEFINE_TEST_CASE(DownloadBlobTest)
    {
        TEST_LOG(L"Test starting: DownloadBlobTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Json, 1);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Binary, 1);
        //DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Config, 1); Unsupported?
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Json, 1);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Binary, 1);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Json, 1);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Binary, 1);
    }

    DEFINE_TEST_CASE(DownloadBlobWithLargeBufferTest)
    {
        TEST_LOG(L"Test starting: DownloadBlobWithLargeBufferTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Json, 2);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Binary, 2);
        //DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::GlobalStorage, XblTitleStorageBlobType::Config, 2); Unsupported?
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Json, 2);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::TrustedPlatformStorage, XblTitleStorageBlobType::Binary, 2);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Json, 2);
        DownloadBlob(xboxLiveContext.get(), XblTitleStorageType::Universal, XblTitleStorageBlobType::Binary, 2);
    }

    DEFINE_TEST_CASE(UploadBlobTest)
    {
        TEST_LOG(L"Test starting: UploadBlobTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::TrustedPlatformStorage,
            XblTitleStorageBlobType::Json,
            reinterpret_cast<const uint8_t*>(jsonBlob),
            sizeof(jsonBlob),
            1
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::TrustedPlatformStorage,
            XblTitleStorageBlobType::Binary,
            binaryBlob,
            sizeof(binaryBlob),
            1
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::Universal,
            XblTitleStorageBlobType::Json,
            reinterpret_cast<const uint8_t*>(jsonBlob),
            sizeof(jsonBlob),
            1
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::Universal,
            XblTitleStorageBlobType::Binary,
            binaryBlob,
            sizeof(binaryBlob),
            1
        );
    }

    DEFINE_TEST_CASE(UploadBlobWithLargeBufferTest)
    {
        TEST_LOG(L"Test starting: UploadBlobWithLargeBufferTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::TrustedPlatformStorage,
            XblTitleStorageBlobType::Json,
            reinterpret_cast<const uint8_t*>(jsonBlob),
            sizeof(jsonBlob),
            2
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::TrustedPlatformStorage,
            XblTitleStorageBlobType::Binary,
            binaryBlob,
            sizeof(binaryBlob),
            2
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::Universal,
            XblTitleStorageBlobType::Json,
            reinterpret_cast<const uint8_t*>(jsonBlob),
            sizeof(jsonBlob),
            2
        );

        UploadBlob(
            xboxLiveContext.get(),
            XblTitleStorageType::Universal,
            XblTitleStorageBlobType::Binary,
            binaryBlob,
            sizeof(binaryBlob),
            2
        );
    }

    DEFINE_TEST_CASE(UploadBlobMultipleChunksTest)
    {
        TEST_LOG(L"Test starting: UploadBlobMultipleChunksTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        const size_t blockSize{ 256 * 1024 };
        uint8_t data[blockSize + 1]; // ensure there are two chunks
        for (uint32_t i = 0; i < _countof(data); ++i)
        {
            data[i] = rand() % UCHAR_MAX;
        }

        XblTitleStorageBlobMetadata metadata
        {
            "blobPath",
            XblTitleStorageBlobType::Binary,
            XblTitleStorageType::Universal,
            "Name",
            "0x52345234e3",
            0,
            0,
            MOCK_SCID,
            xboxLiveContext->Xuid()
        };

        // This should result in multiple Http calls (1 per chunk), but intermediate results should not be
        // propagated to the client
        auto mock = std::make_shared<HttpMock>( "PUT", "https://titlestorage.xboxlive.com" );

        uint32_t requestCount{ 0 };
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(requestBody);

                auto queryParams = xbox::services::uri::split_query(xbox::services::uri{ requestUrl.data() }.query());
                if (queryParams["finalBlock"] == "true")
                {
                    // Validate that we supplied the continuation token
                    JsonDocument d;
                    d.Parse(continuationTokenJson);

                    requestWellFormed &= d["continuationToken"].GetString() == queryParams["continuationToken"];
                    
                    // no response body in this case
                    mock->ClearReponseBody();
                }
                else if (queryParams["finalBlock"] == "false")
                {
                    mock->SetResponseBody(continuationTokenJson);
                }
                else
                {
                    assert(false);
                }
                requestCount++;
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageUploadBlobAsync(
            xboxLiveContext.get(),
            metadata,
            data,
            sizeof(data),
            XblTitleStorageETagMatchCondition::NotUsed,
            blockSize,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_ARE_EQUAL_UINT(2, requestCount);

        VERIFY_SUCCEEDED(XblTitleStorageUploadBlobResult(&async, &metadata));
    }

    // Direct regression test for the in-place deserializer's zeroing contract (AB#63508132).
    //
    // DeserializeXblTitleStorageBlobMetadata now writes into caller-owned storage instead of
    // returning a Result<XblTitleStorageBlobMetadata> by value. That removed two ~1.2KB copies from
    // the frame, but it also means a caller's pre-existing bytes survive anything the deserializer
    // does not explicitly overwrite - so it must zero its out-param up front.
    //
    // The service call path happens to hide a regression here, because ExtractJsonVectorInPlace
    // value-initializes each vector element before handing it over. This test calls the deserializer
    // directly with deliberately dirtied storage so the memset itself is what is under test.
    DEFINE_TEST_CASE(DeserializeBlobMetadataZeroesCallerStorageTest)
    {
        TEST_LOG(L"Test starting: DeserializeBlobMetadataZeroesCallerStorageTest");

        // Poison every byte the way a reused stack slot would be.
        XblTitleStorageBlobMetadata metadata;
        memset(&metadata, 0xCD, sizeof(metadata));

        JsonDocument json;
        json.Parse(R"({ "etag": "\"0xSPARSEETAG\"" })");
        VERIFY_IS_FALSE(json.HasParseError());

        VERIFY_SUCCEEDED(XblTitleStorageBlobMetadataResult::DeserializeXblTitleStorageBlobMetadata(json, metadata));

        // Nothing in this payload writes blobPath, blobType, length or clientTimestamp, so all of
        // them must come back zeroed rather than holding the caller's previous bytes.
        //
        // displayName is deliberately not byte-checked: the deserializer always calls utils::strcpy
        // for it (with an empty string when the member is absent), and strcpy_s in debug CRT builds
        // fills the remainder of the destination past the null terminator with 0xFE. That fill is
        // deterministic, so it does not disturb the memcmp-based comparisons this zeroing protects.
        VERIFY_IS_TRUE(IsBufferZeroed(metadata.blobPath, sizeof(metadata.blobPath)));
        VERIFY_ARE_EQUAL_STR("", metadata.displayName);
        VERIFY_IS_TRUE(metadata.blobType == XblTitleStorageBlobType::Unknown);
        VERIFY_ARE_EQUAL_UINT(0u, metadata.length);
        VERIFY_ARE_EQUAL_INT(0, static_cast<int>(metadata.clientTimestamp));
        VERIFY_ARE_EQUAL_UINT(0u, metadata.xboxUserId);

        // And the field that IS present still round-trips.
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE("\"0xSPARSEETAG\"", metadata.eTag);
    }

    // Companion to the above, exercised through the real service call path: a malformed entry must
    // fail the whole request rather than yield a truncated or partially populated result.
    //
    // Scope note: this asserts only that the failure propagates out of the async call. The
    // pop_back() in ExtractJsonVectorInPlace is NOT covered - XblTitleStorageBlobMetadataResult
    // ::Deserialize discards the whole result on failure, so the partially written element is never
    // observable from any caller. That pop_back is defense in depth for future callers that might
    // read the vector before the error propagates; nothing currently tests it.
    DEFINE_TEST_CASE(GetBlobMetadataMalformedEntryFailsTest)
    {
        TEST_LOG(L"Test starting: GetBlobMetadataMalformedEntryFailsTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("GET", "https://titlestorage.xboxlive.com");
        JsonDocument responseJson;
        responseJson.Parse(blobMetadataMalformedSecondEntry);
        mock->SetResponseBody(responseJson);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageGetBlobMetadataAsync(
            xboxLiveContext.get(),
            MOCK_SCID,
            XblTitleStorageType::Universal,
            "blobPath",
            xboxLiveContext->Xuid(),
            0,
            0,
            &async
        ));

        // The malformed entry must surface as a failure rather than a silently truncated or
        // partially populated result.
        HRESULT hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL_INT(WEB_E_INVALID_JSON_STRING, hr);

        XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
        VERIFY_FAILED(XblTitleStorageGetBlobMetadataResult(&async, &handle));
        VERIFY_IS_TRUE(handle == nullptr);
    }

    // Characterization test for the deserializer refactor: fields absent from the service response
    // must read back zeroed through the real call path. Complements
    // DeserializeBlobMetadataZeroesCallerStorageTest, which pins the same contract at the point
    // where it can actually regress.
    DEFINE_TEST_CASE(GetBlobMetadataSparseEntryIsZeroedTest)
    {
        TEST_LOG(L"Test starting: GetBlobMetadataSparseEntryIsZeroedTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("GET", "https://titlestorage.xboxlive.com");
        JsonDocument responseJson;
        responseJson.Parse(blobMetadataSparseEntry);
        mock->SetResponseBody(responseJson);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleStorageGetBlobMetadataAsync(
            xboxLiveContext.get(),
            MOCK_SCID,
            XblTitleStorageType::Universal,
            "blobPath",
            xboxLiveContext->Xuid(),
            0,
            0,
            &async
        ));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
        VERIFY_SUCCEEDED(XblTitleStorageGetBlobMetadataResult(&async, &handle));

        BlobMetadataResult result{ handle };
        auto items{ result.Items() };
        VERIFY_ARE_EQUAL_UINT(1u, items.size());

        const auto& metadata{ items[0] };

        // Present in the response.
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE("\"0xSPARSEETAG\"", metadata.eTag);

        // Absent from the response. Nothing in the deserializer writes these when their JSON members
        // are missing, so they are zeroed only because the deserializer memsets its out-param up
        // front. Without that, they would hold whatever was previously in caller-owned storage -
        // and callers memcmp and copy the whole struct.
        VERIFY_IS_TRUE(metadata.blobType == XblTitleStorageBlobType::Unknown);
        VERIFY_ARE_EQUAL_UINT(0u, metadata.length);
        VERIFY_ARE_EQUAL_INT(0, static_cast<int>(metadata.clientTimestamp));
        VERIFY_ARE_EQUAL_STR("", metadata.blobPath);

        // blobPath is never written for this payload, so it must be zero across its full width.
        //
        // This check is deliberately not applied to fields that ARE written, such as eTag:
        // utils::strcpy resolves to strcpy_s, which in debug CRT builds intentionally fills the
        // remainder of the destination buffer past the null terminator with 0xFE. That fill is
        // deterministic for a given buffer, so it does not disturb the memcmp-based change detection
        // this zeroing protects.
        VERIFY_IS_TRUE(IsBufferZeroed(metadata.blobPath, sizeof(metadata.blobPath)));
    }

    DEFINE_TEST_CASE(TitleStorageInvalidArgsTest)    {
        TEST_LOG(L"Test starting: TitleStorageInvalidArgsTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XAsyncBlock async{};
        VERIFY_ARE_EQUAL_INT(
            XblTitleStorageDeleteBlobAsync(xboxLiveContext.get(), XblTitleStorageBlobMetadata{}, true, &async), 
            E_INVALIDARG
        );

        XblTitleStorageBlobMetadata metadata
        {
            "blobPath",
            XblTitleStorageBlobType::Json,
            XblTitleStorageType::GlobalStorage,
            "Name",
            "0x52345234e3",
            0,
            0,
            MOCK_SCID,
            xboxLiveContext->Xuid()
        };

        std::vector<uint8_t> buffer(10);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageDownloadBlobAsync(
            xboxLiveContext.get(),
            XblTitleStorageBlobMetadata{},
            buffer.data(),
            buffer.size(),
            XblTitleStorageETagMatchCondition::NotUsed,
            nullptr,
            0,
            &async
        ), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageDownloadBlobAsync(
            xboxLiveContext.get(),
            metadata,
            nullptr,
            0,
            XblTitleStorageETagMatchCondition::NotUsed,
            nullptr,
            0,
            &async
        ), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageUploadBlobAsync(
            xboxLiveContext.get(),
            XblTitleStorageBlobMetadata{},
            buffer.data(),
            buffer.size(),
            XblTitleStorageETagMatchCondition::NotUsed,
            0,
            &async
        ), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageUploadBlobAsync(
            xboxLiveContext.get(),
            metadata,
            nullptr,
            0,
            XblTitleStorageETagMatchCondition::NotUsed,
            0,
            &async
        ), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageGetBlobMetadataAsync(
            xboxLiveContext.get(),
            nullptr,
            XblTitleStorageType::GlobalStorage,
            nullptr,
            0,
            0,
            0,
            &async
        ), E_INVALIDARG);

        VERIFY_ARE_EQUAL_INT(XblTitleStorageGetQuotaAsync(
            xboxLiveContext.get(),
            nullptr,
            XblTitleStorageType::GlobalStorage,
            &async
        ), E_INVALIDARG);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
