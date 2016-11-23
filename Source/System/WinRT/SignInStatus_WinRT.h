//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>
/// Enumeration values that indicate the result status of sign in.
/// </summary>
public enum class SignInStatus
{
    /// <summary>
    /// Signed in successfully.
    /// </summary>
    Success = xbox::services::system::sign_in_status::success,

    /// <summary>
    /// Need to invoke the SignInAsync API (w/ UX) to let the user take necessary actions for the sign-in operation to continue.
    /// Can only be returned from SigninSilentlyAsync().
    /// </summary>
    UserInteractionRequired = xbox::services::system::sign_in_status::user_interaction_required,

    /// <summary>
    /// The user decided to cancel the sign-in operation.
    /// Can only be returned from SigninAsync().
    /// </summary>
    UserCancel = xbox::services::system::sign_in_status::user_cancel,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END