// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/system.h"
#include "shared_macros.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

/// <summary>
/// Contains information about the authorization token and digital signature for an HTTP request by a user.
/// This class is returned as the result of a call to XboxLiveUser.GetTokenAndSignatureAsync.
/// </summary>
public ref class GetTokenAndSignatureResult sealed
{
public:

    /// <summary>
    /// The authorization token for the HTTP request.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Token, token)

    /// <summary>
    /// The digital signature for the HTTP request.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Signature, signature)

    /// <summary>
    /// The unique ID tied to the Xbox user's account.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id)

    /// <summary>
    /// The gamertag name associated with the Xbox user's account.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Gamertag, gamertag)

    /// <summary>
    /// The hashcode that identifies the user. This value is used for HTTP calls.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserHash, xbox_user_hash)

    /// <summary>
    /// The age group
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(AgeGroup, age_group)

    /// <summary>
    /// The privileges
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Privileges, privileges)

    /// <summary>
    /// Gets id of WebAccount returned by the Xbox live WebAccountProvider.
    /// </summary>
    /// <remarks>
    /// Check https://msdn.microsoft.com/en-us/library/windows/apps/windows.security.credentials.webaccount.aspx
    /// for more information about WebAccount
    /// </remarks>
    DEFINE_PROP_GET_STR_OBJ(WebAccountId, web_account_id)

internal:
    GetTokenAndSignatureResult(
        _In_ xbox::services::system::token_and_signature_result cppObj
        );

private:
    xbox::services::system::token_and_signature_result m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
