#include "pch.h"
#include "GameLogic\Game.h"

using namespace Sample;

void Game::HandleSignInResult(XblSignInResult signInResult)
{
    switch (signInResult.status)
    {
    case xbox::services::system::sign_in_status::success:
        XblUserGetXboxUserId(m_user, &m_xuid);
        XblContextCreateHandle(m_user, &m_xboxLiveContext);
        XblGetXboxLiveAppConfig(&m_config);
        Log(L"Sign in succeeded");
        break;

    case xbox::services::system::sign_in_status::user_cancel:
        Log(L"User cancel");
        break;

    case xbox::services::system::sign_in_status::user_interaction_required:
        Log(L"User interaction required");
        break;

    default:
        Log(L"Unknown error");
        break;
    }
}

void Game::SignInSilently()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblSignInResult signInResult;
        auto result = XblUserGetSignInResult(asyncBlock, &signInResult);

        if (SUCCEEDED(result))
        {
            pThis->HandleSignInResult(signInResult);
        }
        else
        {
            pThis->Log(L"Failed signing in.");
            return;
        }
        delete asyncBlock;
    };

    XblUserSignInSilently(asyncBlock, m_user);
}

void Game::SignIn()
{
    bool isSignedIn;
    XblUserIsSignedIn(m_user, &isSignedIn);
    if (isSignedIn)
    {
        Log(L"Already signed in.");
        return;
    }
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblSignInResult signInResult;
        auto result = XblUserGetSignInResult(asyncBlock, &signInResult);

        if (SUCCEEDED(result))
        {
            pThis->HandleSignInResult(signInResult);
        }
        else
        {
            pThis->Log(L"Failed signing in.");
            return;
        }
        delete asyncBlock;
    };

    XblUserSignInWithCoreDispatcher(
        asyncBlock,
        m_user,
        Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->CoreWindow->Dispatcher
    );
}

void Game::HandleSignout(xbl_user_handle user)
{
    WCHAR text[1024];
    char gamertag[GamertagMaxBytes];
    XblUserGetGamertag(user, GamertagMaxBytes, gamertag, nullptr);

    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", utility::conversions::utf8_to_utf16(gamertag).data());
    g_sampleInstance->Log(text);
}