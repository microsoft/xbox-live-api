#pragma once
namespace xbox { namespace services { namespace system { 
class title_callable_ui_internal
{
public:
    static void tcui_completed_callback(JNIEnv *, jclass, int errorCode);
    static pplx::task_completion_event<int32_t> s_tcuiEventCompleted;
    static bool s_isTcuiRunning;
};
} } }

