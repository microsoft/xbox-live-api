#define  _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#if HC_PLATFORM != HC_PLATFORM_ANDROID
#pragma warning (push)
#pragma warning(disable : 4334)
#pragma warning(disable : 4365)
#pragma warning(disable : 4242)
#pragma warning(disable : 4061)
#pragma warning(disable : 4191)
#pragma warning(disable : 4310)
#endif

#include "lapi.c"
#include "lauxlib.c"
#include "lbaselib.c"
#if HC_PLATFORM != HC_PLATFORM_ANDROID
#include "lbitlib.c"
#endif
#include "lcode.c"
#include "lcorolib.c"
#include "lctype.c"
#include "ldblib.c"
#include "ldebug.c"
#include "ldo.c"
#include "ldump.c"
#include "lfunc.c"
#include "lgc.c"
#include "linit.c"
#include "liolib.c"
#include "llex.c"
#include "lmathlib.c"
#include "lmem.c"
#include "loadlib.c"
#include "lobject.c"
#include "lopcodes.c"
#include "loslib.c"
#include "lparser.c"
#include "lstate.c"
#include "lstring.c"
#include "lstrlib.c"
#include "ltable.c"
#include "ltablib.c"
#include "ltm.c"
#include "lundump.c"
#include "lutf8lib.c"
#include "lvm.c"
#include "lzio.c"

#if HC_PLATFORM != HC_PLATFORM_ANDROID
#pragma warning (pop)
#endif
