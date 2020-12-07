#pragma once

#include <Xal/xal_platform_types.h>
#include <Xal/grts_base/impersonation_ctx.h>
#include <Xal/grts_base/nt_user_context.h>

//-----------------------------------------------------------------------------
// MSA Account Picker / Web Account Manager UI

/// <summary>
/// Show MSA Account Picker / Web Account Manager UI event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Optional pointer to the client provided user
/// specific context.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="msaWamUiMode">The UI mode to launch WAM.</param>
/// <param name="scope">The scope to request to MSA.</param>
/// <param name="clientId">The client ID requesting the MSA ticket.</param>
/// <param name="msaUserId">Optional MSA User ID to request a ticket.</param>
/// <returns></returns>
/// <remarks>
///
/// </remarks>
typedef void (XalPlatformMsaWamUiEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_ MsaWamUiMode msaWamUiMode,
    _In_z_ char const* scope,
    _In_z_ char const* clientId,
    _In_opt_z_ char const* msaUserId
);

typedef struct XalPlatformHooks
{
    XalPlatformWebShowUrlEventHandler* showUrl;
    XalPlatformMsaWamUiEventHandler* showMsaWamUi;
    void* clientOpsCtx;
    Xal::Platform::Grts::ImpersonationCtx* impersonationCtx;
    Xal::Platform::Grts::NtUserContext* ntCtx;
} XalPlatformHooks;
