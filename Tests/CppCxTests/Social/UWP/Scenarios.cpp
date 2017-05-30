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
#include "Scenarios.h"
#include "MainPage.xaml.h"
#include <ppltasks.h>
#include <mutex>

using namespace Social_CppCx_140;
using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Authentication::OnlineId;

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Social;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Presence;

std::mutex g_blockOfTextLock;

void Scenarios::Active_RealTimeActivity(_In_ MainPage^ mainPage, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
{
    mainPage->Log(L"Activating calling get_user_profile...");

    xboxLiveContext->RealTimeActivityService->Activate();

    xboxLiveContext->PresenceService->SubscribeToDevicePresenceChange(
        xboxLiveContext->User->XboxUserId
        );

    xboxLiveContext->PresenceService->SubscribeToTitlePresenceChange(
		xboxLiveContext->User->XboxUserId,
        0x5D2A2BCA
        );

    EventHandler<DevicePresenceChangeEventArgs^>^ devicePresenceChangeEvent = ref new EventHandler<DevicePresenceChangeEventArgs^>(
    [this, mainPage](Platform::Object^, DevicePresenceChangeEventArgs^ eventArgs)
    {
        OnDevicePresenceChange(mainPage, eventArgs);
    });
    m_devicePresenceChangeEventToken = xboxLiveContext->PresenceService->DevicePresenceChanged += devicePresenceChangeEvent;

    EventHandler<TitlePresenceChangeEventArgs^>^ titlePresenceChangeEvent = ref new EventHandler<TitlePresenceChangeEventArgs^>(
    [this, mainPage](Platform::Object^, TitlePresenceChangeEventArgs^ eventArgs)
    {
        OnTitlePresenceChange(mainPage, eventArgs);
    });
    m_titlePresenceChangeEventToken = xboxLiveContext->PresenceService->TitlePresenceChanged += titlePresenceChangeEvent;
}

void Scenarios::Scenario_GetUserProfileAsync(_In_ MainPage^ ui, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
{
    ui->Log(L"Calling get_user_profile...");

    TimeSpan giveUpDuration;
    giveUpDuration.Duration = 10 * 10000000i64; // 10 seconds
    xboxLiveContext->Settings->HttpRetryDelay = giveUpDuration;
    xboxLiveContext->Settings->EnableServiceCallRoutedEvents = true;

    auto asyncOp = xboxLiveContext->ProfileService->GetUserProfileAsync(xboxLiveContext->User->XboxUserId);
    create_task(asyncOp)
    .then([this, ui](task<XboxUserProfile^> resultTask)
    {
        try
        {
            XboxUserProfile^ profile = resultTask.get();
            String^ gameDisplayPicResizeUri;
            if (profile->GameDisplayPictureResizeUri != nullptr)
            {
                // Retrieve small image size by appending size query strings.
                gameDisplayPicResizeUri = String::Concat(profile->GameDisplayPictureResizeUri->AbsoluteUri, L"&w=64&h=64");
            }

            std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
            ui->LogFormat(L"");
            ui->LogFormat(L"----------------");
            ui->LogFormat(L"get_user_profile result:");
            ui->LogFormat(L"app_display_name: %S", profile->ApplicationDisplayName->ToString()->Data());
            //ui->LogFormat(L"app_display_picture_resize_uri: %s", gameDisplayPicResizeUri->Data());
            ui->LogFormat(L"game_display_name: %S", profile->GameDisplayName->ToString()->Data());
            ui->LogFormat(L"game_display_picture_resize_uri: %S", gameDisplayPicResizeUri->Data());
            ui->LogFormat(L"gamerscore: %S", profile->Gamerscore->Data());
            ui->LogFormat(L"gamertag: %S", profile->Gamertag->Data());
            ui->LogFormat(L"xbox_user_id: %S", profile->XboxUserId->Data());
        }
        catch (Platform::Exception^ ex)
        {
            ui->LogFormat(L"Error calling GetUserProfileAsync function: 0x%0.8x", ex->HResult);
        }
    });
}

void Scenarios::Scenario_GetSocialRelationshipsAsync(_In_ MainPage^ ui, Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext)
{
    ui->Log(L"Calling get_social_relationships...");

    // Select 10 friends to display.
    // The service can override the user specified maximum to what it supports
    const int startIndex = 0;
    const int maxItems = 10;

    auto pAsyncOp = xboxLiveContext->SocialService->GetSocialRelationshipsAsync(
        SocialRelationship::All,
        startIndex,
        maxItems
        );

    create_task(pAsyncOp)
        .then([this, maxItems, ui](task<XboxSocialRelationshipResult^> resultTask)
    {
        int itemCount = 0;
        try
        {
            XboxSocialRelationshipResult^ result = resultTask.get();
            std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
            ui->LogFormat(L"");
            ui->LogFormat(L"----------------");
            ui->LogFormat(L"get_social_relationships results:");
            ui->LogFormat(L"total_count: %d", result->TotalCount);
            int index = 1;
            IVectorView<XboxSocialRelationship ^>^ list = result->Items;

            for (UINT index = 0; index < list->Size; index++)
            {
                XboxSocialRelationship ^ relationship = list->GetAt(index);
                ui->LogFormat(L"");
                ui->LogFormat(L"[Item %d]", index);
                ui->LogFormat(L"xbox_user_id: %S", relationship->XboxUserId->Data());
                ui->LogFormat(L"is_favorite: %d", relationship->IsFavorite.ToString()->Data());
                ui->LogFormat(L"is_following_caller: %d", relationship->IsFollowingCaller.ToString()->Data());
                index++;
            }
        }
        catch (Platform::Exception^ ex)
        {    
            ui->LogFormat(L"get_social_relationships failed:  0x%0.8x", ex->HResult);
        }
    });
}

void
Scenarios::OnDevicePresenceChange(_In_ MainPage^ ui, _In_ DevicePresenceChangeEventArgs^ args)
{
    std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);

    ui->LogFormat(L"");
    ui->LogFormat(L"----------------");
    ui->LogFormat(L"device presence changed:");
    ui->LogFormat(L"Xbox User ID: %S:", args->XboxUserId->ToString()->Data());
    ui->LogFormat(L"Device Type: %d", args->DeviceType.ToString()->Data());
    ui->LogFormat(L"Is user logged in: %S", args->IsUserLoggedOnDevice ? L"True" : L"False");
}

void
Scenarios::OnTitlePresenceChange(_In_ MainPage^ ui, _In_ TitlePresenceChangeEventArgs^ args)
{
    std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);

    ui->LogFormat(L"");
    ui->LogFormat(L"----------------");
    ui->LogFormat(L"title presence changed:");
    ui->LogFormat(L"Xbox User ID: %S:", args->XboxUserId->ToString()->Data());
    ui->LogFormat(L"Title State: %d", args->TitleState);
    ui->LogFormat(L"Title Id %d", args->TitleId);
}
