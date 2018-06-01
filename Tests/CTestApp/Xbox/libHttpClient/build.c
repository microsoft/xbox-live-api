#define NOMINMAX

#include <xdk.h>
#define CALLBACK    __stdcall

#include "httpClient/httpClient.h"
#include "httpClient/httpProvider.h"
#include "httpClient/asyncProvider.h"

int test(int a, int b)
{
    HCMemSetFunctions(0, 0);
    return a + b;
}