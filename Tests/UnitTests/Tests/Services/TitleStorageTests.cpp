//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#define TEST_CLASS_OWNER L"blgross"
#define TEST_CLASS_AREA L"TitleStorage"
#include "UnitTestIncludes.h"

#include "TitleStorageService_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::TitleStorage;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const web::json::value queryJson = web::json::value::parse(LR"(
{
    "quotaInfo": {
        "usedBytes": 777,
        "quotaBytes": 268435456
    }
})");

const web::json::value blobMetadataJson = web::json::value::parse(LR"(
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
})");

const string_t downloadJson = LR"(
{
    "isThisJson": 1,
    "monstersKilled": 10,
    "playerClass": "warrior"
})";

const web::json::value largeUploadJson = web::json::value::parse(LR"(
{"continuationToken":"257e3886-cf73-4b52-8e89-2d7088d60bab-1"})");

const std::vector<unsigned char> downloadBinary = { 
    10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 
    0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 
    55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100, 10, 255, 101, 0, 1, 29, 50, 55, 100 
    };

DEFINE_TEST_CLASS(TitleStorageTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(TitleStorageTests)

    void VerifyTitleStorageBlobMetadata(TitleStorageBlobMetadata^ metadata, web::json::value expected)
    {
        auto fileName = expected[L"fileName"].as_string();
        auto nPos = fileName.find(',');
        auto blobType = fileName.substr(nPos + 1);
        VERIFY_IS_TRUE(utils::str_icmp(metadata->BlobType.ToString()->Data(), blobType) == 0);
        fileName.resize(nPos);
        VERIFY_ARE_EQUAL(metadata->BlobPath->ToString()->Data(), fileName);
        VERIFY_ARE_EQUAL(metadata->ETag->ToString()->Data(), expected[L"etag"].as_string());
    }

    void VerifyTitleStorageBlobMetadataResult(TitleStorageBlobMetadataResult^ metadata, web::json::value expected)
    {
        web::json::array blobs = expected[L"blobs"].as_array();
        VERIFY_ARE_EQUAL_INT(metadata->Items->Size, blobs.size());

        uint32_t counter = 0;
        for (auto blob : blobs)
        {
            VerifyTitleStorageBlobMetadata(metadata->Items->GetAt(counter), blob);
            ++counter;
        }
    }

    void VerifyTitleStorageQuota(TitleStorageQuota^ quota, web::json::value expected)
    {
        auto quotaInfoJson = expected[L"quotaInfo"];
        VERIFY_ARE_EQUAL_UINT(quota->UsedBytes, quotaInfoJson[L"usedBytes"].as_number().to_uint64());
        VERIFY_ARE_EQUAL_UINT(quota->QuotaBytes, quotaInfoJson[L"quotaBytes"].as_number().to_uint64());
    }
    
    void UploadBlobHelper(TitleStorageType type, TitleStorageBlobType blobType, const string_t& uri, std::vector<unsigned char> uploadData)
    {
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        Windows::Foundation::DateTime dt;
        dt.UniversalTime = 1000;
        auto blobMetadata = ref new TitleStorageBlobMetadata(
            _T("123456789"),
            type,
            _T("blobPath"),
            blobType,
            _T("TestXboxUserId"),
            _T("Name"),
            _T("0x52345234e3"),
            dt
            );

        auto writer = ref new Windows::Storage::Streams::DataWriter();
        writer->WriteBytes(Platform::ArrayReference<unsigned char>(&(uploadData.at(0)), static_cast<uint32_t>(uploadData.size())));
        auto buffer = writer->DetachBuffer();

        auto result = create_task(xboxLiveContext->TitleStorageService->UploadBlobAsync(
            blobMetadata,
            buffer,
            TitleStorageETagMatchCondition::NotUsed,
            1000
            )).get();

        stringstream_t uriPath;
        uriPath << uri;

        uriPath << _T("?clientFileTime=Mon,%2001%20Jan%201601%2000:00:00%20GMT&displayName=Name");
        if (blobType == TitleStorageBlobType::Binary)
        {
            uriPath << _T("&finalBlock=true");
        }

        VERIFY_ARE_EQUAL_STR(L"PUT", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(uriPath.str(), httpCall->PathQueryFragment.to_string());
    }

    void DownloadBlobHelper(TitleStorageType type, TitleStorageBlobType blobType, const string_t& uri, std::vector<unsigned char> blobResult)
    {
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(blobResult);
        Windows::Foundation::DateTime dt;
        dt.UniversalTime = 1000;
        auto blobMetadata = ref new TitleStorageBlobMetadata(
            _T("123456789"),
            type,
            _T("blobPath"),
            blobType,
            _T("TestXboxUserId"),
            _T("Name"),
            _T("0x52345234e3"),
            dt
            );

        Windows::Storage::Streams::IBuffer^ buffer = ref new Windows::Storage::Streams::Buffer(0);
        auto result = create_task(xboxLiveContext->TitleStorageService->DownloadBlobAsync(
            blobMetadata,
            buffer,
            TitleStorageETagMatchCondition::NotUsed,
            _T("")
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(uri, httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(result->BlobBuffer->Length, blobResult.size());

        auto reader = Windows::Storage::Streams::DataReader::FromBuffer(result->BlobBuffer);
        std::vector<unsigned char> actualBlobBuffer = std::vector<unsigned char>(reader->UnconsumedBufferLength);

        reader->ReadBytes(Platform::ArrayReference<unsigned char>(&(actualBlobBuffer.at(0)), static_cast<uint32_t>(blobResult.size())));

        VERIFY_ARE_EQUAL_INT(actualBlobBuffer.size(), blobResult.size());
    }

    void DeleteBlobHelper(TitleStorageType type, TitleStorageBlobType blobType, const string_t& uri)
    {
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        Windows::Foundation::DateTime dt;
        dt.UniversalTime = 1000;

        auto blobMetadata = ref new TitleStorageBlobMetadata(
            _T("123456789"),
            type,
            _T("blobPath"),
            blobType,
            _T("TestXboxUserId"),
            _T("Name"),
            _T("0x52345234e3"),
            dt
            );

        create_task(xboxLiveContext->TitleStorageService->DeleteBlobAsync(
            blobMetadata,
            true
            )).get();

        VERIFY_ARE_EQUAL_STR(L"DELETE", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(uri, httpCall->PathQueryFragment.to_string());
    }

    void GetBlobMetadataHelper(TitleStorageType type, const string_t& uriPath)
    {
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(blobMetadataJson);

        Platform::String^ xboxUserId = nullptr;
        if (type != TitleStorageType::GlobalStorage)
        {
            xboxUserId = L"TestXboxUserId";
        }
        auto result = create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataAsync(
            L"123456789",
            type,
            L"blobPath",
            xboxUserId,
            1,
            10
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);

        stringstream_t uri;
        uri << uriPath;
        uri << _T("/scids/123456789/data/blobPath?maxItems=10&skipItems=1");
        VERIFY_ARE_EQUAL(uri.str(), httpCall->PathQueryFragment.to_string());

        VERIFY_IS_TRUE(result->HasNext);
        VerifyTitleStorageBlobMetadataResult(result, blobMetadataJson);

        stringstream_t continuationUri;
        continuationUri << uriPath;
        continuationUri << _T("/scids/123456789/data/blobPath?maxItems=10&continuationToken=gF1nbDcy7loA_BWANNku1Go-6lM1~qH499VFFeHwow0UUKyXpSETnrEY1");
        result = create_task(result->GetNextAsync(10)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(continuationUri.str(), httpCall->PathQueryFragment.to_string());
        VerifyTitleStorageBlobMetadataResult(result, blobMetadataJson);
    }

    void GetQuotaHelper(TitleStorageType type, const string_t& uriPath)
    {
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(queryJson);
        auto result = create_task(xboxLiveContext->TitleStorageService->GetQuotaAsync(
            _T("123456789"),
            type
            )).get();

        stringstream_t uri;
        uri << uriPath;
        uri << _T("/scids/123456789");
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(uri.str(), httpCall->PathQueryFragment.to_string());

        VerifyTitleStorageQuota(result, queryJson);
    }

    DEFINE_TEST_CASE(GetQuotaTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(GetQuotaTest);
        GetQuotaHelper(TitleStorageType::GlobalStorage, _T("/global"));
        GetQuotaHelper(TitleStorageType::JsonStorage, _T("/json/users/xuid(TestXboxUserId)"));
        GetQuotaHelper(TitleStorageType::TrustedPlatformStorage, _T("/trustedplatform/users/xuid(TestXboxUserId)"));
        GetQuotaHelper(TitleStorageType::UntrustedPlatformStorage, _T("/untrustedplatform/users/xuid(TestXboxUserId)"));
        GetQuotaHelper(TitleStorageType::Universal, _T("/universal/users/xuid(TestXboxUserId)"));
    }

    DEFINE_TEST_CASE(GetQuotaForSessionStorageTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(GetQuotaForSessionStorageTest);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(queryJson);
        auto result = create_task(xboxLiveContext->TitleStorageService->GetQuotaForSessionStorageAsync(
            _T("123456789"),
            _T("TestTemplate"),
            _T("TestSession")
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/sessions/TestTemplate~TestSession/scids/123456789", httpCall->PathQueryFragment.to_string());

        VerifyTitleStorageQuota(result, queryJson);
    }

    DEFINE_TEST_CASE(GetBlobMetadataTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(GetBlobMetadataTest);
        GetBlobMetadataHelper(TitleStorageType::JsonStorage, _T("/json/users/xuid(TestXboxUserId)"));
        GetBlobMetadataHelper(TitleStorageType::GlobalStorage, _T("/global"));
        GetBlobMetadataHelper(TitleStorageType::TrustedPlatformStorage, _T("/trustedplatform/users/xuid(TestXboxUserId)"));
        GetBlobMetadataHelper(TitleStorageType::UntrustedPlatformStorage, _T("/untrustedplatform/users/xuid(TestXboxUserId)"));
        GetBlobMetadataHelper(TitleStorageType::Universal, _T("/universal/users/xuid(TestXboxUserId)"));
    }

    DEFINE_TEST_CASE(GetBlobMetadataForSessionStorageTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(GetBlobMetadataForSessionStorageTest);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(blobMetadataJson);
        auto result = create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataForSessionStorageAsync(
            _T("123456789"),
            _T("blobPath"),
            _T("TestTemplate"),
            _T("TestSession"),
            1,
            10
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/sessions/TestTemplate~TestSession/scids/123456789/data/blobPath?maxItems=10&skipItems=1", httpCall->PathQueryFragment.to_string());
        
        VerifyTitleStorageBlobMetadataResult(result, blobMetadataJson);
    }

    DEFINE_TEST_CASE(DeleteBlobTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(DeleteBlobTest);
        DeleteBlobHelper(TitleStorageType::JsonStorage, TitleStorageBlobType::Json, L"/json/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json");
        DeleteBlobHelper(TitleStorageType::SessionStorage, TitleStorageBlobType::Json, L"/sessions/~/scids/123456789/data/blobPath,json");
        DeleteBlobHelper(TitleStorageType::SessionStorage, TitleStorageBlobType::Binary, L"/sessions/~/scids/123456789/data/blobPath,binary");
        DeleteBlobHelper(TitleStorageType::TrustedPlatformStorage, TitleStorageBlobType::Json, L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json");
        DeleteBlobHelper(TitleStorageType::TrustedPlatformStorage, TitleStorageBlobType::Binary, L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary");
        DeleteBlobHelper(TitleStorageType::UntrustedPlatformStorage, TitleStorageBlobType::Json, L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json");
        DeleteBlobHelper(TitleStorageType::UntrustedPlatformStorage, TitleStorageBlobType::Binary, L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary");
        DeleteBlobHelper(TitleStorageType::Universal, TitleStorageBlobType::Json, L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json");
        DeleteBlobHelper(TitleStorageType::Universal, TitleStorageBlobType::Binary, L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary");
    }

    DEFINE_TEST_CASE(DownloadBlobTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(DownloadBlobTest);
#pragma warning(suppress: 6260)
        std::vector<unsigned char> jsonResultBuffer(sizeof(downloadJson) * 2);
        memcpy(&jsonResultBuffer[0], &downloadJson, sizeof(downloadJson));

#pragma warning(suppress: 6260)
        std::vector<unsigned char> binaryResultBuffer(sizeof(downloadBinary) * 2);
        memcpy(&binaryResultBuffer[0], &downloadBinary, sizeof(downloadBinary));

        DownloadBlobHelper(
            TitleStorageType::JsonStorage, 
            TitleStorageBlobType::Json, 
            L"/json/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json", 
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::GlobalStorage,
            TitleStorageBlobType::Json,
            L"/global/scids/123456789/data/blobPath,json",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::GlobalStorage,
            TitleStorageBlobType::Binary,
            L"/global/scids/123456789/data/blobPath,binary",
            binaryResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::GlobalStorage,
            TitleStorageBlobType::Config,
            L"/global/scids/123456789/data/blobPath,config",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::TrustedPlatformStorage,
            TitleStorageBlobType::Json,
            L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::TrustedPlatformStorage,
            TitleStorageBlobType::Binary,
            L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::UntrustedPlatformStorage,
            TitleStorageBlobType::Json,
            L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::UntrustedPlatformStorage,
            TitleStorageBlobType::Binary,
            L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::Universal,
            TitleStorageBlobType::Json,
            L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::Universal,
            TitleStorageBlobType::Binary,
            L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::SessionStorage,
            TitleStorageBlobType::Json,
            L"/sessions/~/scids/123456789/data/blobPath,json",
            jsonResultBuffer
            );

        DownloadBlobHelper(
            TitleStorageType::SessionStorage,
            TitleStorageBlobType::Binary,
            L"/sessions/~/scids/123456789/data/blobPath,binary",
            binaryResultBuffer
            );
    }

    DEFINE_TEST_CASE(UploadBlobTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(UploadBlobTest);
#pragma warning(suppress: 6260)
        std::vector<unsigned char> jsonUploadBuffer(sizeof(downloadJson) * 2);
        memcpy(&jsonUploadBuffer[0], &downloadJson, sizeof(downloadJson));

#pragma warning(suppress: 6260)
        std::vector<unsigned char> binaryUploadBuffer(sizeof(downloadBinary) * 2);
        memcpy(&binaryUploadBuffer[0], &downloadBinary, sizeof(downloadBinary));

        UploadBlobHelper(
            TitleStorageType::JsonStorage,
            TitleStorageBlobType::Json,
            L"/json/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::TrustedPlatformStorage,
            TitleStorageBlobType::Json,
            L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::TrustedPlatformStorage,
            TitleStorageBlobType::Binary,
            L"/trustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::UntrustedPlatformStorage,
            TitleStorageBlobType::Json,
            L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::UntrustedPlatformStorage,
            TitleStorageBlobType::Binary,
            L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::Universal,
            TitleStorageBlobType::Json,
            L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,json",
            jsonUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::Universal,
            TitleStorageBlobType::Binary,
            L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary",
            binaryUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::SessionStorage,
            TitleStorageBlobType::Json,
            L"/sessions/~/scids/123456789/data/blobPath,json",
            jsonUploadBuffer
            );

        UploadBlobHelper(
            TitleStorageType::SessionStorage,
            TitleStorageBlobType::Binary,
            L"/sessions/~/scids/123456789/data/blobPath,binary",
            binaryUploadBuffer
            );
    }

#pragma warning(suppress: 6262)
    DEFINE_TEST_CASE(UploadLargeUntrustedBlob)
    {
       DEFINE_TEST_CASE_PROPERTIES(UploadLargeUntrustedBlob);
       const uint32 DEFAULT_UPLOAD_BLOCK_SIZE = 256 * 1024;
       Windows::Storage::Streams::DataWriter^ dataWriter = ref new Windows::Storage::Streams::DataWriter();
       unsigned char randChars[DEFAULT_UPLOAD_BLOCK_SIZE];
       for (uint32 i = 0; i < DEFAULT_UPLOAD_BLOCK_SIZE; ++i)
       {
           unsigned char randChar = rand() % UCHAR_MAX;
           randChars[i] = randChar;
           dataWriter->WriteByte(randChar);
       }

       auto blobMetadata = ref new TitleStorageBlobMetadata(
           _T("123456789"),
           TitleStorageType::UntrustedPlatformStorage,
           _T("blobPath"),
           TitleStorageBlobType::Binary,
           _T("TestXboxUserId")
           );

       auto buffer = dataWriter->DetachBuffer();
       auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
       auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
       httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(largeUploadJson);

       auto result = create_task(xboxLiveContext->TitleStorageService->UploadBlobAsync(
           blobMetadata,
           buffer,
           TitleStorageETagMatchCondition::NotUsed,
           0
           )).get();

       VERIFY_ARE_EQUAL_STR(L"PUT", httpCall->HttpMethod);
       VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
       VERIFY_ARE_EQUAL(
           L"/untrustedplatform/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary?continuationToken=257e3886-cf73-4b52-8e89-2d7088d60bab-1&finalBlock=true", 
           httpCall->PathQueryFragment.to_string()
           );
    }

#pragma warning(suppress: 6262)
    DEFINE_TEST_CASE(UploadLargeUniversalBlob)
    {
        DEFINE_TEST_CASE_PROPERTIES(UploadLargeUniversalBlob);
        const uint32 DEFAULT_UPLOAD_BLOCK_SIZE = 256 * 1024;
        Windows::Storage::Streams::DataWriter^ dataWriter = ref new Windows::Storage::Streams::DataWriter();
        unsigned char randChars[DEFAULT_UPLOAD_BLOCK_SIZE];
        for (uint32 i = 0; i < DEFAULT_UPLOAD_BLOCK_SIZE; ++i)
        {
            unsigned char randChar = rand() % UCHAR_MAX;
            randChars[i] = randChar;
            dataWriter->WriteByte(randChar);
        }

        auto blobMetadata = ref new TitleStorageBlobMetadata(
            _T("123456789"),
            TitleStorageType::Universal,
            _T("blobPath"),
            TitleStorageBlobType::Binary,
            _T("TestXboxUserId")
            );

        auto buffer = dataWriter->DetachBuffer();
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(largeUploadJson);

        auto result = create_task(xboxLiveContext->TitleStorageService->UploadBlobAsync(
            blobMetadata,
            buffer,
            TitleStorageETagMatchCondition::NotUsed,
            0
            )).get();

        VERIFY_ARE_EQUAL_STR(L"PUT", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://titlestorage.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(
            L"/universal/users/xuid(TestXboxUserId)/scids/123456789/data/blobPath,binary?continuationToken=257e3886-cf73-4b52-8e89-2d7088d60bab-1&finalBlock=true",
            httpCall->PathQueryFragment.to_string()
            );
    }

    DEFINE_TEST_CASE(TitleStorageInvalidArgsTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(TitleStorageInvalidArgsTest);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->DeleteBlobAsync(nullptr, false)).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->DownloadBlobAsync(
                nullptr,
                nullptr,
                TitleStorageETagMatchCondition::NotUsed,
                nullptr
                )).get(),
            E_INVALIDARG
            );

        auto blobMetadata = ref new TitleStorageBlobMetadata(
            L"123456", 
            TitleStorageType::GlobalStorage, 
            L"test", 
            TitleStorageBlobType::Json, 
            L"TestUser"
            );

        auto blobBuffer = ref new Windows::Storage::Streams::Buffer(0);

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->DownloadBlobAsync(
                blobMetadata,
                nullptr,
                TitleStorageETagMatchCondition::NotUsed,
                nullptr
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->UploadBlobAsync(
                nullptr,
                blobBuffer,
                TitleStorageETagMatchCondition::NotUsed,
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->UploadBlobAsync(
                blobMetadata,
                nullptr,
                TitleStorageETagMatchCondition::NotUsed,
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataAsync(
                nullptr,
                TitleStorageType::GlobalStorage,
                nullptr,
                nullptr,
                0,
                0
                )).get(),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataAsync(
                L"scid",
                static_cast<TitleStorageType>((int)(TitleStorageType::TrustedPlatformStorage) - 1),
                nullptr,
                nullptr,
                0,
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataForSessionStorageAsync(
                nullptr,
                nullptr, 
                _T("name"), 
                _T("name"), 
                0, 
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataForSessionStorageAsync(
                _T("1234567"),
                nullptr,
                nullptr,
                _T("name"),
                0,
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetBlobMetadataForSessionStorageAsync(
                _T("1234567"),
                nullptr,
                _T("name"),
                nullptr,
                0,
                0
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetQuotaAsync(
                nullptr, 
                TitleStorageType::GlobalStorage
                )).get(),
            E_INVALIDARG
            );

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetQuotaAsync(
                _T("12345"), 
                static_cast<TitleStorageType>((int)(TitleStorageType::TrustedPlatformStorage) - 1)
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetQuotaForSessionStorageAsync(
                nullptr, 
                _T("name"), 
                _T("name")
                )).get(),
            E_INVALIDARG
            );
 
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetQuotaForSessionStorageAsync(
                _T("1234567"), 
                nullptr, 
                _T("name")
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->TitleStorageService->GetQuotaForSessionStorageAsync(
                _T("1234567"), 
                _T("name"), 
                nullptr
                )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            ref new TitleStorageBlobMetadata(
                nullptr, 
                TitleStorageType::GlobalStorage, 
                _T("path"), 
                TitleStorageBlobType::Json, 
                nullptr
                ),
            E_INVALIDARG
            );
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
