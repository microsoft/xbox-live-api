using System;
using System.Collections.Generic;
using Microsoft.Xbox.Services;
using Microsoft.Xbox.Services.System;
using Microsoft.Xbox.Services.Social.Manager;

public class SocialManagerIntegration
{
    Social.MainPage m_ui;
    SocialManager m_socialManager = SocialManager.SingletonInstance;
    List<XboxSocialUserGroup> m_socialUserGroups = new List<XboxSocialUserGroup>();
    string m_logText = string.Empty;
    List<string> m_logLines = new List<string>();

    public void Init(Social.MainPage ui)
    {
        m_ui = ui;
    }

    public void AddUser(XboxLiveUser user)
    {
        lock (m_socialManager)
        {
            m_ui.LogEvent(string.Format("Adding user {0} to SocialManager", user.Gamertag));
            m_socialManager.AddLocalUser(user, SocialManagerExtraDetailLevel.NoExtraDetail);
            m_ui.CreateSocialGroupsBasedOnUI(user);
        }
    }

    public void CreateOrUpdateSocialGroupFromList(XboxLiveUser user, List<string> xuidList)
    {

        foreach (XboxSocialUserGroup userGroup in m_socialUserGroups)
        {
            if (userGroup.LocalUser.XboxUserId == user.XboxUserId && userGroup.SocialUserGroupType == SocialUserGroupType.UserListType)
            {
                m_socialManager.UpdateSocialUserGroup(userGroup, xuidList);
                return;
            }
        }
        if ( xuidList.Count > 0 )
        {
            XboxSocialUserGroup socialUserGroup = m_socialManager.CreateSocialUserGroupFromList(user, xuidList);
            lock (m_socialManager)
            {
                m_socialUserGroups.Add(socialUserGroup);
            }
        }
    }

    public void DestorySocialGroupFromList(XboxLiveUser user)
    {
        lock (m_socialManager)
        {
            foreach (XboxSocialUserGroup socialUserGroup in m_socialUserGroups)
            {
                if (socialUserGroup.LocalUser.XboxUserId == user.XboxUserId &&
                    socialUserGroup.SocialUserGroupType == SocialUserGroupType.UserListType)
                {
                    m_socialUserGroups.Remove(socialUserGroup);
                    m_socialManager.DestroySocialUserGroup(socialUserGroup);
                    break;
                }
            }
        }
    }

    public void CreateSocialGroupFromFilters(XboxLiveUser user, PresenceFilter presenceFilter, RelationshipFilter relationshipFilter)
    {
        XboxSocialUserGroup socialUserGroup = m_socialManager.CreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter);
        lock (m_socialManager)
        {
            m_socialUserGroups.Add(socialUserGroup);
        }
    }

    public void DestroySocialGroup(XboxLiveUser user, PresenceFilter presenceFilter, RelationshipFilter relationshipFilter)
    {
        lock (m_socialManager)
        {
            foreach (XboxSocialUserGroup socialUserGroup in m_socialUserGroups)
            {
                if( socialUserGroup.LocalUser.XboxUserId == user.XboxUserId &&
                    socialUserGroup.PresenceFilterOfGroup == presenceFilter && 
                    socialUserGroup.RelationshipFilterOfGroup == relationshipFilter )
                {
                    m_socialUserGroups.Remove(socialUserGroup);
                    m_socialManager.DestroySocialUserGroup(socialUserGroup);
                    break;
                }
            }
        }
    }

    public void RemoveUser(XboxLiveUser user)
    {
        lock (m_socialManager)
        {
            List<XboxSocialUserGroup> socialUserGroups = new List<XboxSocialUserGroup>(m_socialUserGroups);
            foreach(XboxSocialUserGroup socialUserGroup in socialUserGroups)
            {
                if( socialUserGroup.LocalUser.XboxUserId == user.XboxUserId )
                {
                    m_socialUserGroups.Remove(socialUserGroup);
                }
            }

            m_ui.LogEvent(string.Format("Removing user {0} from SocialManager", user.Gamertag));
            m_socialManager.RemoveLocalUser(user);
        }
    }

    public void Update()
    {
        List<XboxSocialUserGroup> socialUserGroups = null;
        IReadOnlyList<SocialEvent> socialEventList = null;
        lock (m_socialManager)
        {
            socialUserGroups = m_socialUserGroups;
            socialEventList = m_socialManager.DoWork();
        }
        RenderSocialGroupList(socialUserGroups);
        LogSocialEventList(socialEventList);
    }

    void LogSocialEventList(IReadOnlyList<SocialEvent> socialEventList)
    {
        foreach (SocialEvent socialEvent in socialEventList)
        {
            if ( socialEvent.ErrorCode != 0 )
            {
                m_ui.LogEvent(string.Format("Event: {0} ErrorCode: 0x{1:X} ErrorMessage: {2}", 
                    socialEvent.EventType.ToString(), 
                    socialEvent.ErrorCode,
                    socialEvent.ErrorMessage));
            }
            else
            {
                string usersAffected = string.Empty;
                foreach( String u in socialEvent.UsersAffected )
                {
                    usersAffected += u;
                    usersAffected += ", ";
                }
                m_ui.LogEvent(string.Format("Event: {0} UserAffected: {1}", socialEvent.EventType.ToString(), usersAffected));
            }
        }
    }

    async void RenderSocialGroupList(List<XboxSocialUserGroup> socialUserGroups)
    {
        Windows.UI.Core.CoreDispatcher UIDispatcher = Windows.UI.Xaml.Window.Current.CoreWindow.Dispatcher;
        await UIDispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
            () =>
            {
                m_ui.ClearSocialGroup();

                foreach (XboxSocialUserGroup socialUserGroup in socialUserGroups)
                {
                    m_ui.AppendToSocialGroup("--------------------");
                    if( socialUserGroup.SocialUserGroupType == SocialUserGroupType.FilterType )
                    {
                        m_ui.AppendToSocialGroup(
                            string.Format("Group from filter: {0} {1}", socialUserGroup.PresenceFilterOfGroup.ToString(), socialUserGroup.RelationshipFilterOfGroup.ToString())
                            );
                    }
                    else
                    {
                        m_ui.AppendToSocialGroup("Group from custom list");
                    }

                    IReadOnlyList<XboxSocialUser> userList = socialUserGroup.Users;
                    foreach (XboxSocialUser socialUser in userList)
                    {
                        m_ui.AppendToSocialGroup("Gamertag: " + socialUser.Gamertag + ". Status: " + socialUser.PresenceRecord.UserState.ToString());
                    }

                    if (userList.Count == 0)
                    {
                        m_ui.AppendToSocialGroup("No friends found");
                    }
                }
            });
    }
}
