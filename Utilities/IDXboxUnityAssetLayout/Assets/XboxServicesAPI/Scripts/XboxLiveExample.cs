
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class XboxLiveExample : MonoBehaviour
{
#if ENABLE_WINMD_SUPPORT 
    Microsoft.Xbox.Services.System.XboxLiveUser m_user = new Microsoft.Xbox.Services.System.XboxLiveUser();

    Microsoft.Xbox.Services.XboxLiveContext m_xboxLiveContext = null;
    Windows.UI.Core.CoreDispatcher UIDispatcher = null;
#endif

    string debugText = "";

    // Use this for initialization 
    void Start()
    {
#if ENABLE_WINMD_SUPPORT 
        Windows.ApplicationModel.Core.CoreApplicationView mainView = Windows.ApplicationModel.Core.CoreApplication.MainView;
        Windows.UI.Core.CoreWindow cw = mainView.CoreWindow;
        UIDispatcher = cw.Dispatcher;
        SignIn();
#endif
    }

    // Update is called once per frame 
    void Update()
    {        
    }

    void OnGUI()
    {
        GUI.Label(new UnityEngine.Rect(10, 10, 300, 50), debugText);
    }

#if ENABLE_WINMD_SUPPORT 
    async void SignIn()
    {
        Microsoft.Xbox.Services.System.SignInResult result = await m_user.SignInAsync(UIDispatcher);

        if (result.Status == Microsoft.Xbox.Services.System.SignInStatus.Success)
        {
            m_xboxLiveContext = new Microsoft.Xbox.Services.XboxLiveContext(m_user);
            debugText += "\n User signed in: " + m_xboxLiveContext.User.Gamertag;
        }
    }
#endif
}

