//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi-cpp/title_callable_ui.h"
#include "a/java_interop.h"
#include "a/jni_utils.h"
#include "TCUI/Android/title_callable_ui_jni.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

bool title_callable_ui_internal::s_isTcuiRunning = false;
pplx::task_completion_event<int32_t> title_callable_ui_internal::s_tcuiEventCompleted;

void
title_callable_ui_internal::tcui_completed_callback(JNIEnv *, jclass, int errorCode)
{
    s_tcuiEventCompleted.set(errorCode);
}

xbox_live_result<std::shared_ptr<java_interop>>
tcui_init()
{
    if (title_callable_ui_internal::s_isTcuiRunning)
    {
        return xbox_live_result<std::shared_ptr<java_interop>>(xbox_live_result<std::shared_ptr<java_interop>>(
            xbox_live_error_code::logic_error,
            _T("Previous tcui operation has not been completed"))
            );
    }

    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    if (interop == nullptr)
    {
        return xbox_live_result<std::shared_ptr<java_interop>>(xbox_live_result<std::shared_ptr<java_interop>>(
            xbox_live_error_code::logic_error,
            _T("XSAPI has not been initialized with the JVM"))
            );
    }

    title_callable_ui_internal::s_tcuiEventCompleted = task_completion_event<int32_t>();
    return interop;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_profile_card_ui(
    _In_ const string_t& targetXboxUserId,
    _In_ xbox_live_user_t user
)
{
    XblAddServiceCallRoutedHandler(
        [](XblServiceCallRoutedArgs args, void* context)
        {

        LOG_DEBUG(args.fullResponseFormatted);
        },
        nullptr
    );

    auto interopResult = tcui_init();

    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
                )
            );
    }
    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
    .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        (void) java_interop::get_java_interop_singleton()->ExtractStoredUser();
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto activity = interop->get_activity();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);

    jmethodID showProfileCardUIMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowProfileCardUI", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    auto wrapUserResult = User::WrapHandle(user);
    if (showProfileCardUIMethodId != NULL && Succeeded(wrapUserResult))
    {
        java_interop::get_java_interop_singleton()->StoreUser(wrapUserResult.ExtractPayload());
        title_callable_ui_internal::s_isTcuiRunning = true;

        uint64_t xuid;
        XalUserGetId(user, &xuid);
        xsapi_internal_stringstream xuidSS;
        xuidSS << xuid;

        // The only privileges that are needed for TCUI are 254 (Communications) and 255 (AddFriends) so just query those here
        xsapi_internal_stringstream privilegesSS;
        bool hasPrivilege;
        XalUserCheckPrivilege(user, XalPrivilege::XalPrivilege_Comms, &hasPrivilege, nullptr);
        if (hasPrivilege)
        {
            privilegesSS << XalPrivilege::XalPrivilege_Comms;
        }
        XalUserCheckPrivilege(user, XalPrivilege::XalPrivilege_AddFriends, &hasPrivilege, nullptr);
        if (hasPrivilege)
        {
            privilegesSS << " " << XalPrivilege::XalPrivilege_AddFriends;
        }

        auto currentUserIdString = jniEnv->NewStringUTF(xuidSS.str().data());
        auto targetUserIdString = jniEnv->NewStringUTF(targetXboxUserId.c_str());
        auto currentUserPrivileges = jniEnv->NewStringUTF(privilegesSS.str().data());
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showProfileCardUIMethodId, activity, currentUserIdString, targetUserIdString, currentUserPrivileges, user);
    }
    else
    {
        pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(
            xbox_live_error_code::logic_error,
            _T("ShowProfileCardUI method not found"))
            );
    }

    JNI_ERROR_CHECK(jniEnv)

    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_profile_ui(
    _In_ const string_t& targetXboxUserId
)
{
    auto interopResult = tcui_init();
    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
                )
            );
    }

    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
    .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto activity = interop->get_activity();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);
    jmethodID showUserProfileMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowUserProfile", "(Landroid/content/Context;Ljava/lang/String;J)V");
    if (showUserProfileMethodId != NULL)
    {
        auto targetXuid = jniEnv->NewStringUTF(targetXboxUserId.c_str());
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showUserProfileMethodId, activity, targetXuid);
    }

    JNI_ERROR_CHECK(jniEnv);

    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_hub_ui()
{
    auto interopResult = tcui_init();
    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
                )
            );
    }

    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
    .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto activity = interop->get_activity();
    auto titleId = AppConfig::Instance()->TitleId();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);
    jmethodID showTitleHubMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowTitleHub", "(Landroid/content/Context;Ljava/lang/String;)V");
    if (showTitleHubMethodId != NULL)
    {
        stringstream_t titleIdStr;
        titleIdStr << titleId;
        auto jTitleId = jniEnv->NewStringUTF(titleIdStr.str().c_str());
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showTitleHubMethodId, activity, jTitleId);
    }

    JNI_ERROR_CHECK(jniEnv);

    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_achievements_ui(
    _In_ uint32_t titleId
    )
{
    auto interopResult = tcui_init();
    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
            )
        );
    }

    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
    .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto activity = interop->get_activity();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);
    jmethodID showTitleAchievementsMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowTitleAchievements", "(Landroid/content/Context;Ljava/lang/String;)V");
    if (showTitleAchievementsMethodId != NULL)
    {
        stringstream_t titleIdStr;
        titleIdStr << titleId;
        auto jTitleId = jniEnv->NewStringUTF(titleIdStr.str().c_str());
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showTitleAchievementsMethodId, activity, jTitleId);
    }

    JNI_ERROR_CHECK(jniEnv);

    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_settings_ui()
{
    auto interopResult = tcui_init();
    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
                )
            );
    }

    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
    .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto context = interop->get_context_object();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);
    jmethodID showUserSettingsMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowUserSettings", "(Landroid/content/Context;)V");
    if (showUserSettingsMethodId != NULL)
    {
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showUserSettingsMethodId, context);
    }

    JNI_ERROR_CHECK(jniEnv);

    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_add_friends_ui()
{
    auto interopResult = tcui_init();
    if (interopResult.err())
    {
        return pplx::task_from_result<xbox::services::xbox_live_result<void>>(
            xbox_live_result<void>(
                interopResult.err(),
                interopResult.err_message()
                )
            );
    }

    auto task = pplx::create_task(title_callable_ui_internal::s_tcuiEventCompleted)
        .then([](int32_t errorCode)
    {
        title_callable_ui_internal::s_isTcuiRunning = false;
        return xbox_live_result<void>(xbox_live_result<void>());
    });

    auto interop = interopResult.payload();
    auto jvm = interop->get_java_vm();
    JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, "java interop not initialized properly")

    auto interopTcuiClass = interop->get_tcui_interop_class();
    auto context = interop->get_context_object();

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(jvm, jniEnv);
    jmethodID showAddFriendsMethodId = jniEnv->GetStaticMethodID(interopTcuiClass, "ShowAddFriends", "(Landroid/content/Context;)V");
    if (showAddFriendsMethodId != NULL)
    {
        jniEnv->CallStaticVoidMethod(interopTcuiClass, showAddFriendsMethodId, context);
    }

    JNI_ERROR_CHECK(jniEnv);

    return task;
}

pplx::task<xbox::services::xbox_live_result<std::vector<string_t>>>
title_callable_ui::show_player_picker_ui(
    _In_ const string_t& promptDisplayText,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<string_t>& preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<std::vector<string_t>>>(xbox::services::xbox_live_result<std::vector<string_t>>(xbox_live_error_code::unsupported));
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_game_invite_ui(
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference,
    _In_ const string_t& invitationDisplayText,
    _In_ const string_t& contextStringId
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_change_friend_relationship_ui(
    _In_ const string_t& targetXboxUserId
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END