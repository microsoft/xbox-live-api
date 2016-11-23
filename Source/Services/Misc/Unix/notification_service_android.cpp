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
#include "xsapi/system.h"
#include "Misc/notification_service.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "user_impl.h"
#include "a/java_interop.h"
#include "a/jni_utils.h"

using namespace pplx;
using namespace xbox::services::system;

JNINativeMethod notification_methods[] =
{
    {
        "notificiation_registration_callback",
        "(Ljava/lang/String;Z)V",
        (void *)&xbox::services::notification::notification_service_android::notificiation_registration_callback
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

notification_service_android::notification_service_android() :
    m_isInitializing(false)
{
}

pplx::task_completion_event<string_t> notification_service_android::s_notificationRegisterEvent;

void notification_service_android::notificiation_registration_callback(JNIEnv* jniEnv, jclass, jstring notificationData, jboolean isCached)
{
    auto notiServiceAndroid = std::dynamic_pointer_cast<notification_service_android>(notification_service::get_notification_service_singleton());
    if (notificationData != NULL || !isCached)
    {
        auto notificationDataString = jniEnv->GetStringUTFChars(notificationData, 0);
        if (!notiServiceAndroid->m_isInitializing)
        {
            notiServiceAndroid->subscribe_to_notifications_finish(
                notificationDataString,
                notiServiceAndroid
                );
        }
        else
        {
            LOG_WARN("notification registration for non refresh");
            s_notificationRegisterEvent.set(notificationDataString);
        }
    }
    else
    {
        s_notificationRegisterEvent.set(_T(""));
    }
    
}

pplx::task<xbox_live_result<void>> notification_service_android::subscribe_to_notifications(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    )
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    interop->register_natives(notification_methods);

    return notification_service::subscribe_to_notifications(
        userContext,
        xboxLiveContextSettings,
        appConfig
        );
}

pplx::task<xbox_live_result<void>>
notification_service_android::subscribe_to_notifications()
{
    std::weak_ptr<notification_service_android> thisWeak = std::dynamic_pointer_cast<notification_service_android>(shared_from_this());
    return create_task([thisWeak]()
    {
        std::shared_ptr<notification_service_android> pThis(thisWeak.lock());
        if (pThis != nullptr)
        {
            auto javaInteropSingleton = java_interop::get_java_interop_singleton();
            auto javaVM = javaInteropSingleton->get_java_vm();
            if (javaVM == nullptr)
            {
                LOG_ERROR("java interop not initialized properly");
                return xbox_live_result<void>(xbox_live_error_code::runtime_error, "java interop not initialized properly");
            }
            JNIEnv* jniEnv;
            JNI_ATTACH_THREAD(javaVM, jniEnv);

            auto marketActivityClass = javaInteropSingleton->get_market_activity_class();
            jmethodID registerWithGNS = jniEnv->GetStaticMethodID(marketActivityClass, "RegisterWithGNS", "(Landroid/content/Context;)V");
            if (registerWithGNS != NULL)
            {
                s_notificationRegisterEvent = pplx::task_completion_event<string_t>();
                pThis->m_isInitializing = true;
                jniEnv->CallStaticVoidMethod(marketActivityClass, registerWithGNS, javaInteropSingleton->get_context_object());
                if (jniEnv->ExceptionCheck())
                {
                    LOG_ERROR("error intialization gns");
                    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "gns init error");
                }
                
                return create_task(s_notificationRegisterEvent)
                .then([pThis](string_t registrationToken)
                {
                    return pThis->subscribe_to_notifications_finish(registrationToken, pThis);
                }).get();
            }
            else
            {
                LOG_ERROR("registerWithGNS method not found");
            }
        }

        return xbox_live_result<void>(xbox_live_error_code::runtime_error);
    });
}

pplx::task<xbox_live_result<void>>
notification_service_android::subscribe_to_notifications_finish(
    _In_ const string_t& registrationToken,
    _In_ const std::shared_ptr<notification_service_android>& pThis
    )
{
    pThis->m_isInitializing = false;
    if (registrationToken.empty())
    {
        LOG_ERROR("gns registration failed or was already cached");
        return pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xbox_live_error_code::runtime_error, "gns registration failed or was already cached"));
    }

    auto endpointIdCache = xbox_system_factory::get_factory()->create_local_config()->get_value_from_local_storage(pThis->ENDPOINT_ID_CACHE_NAME);
    if (!endpointIdCache.empty())
    {
        pThis->m_endpointId = endpointIdCache;
        pThis->unsubscribe_from_notifications(endpointIdCache).get();
    }

    std::vector<notification_filter> notificationFilterList = { { notification_filter_source_type::multiplayer, 1 } };
    return pThis->subscribe_to_notifications_helper(
        utils::create_guid(true),
        registrationToken,
        _T("Android"),
        _T("AndroidDevice"),
        _T("XSAPI_A"),
        notificationFilterList
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END