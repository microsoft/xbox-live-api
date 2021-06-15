cmake_minimum_required(VERSION 3.6)


function(GET_ANDROID_XSAPI_SOURCE_FILES
         OUT_COMMON_SOURCE_FILES
         OUT_SYSTEM_SOURCE_FILES
         OUT_SHARED_SOURCE_FILES
         OUT_TCUI_SOURCE_FILES
         OUT_NOTIFICATION_SOURCE_FILES
         OUT_EVENTS_SOURCE_FILES
         PATH_TO_ROOT
         )

    set(${OUT_COMMON_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/Services/Common/Unix/pch.cpp"
        "${PATH_TO_ROOT}/Source/Services/Common/Unix/pch.h"
        PARENT_SCOPE
        )

    set(${OUT_SYSTEM_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/System/a/java_interop.cpp"
        "${PATH_TO_ROOT}/Source/System/a/java_interop.h"
        "${PATH_TO_ROOT}/Source/System/Android/local_storage_android.cpp"
        "${PATH_TO_ROOT}/Source/System/platform_api.cpp"
        PARENT_SCOPE
        )

    set(${OUT_SHARED_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/Shared/a/android_utils.h"
        "${PATH_TO_ROOT}/Source/Shared/a/android_utils.cpp"
        "${PATH_TO_ROOT}/Source/Shared/u/xbl_guid.h"
        "${PATH_TO_ROOT}/Source/Shared/a/guid.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/http_call_jni.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/http_call_jni.h"
        "${PATH_TO_ROOT}/Source/Shared/a/http_call_static_glue.h"
        "${PATH_TO_ROOT}/Source/Shared/a/http_call_static_glue.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/interop_jni.h"
        "${PATH_TO_ROOT}/Source/Shared/a/interop_jni.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/jni_utils.h"
        "${PATH_TO_ROOT}/Source/Shared/a/rwlock_guard.h"
        "${PATH_TO_ROOT}/Source/Shared/a/rwlock_guard.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/utils_a.h"
        "${PATH_TO_ROOT}/Source/Shared/a/utils_a.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/xbox_live_app_config_jni.cpp"
        "${PATH_TO_ROOT}/Source/Shared/a/xbox_live_app_config_static_glue.h"
        "${PATH_TO_ROOT}/Source/Shared/http_call_legacy.h"
        "${PATH_TO_ROOT}/Source/Shared/http_call_legacy.cpp"
        PARENT_SCOPE
        )

    set(${OUT_TCUI_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/Services/TCUI/Android/title_callable_static_glue.h"
        "${PATH_TO_ROOT}/Source/Services/TCUI/Android/title_callable_ui_static_glue.cpp"
        "${PATH_TO_ROOT}/Source/Services/TCUI/Android/title_callable_ui_android.cpp"
        PARENT_SCOPE
        )

    set(${OUT_NOTIFICATION_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/Services/Notification/Mobile/notification_service_mobile.cpp"
        "${PATH_TO_ROOT}/Source/Services/Notification/notification_internal.h"
        "${PATH_TO_ROOT}/Source/Services/Notification/notification_service.cpp"
        "${PATH_TO_ROOT}/Source/Services/Notification/notification_api.cpp"
        PARENT_SCOPE
        )

    set(${OUT_EVENTS_SOURCE_FILES}
        "${PATH_TO_ROOT}/Source/Services/Events/event.cpp"
        "${PATH_TO_ROOT}/Source/Services/Events/event_queue.cpp"
        "${PATH_TO_ROOT}/Source/Services/Events/events_service.h"
        "${PATH_TO_ROOT}/Source/Services/Events/events_service_api.cpp"
        "${PATH_TO_ROOT}/Source/Services/Events/event_upload_payload.cpp"
        "${PATH_TO_ROOT}/Source/Services/Events/events_service_xsapi.h"
        "${PATH_TO_ROOT}/Source/Services/Events/events_service_xsapi.cpp"
        "${PATH_TO_ROOT}/Source/Services/Events/Android/events_service_android.cpp"
        PARENT_SCOPE
        )


endfunction()