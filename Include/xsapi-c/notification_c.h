// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once
extern "C"
{
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
/// <summary>
/// Sets the device/registration token received from GNS or APNS and subscribes the title to push notifications.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="asyncBlock">Caller allocated AsyncBlock.</param>
/// <param name="deviceToken">The device/registration token received from GNS/APNS</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblNotificationSubscribeToNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock,
    _In_ const char* deviceToken
) XBL_NOEXCEPT;
#endif

#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_UWP
/// <summary>
/// Unsubscribes the title from push notifications.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="asyncBlock">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblNotificationUnsubscribeFromNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;
#endif
}