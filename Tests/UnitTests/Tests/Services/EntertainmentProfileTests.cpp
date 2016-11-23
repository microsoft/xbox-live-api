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
#define TEST_CLASS_OWNER L"garrettw"
#define TEST_CLASS_AREA L"EntertainmentProfile"
#include "UnitTestIncludes.h"


using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::EntertainmentProfile;


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultItemResponse =
LR"(
{
    "ContainedItems": [
        {
            "Contained": true,
            "Item": {
                "ItemId": null,
                "ProviderId": "4ea56956-be59-441e-b8c0-138a710d8569",
                "Provider": "0x3caeec88",
                "ContentType": null,
                "Title": null,
                "SubTitle": null,
                "ImageUrl": null,
                "AltImageUrl": null,
                "Locale": "en-US",
                "IsDefaultPin": false,
                "DeviceType": "XboxOne"
            }
        }
    ]
}
)";

const string_t addItemRequest =
LR"({
    "Items": [
        {
            "AltImageUrl": "http://www.someimage.com/logo/small",
            "ContentType": "Movie",
            "ImageUrl": "http://www.someimage.com/thumb/get?id=1234",
            "Locale": "en-us",
            "Provider": "0x3caeec88",
            "ProviderId": "4ea56956-be59-441e-b8c0-138a710d8569",
            "SubTitle": "XSAPISubTitle",
            "Title": "XSAPITitle"
        }
    ]
})";

const string_t removeItemRequest =
LR"({"Items":[{"Provider":"0x3caeec88","ProviderId":"4ea56956-be59-441e-b8c0-138a710d8569"}]})";

DEFINE_TEST_CLASS(EntertainmentProfileTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(EntertainmentProfileTests)

    void VerifyDefaultItem(EntertainmentProfileListContainsItemResult^ result, web::json::value achievementToVerify)
    {
        VERIFY_ARE_EQUAL_STR(L"0x3caeec88", result->Provider->Data());
        VERIFY_ARE_EQUAL_STR(L"4ea56956-be59-441e-b8c0-138a710d8569", result->ProviderId->Data());
        VERIFY_ARE_EQUAL(true, result->IsContained);
    }

    DEFINE_TEST_CASE(TestContainsItem)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestContainsItem);
        auto responseJson = web::json::value::parse(defaultItemResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        VERIFY_NO_THROW(
            pplx::create_task(xboxLiveContext->EntertainmentProfileListService->XboxOnePins->ContainsItemAsync(
                "provider",
                "providerId"
            )).then([this, responseJson, httpCall](task<EntertainmentProfileListContainsItemResult^> result)
            {
                EntertainmentProfileListContainsItemResult^ item = result.get();
                VerifyDefaultItem(item, responseJson);
                VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
                VERIFY_ARE_EQUAL_STR(L"https://eplists.mockenv.xboxlive.com", httpCall->ServerName);
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/lists/PINS/XBLPins/ContainsItems", httpCall->PathQueryFragment.to_string());
            }).wait()
            );
    }

    DEFINE_TEST_CASE(TestAddItem)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAddItem);

        string_t request = addItemRequest;
        request.erase(remove_if(request.begin(), request.end(), isspace), request.end());

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        VERIFY_NO_THROW(pplx::create_task(xboxLiveContext->EntertainmentProfileListService->XboxOnePins->AddItemAsync(
            Microsoft::Xbox::Services::Marketplace::MediaItemType::Movie,
            "4ea56956-be59-441e-b8c0-138a710d8569",
            "0x3caeec88",
            ref new Windows::Foundation::Uri("http://www.someimage.com/thumb/get?id=1234"),
            ref new Windows::Foundation::Uri("http://www.someimage.com/logo/small"),
            "XSAPITitle",
            "XSAPISubTitle",
            "en-us"
            )));

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://eplists.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/lists/PINS/XBLPins", httpCall->PathQueryFragment.to_string());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(request, httpCall->request_body().request_message_string());
    }

    DEFINE_TEST_CASE(TestRemoveItem)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRemoveItem);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        VERIFY_NO_THROW(pplx::create_task(xboxLiveContext->EntertainmentProfileListService->XboxOnePins->RemoveItemAsync(
            "4ea56956-be59-441e-b8c0-138a710d8569",
            "0x3caeec88"
            )));

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod.c_str());
        VERIFY_ARE_EQUAL_STR(L"https://eplists.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/lists/PINS/XBLPins/RemoveItems", httpCall->PathQueryFragment.to_string());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(removeItemRequest, httpCall->request_body().request_message_string());
    }

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

