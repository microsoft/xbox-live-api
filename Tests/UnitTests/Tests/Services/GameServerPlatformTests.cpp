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
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"GameServerPlatform"
#include "UnitTestIncludes.h"
#include "SocialGroupConstants_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Social;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::GameServerPlatform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring g_defaultResponseAllocateClusterAsync =
LR"(
{
    "pollIntervalMilliseconds" : "60000",
    "fulfillmentState" : "Fulfilled",
    "hostName" : "TestHostName",
    "portMappings": 
    [
        {
            "Key": "GameData",
            "Value": {
                "internal": 7777,
                "external": 31000
            }
        },
        {
            "Key": "Microsoft.WindowsAzure.Plugins.RemoteAccess.Rdp",
            "Value": {
                "internal": 3389,
                "external": 3389
            }
        }
    ],
    "region" : "TestRegion"
}
)";


const std::wstring g_defaultResponseGetTicketStatus =
LR"(
{
    "clusterId": "test_clusterId",
    "description" : "test_description",
    "gameHostId" : "test_gameHostId",
    "hostName" : "test_hostName",
    "portMappings" : 
    [
        {
            "Key": "test_portname",
            "Value" : 
            {
                "internal": 7777,
                "external" : 31000
            }
        },
        {
            "Key": "Microsoft.WindowsAzure.Plugins.RemoteAccess.Rdp",
            "Value" : 
            {
                "internal": 3389,
                "external" : 3389
            }
        }
    ],
    "secureContext": "test_secureContext",
    "status" : "Active",
    "ticketId" : "test_ticketId",
    "titleId" : "test_titleId",
    "region" : "test_region"
}
)";

const std::wstring g_defaultGameServerMetadataResult =
LR"(
{
    "variants" :
    [
        {
            "gamevariantId":"test_gamevariantId",
            "name":"test_variantName",
            "rank":"100",
            "isPublisher":"True",
            "gameVariantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
        }
    ],
    "variantSchemas" :
    [
        {
            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
            "schemaContent":"test_variantSchemasContent",
            "name":"test_variantSchemasName"
        }
    ],
    "gsiSets" :
    [
        {
            "gsiSetId":"test_gsiSetId",
            "gsiSetName":"test_gsiSetName",
            "minRequiredPlayers":"200",
            "maxAllowedPlayers":"300",
            "selectionOrder":"400",
            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
        }
    ]
}
)";

const std::wstring g_defaultQosServers =
LR"(
{
    "qosServers" :
    [
        {
            "serverFqdn":"test_serverFqdn",
            "serverSecureDeviceAddress":"test_serverSecureDeviceAddress",
            "targetLocation":"test_targetLocation"
        },
        {
            "serverFqdn":"test2_serverFqdn",
            "serverSecureDeviceAddress":"test2_serverSecureDeviceAddress",
            "targetLocation":"test2_targetLocation"
        }
    ]
}
)";

DEFINE_TEST_CLASS(GameServerPlatformTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(GameServerPlatformTests);

    DEFINE_TEST_CASE(TestAllocateClusterAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAllocateClusterAsync);
        auto responseJson = web::json::value::parse(g_defaultResponseAllocateClusterAsync);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        uint32 gameServerTitleId = 123;
        Platform::String^ serviceConfigurationId = L"testServiceConfigurationId";
        Platform::String^ sessionTemplateName = L"testSessionTemplateName";
        Platform::String^ sessionName = L"testSessionName";
        bool abortIfQueued = true;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterAsync(
            gameServerTitleId,
            serviceConfigurationId,
            sessionTemplateName,
            sessionName,
            abortIfQueued)).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://gameserverms.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/123/clusters", httpCall->PathQueryFragment.to_string());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(LR"({"abortIfQueued":true,"sessionId":"/serviceconfigs/testServiceConfigurationId/sessionTemplates/testSessionTemplateName/sessions/testSessionName"})", httpCall->request_body().request_message_string());
        VerifyClusterResult(result, responseJson);
    }

    void VerifyClusterResult(ClusterResult^ result, web::json::value resultToVerify)
    {
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"00:01:00", TimeSpanToString(result->PollInterval));
        VERIFY_ARE_EQUAL_INT(GameServerFulfillmentState::Fulfilled, result->FulfillmentState);
        VERIFY_ARE_EQUAL(resultToVerify[L"hostName"].as_string(), result->HostName->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"region"].as_string(), result->Region->Data());
        VERIFY_ARE_EQUAL_INT(2, result->PortMappings->Size);
        std::wstring portName = result->PortMappings->GetAt(0)->PortName->Data();
        VERIFY_ARE_EQUAL_STR(L"GameData", portName);
        VERIFY_ARE_EQUAL_INT(7777, result->PortMappings->GetAt(0)->InternalPortNumber);
        VERIFY_ARE_EQUAL_INT(31000, result->PortMappings->GetAt(0)->ExternalPortNumber);
    }

    DEFINE_TEST_CASE(TestAllocateClusterInlineAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAllocateClusterInlineAsync);
        auto responseJson = web::json::value::parse(g_defaultResponseAllocateClusterAsync);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        uint32 gameServerTitleId = 123;
        Platform::String^ serviceConfigurationId = L"testServiceConfigurationId";
        Platform::String^ ticketId = L"testTicketId";
        Platform::String^ sandboxId = L"testSandboxId";
        Platform::String^ gsiSetId = L"testGsiSetId";
        Platform::String^ gameVariantId = L"testGameVariantId";
        Platform::String^ location = L"testLocation";
        int maxAllowedPlayers = 100;
        bool abortIfQueued = true;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(
            gameServerTitleId,
            serviceConfigurationId,
            sandboxId,
            ticketId,
            gsiSetId,
            gameVariantId,
            maxAllowedPlayers,
            location,
            abortIfQueued)).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://gameserverms.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/123/inlineclusters", httpCall->PathQueryFragment.to_string());
        TEST_LOG(httpCall->request_body().request_message_string().c_str());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(LR"({"abortIfQueued":true,"gameVariantId":"testGameVariantId","gsiSetId":"testGsiSetId","location":"testLocation","maxAllowedPlayers":100,"sandboxId":"testSandboxId","serviceConfigId":"testServiceConfigurationId","ticketId":"testTicketId"})", httpCall->request_body().request_message_string());
        VerifyClusterResult(result, responseJson);
    }

    void TestVerifyGameTicketResult(GameServerTicketStatus^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL(resultToVerify[L"clusterId"].as_string(), result->ClusterId->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"description"].as_string(), result->Description->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"gameHostId"].as_string(), result->GameHostId->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"hostName"].as_string(), result->HostName->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"secureContext"].as_string(), result->SecureContext->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"ticketId"].as_string(), result->TicketId->Data());
        VERIFY_ARE_EQUAL(resultToVerify[L"region"].as_string(), result->Region->Data());

        VERIFY_ARE_EQUAL_INT(2, result->PortMappings->Size);
        std::wstring portName = result->PortMappings->GetAt(0)->PortName->Data();
        VERIFY_ARE_EQUAL_STR(L"test_portname", portName);
        VERIFY_ARE_EQUAL_INT(7777, result->PortMappings->GetAt(0)->InternalPortNumber);
        VERIFY_ARE_EQUAL_INT(31000, result->PortMappings->GetAt(0)->ExternalPortNumber);
    }

    DEFINE_TEST_CASE(TestGetTicketStatusAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTicketStatusAsync);
        auto responseJson = web::json::value::parse(g_defaultResponseGetTicketStatus);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        uint32 gameServerTitleId = 123;
        Platform::String^ serviceConfigurationId = L"testServiceConfigurationId";
        Platform::String^ ticketId = L"testTicketId";

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->GameServerPlatformService->GetTicketStatusAsync(
            gameServerTitleId,
            ticketId)).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://gameserverms.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/123/tickets/testTicketId/status", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL(LR"()", httpCall->request_body().request_message_string());

        TestVerifyGameTicketResult(result, responseJson);
    }

    void VerifyGameServerMetadataResult(GameServerMetadataResult^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL_INT(result->GameVariants->Size, 1);
        auto gsiVar = result->GameVariants->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"test_gamevariantId", gsiVar->Id->Data());
        VERIFY_ARE_EQUAL_STR(L"test_variantName", gsiVar->Name->Data());
        VERIFY_ARE_EQUAL(true, gsiVar->IsPublisher);
        VERIFY_ARE_EQUAL_INT(100, gsiVar->Rank);
        VERIFY_ARE_EQUAL_STR(L"e96582b8-d78d-49e4-afe8-8b7ea3d10806", gsiVar->SchemaId->Data());
        VERIFY_ARE_EQUAL_STR(L"test_variantSchemasContent", gsiVar->SchemaContent->Data());
        VERIFY_ARE_EQUAL_STR(L"test_variantSchemasName", gsiVar->SchemaName->Data());

        VERIFY_ARE_EQUAL_INT(result->GameServerImageSets->Size, 1);
        auto gsiSet = result->GameServerImageSets->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"test_gsiSetId", gsiSet->Id->Data());
        VERIFY_ARE_EQUAL_INT(300, gsiSet->MaxPlayers);
        VERIFY_ARE_EQUAL_INT(200, gsiSet->MinPlayers);
        VERIFY_ARE_EQUAL_STR(L"test_gsiSetName", gsiSet->Name->Data());
        VERIFY_ARE_EQUAL_INT(400, gsiSet->SelectionOrder);
        VERIFY_ARE_EQUAL_STR(L"e96582b8-d78d-49e4-afe8-8b7ea3d10806", gsiSet->SchemaId->Data());
        VERIFY_ARE_EQUAL_STR(L"test_variantSchemasContent", gsiSet->SchemaContent->Data());
        VERIFY_ARE_EQUAL_STR(L"test_variantSchemasName", gsiSet->SchemaName->Data());
    }

    DEFINE_TEST_CASE(TestGetGameServerMetadataAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetGameServerMetadataAsync);
        auto responseJson = web::json::value::parse(g_defaultGameServerMetadataResult);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        uint32 titleId = 1234;
        uint32 maxAllowedPlayers = 200;
        bool publisherOnly = true;
        uint32 maxVariants = 100;

        auto tagsMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
        tagsMap->Insert(L"test1", L"test1value");
        tagsMap->Insert(L"test2", L"test2value");

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->GameServerPlatformService->GetGameServerMetadataAsync(
            titleId,
            maxAllowedPlayers,
            publisherOnly,
            maxVariants,
            nullptr,
            tagsMap->GetView()
            )).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://gameserverds.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/variants", httpCall->PathQueryFragment.to_string());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(LR"({"locale":"en-us","maxAllowedPlayers":200,"maxVariants":100,"publisherOnly":true,"tags":[{"name":"test1","value":"test1value"},{"name":"test2","value":"test2value"}]})", httpCall->request_body().request_message_string());

        VerifyGameServerMetadataResult(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetQualityOfServiceServersAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetQualityOfServiceServersAsync);
        auto responseJson = web::json::value::parse(g_defaultQosServers);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->GameServerPlatformService->GetQualityOfServiceServersAsync()).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://gameserverds.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/xplatqosservers", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL(LR"()", httpCall->request_body().request_message_string());

        VERIFY_ARE_EQUAL_INT(2, result->Size);
        auto server1 = result->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"test_serverFqdn", server1->ServerFullQualifiedDomainName->Data());
        VERIFY_ARE_EQUAL_STR(L"test_serverSecureDeviceAddress", server1->SecureDeviceAddressBase64->Data());
        VERIFY_ARE_EQUAL_STR(L"test_targetLocation", server1->TargetLocation->Data());
    }

    DEFINE_TEST_CASE(TestInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestInvalidArgs);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterAsync(123, nullptr, "testSessionTemplateName", "testSessionName", true)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterAsync(123, "testServiceConfigurationId", nullptr, "testSessionName", true)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterAsync(123, "testServiceConfigurationId", "testSessionTemplateName", nullptr, true)).get(), InvalidArgumentException);
 
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, nullptr, "3", "4", "5", "6", 7, "8", false)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, "2", nullptr, "4", "5", "6", 7, "8", false)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, "2", "3", nullptr, "5", "6", 7, "8", false)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, "2", "3", "4", nullptr, "6", 7, "8", false)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, "2", "3", "4", "5", nullptr, 7, "8", false)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->AllocateClusterInlineAsync(1, "2", "3", "4", "5", "6", 7, nullptr, false)).get(), InvalidArgumentException);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->GameServerPlatformService->GetTicketStatusAsync(1, nullptr)).get(), InvalidArgumentException);

        VERIFY_NO_THROW(xboxLiveContext->GameServerPlatformService->GetGameServerMetadataAsync(1, 2, false, 3, nullptr));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

