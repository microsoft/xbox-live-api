#pragma once

extern "C"
{

/// <summary>
/// Returns the system user associated with the user handle.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="systemUser">The associated system user.</param>
STDAPI XalUserToXboxSystemUser(
    _In_ XalUserHandle user,
    _Out_ Windows::Xbox::System::IUser^* systemUser
) noexcept;

/// <summary>
/// Add the given system user as a Xal user.
/// </summary>
/// <param name="systemUser">The system user.</param>
/// <param name="user">The new user handle.</param>
STDAPI XalAddXboxSystemUser(
    _In_ Windows::Xbox::System::IUser^ systemUser,
    _Out_ XalUserHandle* user
) noexcept;

}
