// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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