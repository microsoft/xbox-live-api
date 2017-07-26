// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubType_WinRT.h"
#include "ClubActionSettings_WinRT.h"
#include "ClubProfile_WinRT.h"
#include "ClubRoster_WinRT.h"
#include "ClubUserPresenceRecord_WinRT.h"
#include "ClubPresenceCounts_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class Club sealed
{
public:  
    /// <summary>ClubId</summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>Type (visibility) of club</summary>
    DEFINE_PROP_GET_ENUM_OBJ(Type, type, ClubType);

    /// <summary>When the club was created.</summary>
    DEFINE_PROP_GET_DATETIME_OBJ(Created, created);

    /// <summary>Club owner's Xuid</summary>
    DEFINE_PROP_GET_STR_OBJ(Owner, owner);

    /// <summary>Title family Id</summary>
    DEFINE_PROP_GET_STR_OBJ(TitleFamilyId, title_family_id);

    /// <summary>Localized Title Family Name</summary>
    DEFINE_PROP_GET_STR_OBJ(TitleFamilyName, title_family_name);

    /// <summary>Club short name</summary>
    DEFINE_PROP_GET_STR_OBJ(ShortName, short_name);

    /// <summary>Configurable club attributes</summary>
    property ClubProfile^ Profile 
    {
        ClubProfile^ get();
    }

    /// <summary>Club's display image url</summary>
    DEFINE_PROP_GET_STR_OBJ(GlyphImageUrl, glyph_image_url);

    /// <summary>Club's background image url</summary>
    DEFINE_PROP_GET_STR_OBJ(BannerImageUrl, banner_image_url);

    /// <summary>Whether or not the club is currently suspended.</summary>
    DEFINE_PROP_GET_OBJ(IsClubSuspended, is_club_suspended, bool);

    /// <summary>When the club remains suspended until.</summary>
    DEFINE_PROP_GET_DATETIME_OBJ(SuspendedUntil, suspended_until);

    /// <summary>Club founder's Xuid.</summary>
    DEFINE_PROP_GET_STR_OBJ(FounderXuid, founder_xuid);

    /// <summary>Number of followers of the club.</summary>
    DEFINE_PROP_GET_OBJ(FollowersCount, followers_count, uint32);

    /// <summary>Count of members present in the club.</summary>
    DEFINE_PROP_GET_OBJ(PresenceCount, presence_count, uint32);

    /// <summary>Count of members present in the club today.</summary>
    DEFINE_PROP_GET_OBJ(PresenceTodayCount, presence_today_count, uint32);

    /// <summary>Number of club members.</summary>
    DEFINE_PROP_GET_OBJ(MembersCount, members_count, uint32);

    /// <summary>Number of club moderators.</summary>
    DEFINE_PROP_GET_OBJ(ModeratorsCount, moderators_count, uint32);

    /// <summary>Number of users recommended for the club.</summary>
    DEFINE_PROP_GET_OBJ(RecommendedCount, recommended_count, uint32);

    /// <summary>Number of users requesting to join the club.</summary>
    DEFINE_PROP_GET_OBJ(RequestedToJoinCount, requested_to_join_count, uint32);

    /// <summary>Number of reports for the club.</summary>
    DEFINE_PROP_GET_OBJ(ReportCount, report_count, uint32);

    /// <summary>Number of reported items for the club.</summary>
    DEFINE_PROP_GET_OBJ(ReporteItemsCount, reported_items_count, uint32);

    /// <summary> 
    /// Settings dictating what actions users can take within the club depending on their role.
    /// </summary> 
    property ClubActionSettings^ ActionSettings
    {
        ClubActionSettings^ get();
    }

    /// <summary>Roles the viewer has in the club</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRole>^ ViewerRoles
    {
        Windows::Foundation::Collections::IVectorView<ClubRole>^ get();
    }

    /// <summary> 
    /// Retrieves a roster of club members that have a special relationship with the club (something other than just member).
    /// </summary>
    /// <returns>
    /// A ClubRoster object which contains the moderators, recommended users, banned users, and users 
    /// who have requested to join the club.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids{clubId}/decoration/roster
    /// </remarks>
    Windows::Foundation::IAsyncOperation<ClubRoster^>^ GetRosterAsync();

    /// <summary>Gets details about (at most) the last 1000 members active within a club.</summary>
    /// <param name="clubId">The club ID of the club.</param>
    /// <returns>An IVectorView containing ClubUserPresenceRecord describing the presence of the club's members.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids{clubId}/decoration/clubpresence
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubUserPresenceRecord^>^>^ GetClubUserPresenceRecordsAsync();

    /// <summary>
    /// Renames the club. Can only be called by the club owner. Some restrictions apply when renaming and public or 
    /// private club; hidden clubs can be renamed freely. Note that the club object this is called from will not be 
    /// updated with the new name even if this method is successful, but if the club object is retrieved again the 
    /// new name will be reflected.
    ///</summary>
    /// <param name="newName">New name to give the club</param>
    /// <remarks>
    /// Calls V1 POST clubaccounts.xboxlive.com/clubs/{clubId}
    /// </remarks>
    Windows::Foundation::IAsyncAction^ RenameClubAsync(
        _In_ Platform::String^ newName
        );

    /// <summary>Returns the roles a given user has within the club.</summary>
    /// <param name="xuid">Xuid of the user</param>
    /// <returns>
    /// An IVectorView containing the ClubRoles the user has within the club. If the user has no association with 
    /// the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ GetRolesAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary>Returns records of the callers roles within the club.</summary>
    /// <returns>
    /// A vector containing the club_role_records which describe the history of a users roles in the club.
    /// If the user has no association with the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^>^ GetRoleRecordsAsync();

    /// <summary>Returns records for a given users roles within the club.</summary>
    /// <param name="xuid">Xuid of the user whose roles to query</param>
    /// <returns>
    /// A vector containing the club_role_records which describe the history of a users roles in the club.
    /// If the user has no association with the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^>^ GetRoleRecordsAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary>Gets information about club member presence.</summary>
    /// <returns>A ClubPresenceCounts object indicating the number of club members present currently/today</returns>
    /// <remarks>
    /// Calls V1 GET clubpresence.xboxlive.com/clubs/{clubId}/users/count
    /// </remarks>
    Windows::Foundation::IAsyncOperation<ClubPresenceCounts^>^ GetPresenceCountsAsync();

    /// <summary>
    /// Sets the user's presence within the club. This keeps track of which club page, if any, 
    /// the user is currently viewing.
    /// </summary>
    /// <param name="xuid">Xuid of the user to set.</param>
    /// <param name="presence">The current presence state of the user</param>
    /// <remarks>
    /// Calls V1 POST clubpresence.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    Windows::Foundation::IAsyncAction^ SetPresenceWithinClubAsync(
        _In_ ClubUserPresence presence
        );

    /// <summary> 
    /// Attempts to join a club. If the user has not yet been invited, this call will send a request to join 
    /// the club which will need to be approved by a moderator before the caller becomes a member.
    /// </summary>
    /// <returns>An IVectorView containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 PUT clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ AddUserToClubAsync();

    /// <summary> 
    /// Deletes all relationships the calling user has with a club including: membership, outstanding invitations, 
    /// recommendations to join, and following.
    /// </summary>
    /// <returns>
    /// An IVectorView containing the roles the calling user now has with the club. If the call succeeds,
    /// it will not contain any role other than "banned".
    /// </returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ RemoveUserFromClubAsync();

    /// <summary>Adds the calling user as a follower of the specified club.</summary>    
    /// <returns>An IVectorView containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ FollowClubAsync();

    /// <summary>Removes the calling user as a follower of the specified club.</summary>
    /// <returns>An IVectorView containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Follower
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ UnfollowClubAsync();

    /// <summary> 
    /// Depending on the role of the caller, this function will have a different effect:
    /// 1) If the caller is a moderator of the club, the target user will be either invited or added as
    /// a club member immediately. In order for the target user to be added immediately, they have
    /// to have previously requested to join using the AddUserToClubAsync method. Moderators can examine the roster 
    /// to get a list of users who have requested to join.
    /// 2) If the caller is a member of the club, but not a moderator, the target user will be recommended
    /// for the club. For the user to be finally added as a member, AddUserToClubAsync still must be called 
    /// by a club moderator AND AddUserToClubAsync must be called by the target user.
    /// Note: If the club doesn't support recommendations, an error will be returned.
    /// 3) If the target user is the caller, this method will have the same exact behavior as AddUserToClubAsync()
    /// </summary>
    /// <param name="xuid">The xuid of the user to add.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 PUT clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ AddUserToClubAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary> 
    /// Removes all relationships between the target user and the club including: membership, outstanding
    /// invitations/recommendations, and following. Can only be called by club moderator (or on yourself, 
    /// which has the same effect as calling RemoveUserFromClubAsync())
    /// </summary>
    /// <param name="xuid">The xuid of the user to remove.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ RemoveUserFromClubAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary>Bans the specified user from the club. Can only be called by club moderators or owner.</summary>
    /// <param name="xuid">The xuid of the user to ban.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ BanUserFromClubAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary> 
    /// Unbans the specified user from the club. Can only be called by club moderators or owner.
    /// Note: A similar affect can also be achieved by a moderator calling add_user_to_club. In that
    /// case however, banned users will immediately receive an invitation to the club in addition to
    /// being unbanned.
    /// </summary>
    /// <param name="xuid">The xuid of the user to ban.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Banned
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ UnbanUserFromClubAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary> 
    /// Promotes the specified user to moderator of the club. The specified user must already be a
    /// member of the club. Can only be called by the club owner.
    /// </summary>
    /// <param name="xuid">The xuid of the user to promote.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ AddClubModeratorAsync(
        _In_ Platform::String^ xuid
        );

    /// <summary> 
    /// Removes the specified user as a moderator of the club. They will retain there other roles
    /// within the club. Can only be called by the club owner.
    /// </summary>
    /// <param name="xuid">The xuid of the user to demote.</param>
    /// <returns>An IVectorView containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Moderator
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ClubRole>^>^ RemoveClubModeratorAsync(
        _In_ Platform::String^ xuid
        );

internal:
    Club(_In_ xbox::services::clubs::club cppObj);
    
private:
    xbox::services::clubs::club m_cppObj;

    ClubProfile^ m_profile;
    ClubActionSettings^ m_actionSettings;
    Windows::Foundation::Collections::IVector<ClubRole>^ m_viewerRoles;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END