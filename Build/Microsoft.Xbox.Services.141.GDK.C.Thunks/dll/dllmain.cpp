#include "pch.h"
#include <Xal/xal.h>
#include <xsapi-c/services_c.h>
#include <XGameRuntimeInit.h>

extern "C"
{
BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

XBL_DLLEXPORT HRESULT XBL_CALLING_CONV XblWrapper_XblInitialize(
    _In_z_ const char* scid,
    _In_ XTaskQueueHandle internalWorkQueue
) noexcept
{
    XGameRuntimeInitialize();

    // xal must be initialized to be used by xsapi
    XalInitArgs xalInitArgs = { };
    XalInitialize(&xalInitArgs, internalWorkQueue);

    XblInitArgs xblInitArgs = { };
    xblInitArgs.scid = scid;
    xblInitArgs.queue = internalWorkQueue;

    return XblInitialize(&xblInitArgs);
}
}