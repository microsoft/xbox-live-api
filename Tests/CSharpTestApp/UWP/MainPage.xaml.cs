//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Storage;
using Windows.Storage.Pickers;

using System.Threading;
using System.Threading.Tasks;
using Microsoft.Xbox.Services;
using Microsoft.Xbox.Services.System; 
using Microsoft.Xbox.Services.Social.Manager;

namespace Social
{
    public sealed partial class MainPage : Page
    {
        Dictionary<string, XboxLiveContext> m_xboxliveContexts = new Dictionary<string, XboxLiveContext>();
        SocialManagerIntegration m_socialManagerUI = new SocialManagerIntegration();
        List<string> m_xuidList = new List<string>();
        Windows.UI.Core.CoreDispatcher UIDispatcher = null;
        bool m_allFriends = false;
        bool m_onlineFriends = false;
        bool m_allFavs = false;
        bool m_onlineInTitle = false;
        bool m_customList = false;
        bool m_isMultiUserApplication = false;

        public MainPage()
        {
            this.InitializeComponent();

            UIDispatcher = Windows.UI.Xaml.Window.Current.CoreWindow.Dispatcher;

            XboxLiveUser.SignOutCompleted += XboxLiveUser_SignOutCompleted;
            Windows.Networking.Connectivity.NetworkInformation.NetworkStatusChanged += new Windows.Networking.Connectivity.NetworkStatusChangedEventHandler(
                (object sender) =>
            {
                UpdateInternetAccessUI();
            });

            bool APIExist = Windows.Foundation.Metadata.ApiInformation.IsMethodPresent("Windows.System.UserPicker", "IsSupported");
            m_isMultiUserApplication = APIExist && Windows.System.UserPicker.IsSupported();

            // Hide switch user button as it is not supported on multi-user application
            if (m_isMultiUserApplication)
            {
                SwitchAccountButton.Visibility = Visibility.Collapsed;
            }

            // Only show the file picker for PC
            var deviceFamily = Windows.System.Profile.AnalyticsInfo.VersionInfo.DeviceFamily;
            if (deviceFamily != "Windows.Desktop")
            {
                SocialGroup_CustomList.Visibility = Visibility.Collapsed;
                ImportSocialGroup_List.Visibility = Visibility.Collapsed;
            }

            SetupSignInUI();

            m_socialManagerUI.Init(this);
            CompositionTarget.Rendering += CompositionTarget_Rendering;

            UpdateInternetAccessUI();
            SignInSilently();

            ReadLastCsv();
        }

        private void SetupSignInUI()
        {
            var uiSettings = Microsoft.Xbox.Services.XboxLiveAppConfiguration.SingletonInstance.AppSignInUISettings;
            uiSettings.TitleCategory = Microsoft.Xbox.Services.SignInUIGameCategory.Casual;
            uiSettings.BackgroundHexColor = "0F2C55";
            uiSettings.AddEmphasisFeature(Microsoft.Xbox.Services.SignInUIEmphasisFeature.Achievements);
            uiSettings.AddEmphasisFeature(Microsoft.Xbox.Services.SignInUIEmphasisFeature.ConnectedStorage);
            uiSettings.AddEmphasisFeature(Microsoft.Xbox.Services.SignInUIEmphasisFeature.FindPlayers);

            var installDir = Windows.ApplicationModel.Package.Current.InstalledLocation;
            using (FileStream fs = File.OpenRead(installDir.Path + "\\Assets\\background.jpg"))
            {
                byte[] image = new byte[fs.Length];
                fs.Read(image, 0, (int)fs.Length);
                uiSettings.SetBackgroundImage(image);
            }
        }

        private async void ReadLastCsv()
        {
            if( Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.Entries.Count > 0)
            {
                var path = Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.Entries[0];
                StorageFile file = await Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.GetFileAsync(path.Token);
                ReadCsvFile(file);
            }
        }

        private void CompositionTarget_Rendering(object sender, object e)
        {
            m_socialManagerUI.Update();
        }

        private async void XboxLiveUser_SignOutCompleted(object sender, SignOutCompletedEventArgs args)
        {
            await UIDispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    m_xboxliveContexts.Remove(args.User.WindowsSystemUser.NonRoamableId);
                    UserInfoLabel.Text = "User signed out";
                    Log("User " + args.User.Gamertag + " signed out.");
                    m_socialManagerUI.RemoveUser(args.User);
                });
        }

        private void SignInButton_Click(object sender, RoutedEventArgs e)
        {
            SignIn();
        }

        private void SwitchAccountButton_Click(object sender, RoutedEventArgs e)
        {
            SwitchAccount();
        }

        void ClearLogs()
        {
            OutputStackPanel.Children.Clear();
        }

        public void Log(string logLine)
        {
            TextBlock uiElement = new TextBlock();
            uiElement.FontSize = 14;
            uiElement.Text = logLine;
            OutputStackPanel.Children.Add(uiElement);
            OutputScroll.ChangeView(null, OutputScroll.ScrollableHeight + 200, null);
        }

        public void ClearSocialGroup()
        {
            SocialGroupStackPanel.Children.Clear();
        }

        public void AppendToSocialGroup(string logLine)
        {
            TextBlock uiElement = new TextBlock();
            uiElement.FontSize = 14;
            uiElement.Text = logLine;
            SocialGroupStackPanel.Children.Add(uiElement);
        }

        private async Task<List<Windows.System.User>> GetAllSystemUser()
        {
            var allUser = await Windows.System.User.FindAllAsync();
            return allUser.Where(user => (user.Type != Windows.System.UserType.LocalGuest || user.Type != Windows.System.UserType.RemoteGuest)).ToList();
        }

        async void SignIn()
        {
            UserInfoLabel.Text = "Trying to sign in...";
            Log(UserInfoLabel.Text);

            var allSysUser = await GetAllSystemUser();
            Log(string.Format("Signing in {0} user(s)", allSysUser.Count));
            allSysUser.ForEach(async user =>
            {
                try
                {
                    XboxLiveUser xboxLiveUser = new XboxLiveUser(user);
                    var signInResult = await xboxLiveUser.SignInAsync(UIDispatcher);
                    HandleSignInResult(signInResult, xboxLiveUser);
                }
                catch (Exception e)
                {
                    Log("SignInAsync failed.  Exception: " + e.ToString());
                }
            });
        }

        async void SignInSilently()
        {
            UserInfoLabel.Text = "Trying to sign in silently...";
            Log(UserInfoLabel.Text);

            var allSysUser = await GetAllSystemUser();
            Log(string.Format("Signing in {0} user(s)", allSysUser.Count));
            allSysUser.ToList().ForEach(async user =>
            {
                try
                {
                    XboxLiveUser xboxLiveUser = new XboxLiveUser(user);
                    var signInResult = await xboxLiveUser.SignInSilentlyAsync(UIDispatcher);
                    HandleSignInResult(signInResult, xboxLiveUser);
                }
                catch (Exception e)
                {
                    Log("SignInSilentlyAsync failed.  Exception: " + e.ToString());
                }
            });
        }

        async void SwitchAccount()
        {
            UserInfoLabel.Text = "Trying to Switch Account...";
            Log(UserInfoLabel.Text);

            // SwtichAccount only support Single User Application, so there will be only one user
            
            try
            {
                var userToSwitch = m_xboxliveContexts.First().Value.User;

                var signInResult = await userToSwitch.SwitchAccountAsync(Windows.UI.Xaml.Window.Current.CoreWindow.Dispatcher);
                HandleSignInResult(signInResult, userToSwitch);
            }
            catch (Exception e)
            {
                Log("SwitchAccountAsync failed.  Exception: " + e.ToString());
            }
        }

        public async void LogEvent(String str)
        {
            await UIDispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    Log(str);
                });
        }

        private bool CheckInternetAccess()
        {
            bool internetAccess = false;
            Windows.Networking.Connectivity.ConnectionProfile connectionProfile = Windows.Networking.Connectivity.NetworkInformation.GetInternetConnectionProfile();

            if (connectionProfile != null)
            {
                var connectivityLevel = connectionProfile.GetNetworkConnectivityLevel();
                if (connectivityLevel == Windows.Networking.Connectivity.NetworkConnectivityLevel.ConstrainedInternetAccess ||
                    connectivityLevel == Windows.Networking.Connectivity.NetworkConnectivityLevel.InternetAccess)
                {
                    internetAccess = true;
                }
            }

            return internetAccess;
        }

        private async void UpdateInternetAccessUI()
        {
            String newNetworkInfoTest = CheckInternetAccess() ? "Yes" : "No Internet";

            // Dispatch UI change to UI thread
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                NetworkInfoLabel.Text = newNetworkInfoTest;
                Log("Network connectivity changed to " + newNetworkInfoTest);
            });
        }

        private void HandleSignInResult(SignInResult signInResult, XboxLiveUser user)
        {
            switch (signInResult.Status)
            {
                case SignInStatus.Success:
                    var xboxLiveContext = new Microsoft.Xbox.Services.XboxLiveContext(user);
                    UserInfoLabel.Text = "Sign in succeeded";
                    m_xboxliveContexts.Add(user.WindowsSystemUser.NonRoamableId, xboxLiveContext);
                    m_socialManagerUI.AddUser(user);
                    break;

                case SignInStatus.UserCancel:
                    UserInfoLabel.Text = "User cancel";
                    break;

                case SignInStatus.UserInteractionRequired:
                    UserInfoLabel.Text = "User interaction required";
                    break;

                default:
                    UserInfoLabel.Text = "Unknown Error";
                    break;
            }
            Log(UserInfoLabel.Text);
        }

        void UpdateSocialGroupForAllUsers(bool create, PresenceFilter presenceFilter, RelationshipFilter relationshipFilter)
        {
            foreach (var xboxLiveContext in m_xboxliveContexts)
            {
                UpdateSocialGroup(xboxLiveContext.Value.User, create, presenceFilter, relationshipFilter);
            }
        }

        void UpdateSocialGroupOfListForAllUsers(bool create)
        {
            foreach (var xboxLiveContext in m_xboxliveContexts)
            {
                UpdateSocialGroupOfList(xboxLiveContext.Value.User, create);
            }
        }

        void UpdateSocialGroup(XboxLiveUser user, bool create, PresenceFilter presenceFilter, RelationshipFilter relationshipFilter)
        {
            if (create)
            {
                m_socialManagerUI.CreateSocialGroupFromFilters(user, presenceFilter, relationshipFilter);
            }
            else
            {
                m_socialManagerUI.DestroySocialGroup(user, presenceFilter, relationshipFilter);
            }
        }

        void UpdateSocialGroupOfList(XboxLiveUser user, bool create)
        {
            if (create)
            {
                m_socialManagerUI.CreateOrUpdateSocialGroupFromList(user, m_xuidList);
            }
        }

        public void CreateSocialGroupsBasedOnUI(XboxLiveUser user)
        {
            UpdateSocialGroup(user, m_allFriends, PresenceFilter.All, RelationshipFilter.Friends);
            UpdateSocialGroup(user, m_onlineFriends, PresenceFilter.AllOnline, RelationshipFilter.Friends);
            UpdateSocialGroup(user, m_allFavs, PresenceFilter.All, RelationshipFilter.Favorite);
            UpdateSocialGroup(user, m_onlineInTitle, PresenceFilter.TitleOnline, RelationshipFilter.Friends);
            UpdateSocialGroupOfList(user, m_customList);
        }

        private void SocialGroup_AllFriends_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            bool valueChanged = (m_allFriends != checkBox.IsChecked.Value);
            if (valueChanged)
            {
                m_allFriends = checkBox.IsChecked.Value;
                UpdateSocialGroupForAllUsers(checkBox.IsChecked.Value, PresenceFilter.All, RelationshipFilter.Friends);
            }
        }

        private void SocialGroup_OnlineFriends_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            bool valueChanged = (m_onlineFriends != checkBox.IsChecked.Value);
            if (valueChanged)
            {
                m_onlineFriends = checkBox.IsChecked.Value;
                UpdateSocialGroupForAllUsers(checkBox.IsChecked.Value, PresenceFilter.AllOnline, RelationshipFilter.Friends);
            }
        }

        private void SocialGroup_AllFavs_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            bool valueChanged = (m_allFavs != checkBox.IsChecked.Value);
            if (valueChanged)
            {
                m_allFavs = checkBox.IsChecked.Value;
                UpdateSocialGroupForAllUsers(checkBox.IsChecked.Value, PresenceFilter.All, RelationshipFilter.Favorite);
            }
        }

        private void SocialGroup_OnlineInTitle_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            bool valueChanged = (m_onlineInTitle != checkBox.IsChecked.Value);
            if (valueChanged)
            {
                m_onlineInTitle = checkBox.IsChecked.Value;
                UpdateSocialGroupForAllUsers(checkBox.IsChecked.Value, PresenceFilter.AllTitle, RelationshipFilter.Friends);
            }
        }

        private void SocialGroup_CustomList_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            bool valueChanged = (m_customList != checkBox.IsChecked.Value);
            if (valueChanged)
            {
                m_customList = checkBox.IsChecked.Value;
                UpdateSocialGroupOfListForAllUsers(checkBox.IsChecked.Value);
            }
        }

        private async void ImportSocialGroup_List_Click(object sender, RoutedEventArgs e)
        {
            LogEvent("Dev accounts CSV can be exported from XDP");
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.List;
            openPicker.SuggestedStartLocation = PickerLocationId.Desktop;
            openPicker.FileTypeFilter.Add(".csv");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.AddOrReplace("LastUsedFile", file);
                ReadCsvFile(file);
            }
        }

        async void ReadCsvFile(StorageFile file)
        {
            LogEvent(string.Format("Reading {0}", file.Path));
            var lines = await FileIO.ReadLinesAsync(file);
            int count = 0;
            m_xuidList = new List<string>();
            foreach (string line in lines)
            {
                count++;
                if (count == 1)
                {
                    continue; // skip header
                }

                string[] items = line.Split(',');
                if (items.Length > 4)
                {
                    string xuid = items[3];
                    xuid = xuid.Replace("\"", "").Replace("=", "");
                    m_xuidList.Add(xuid);
                    LogEvent(string.Format("Read from CSV: {0}", xuid));
                }
            }

            // Refresh custom list if its active
            if (m_customList)
            {
                UpdateSocialGroupOfListForAllUsers(true);
            }
        }
    }
}

