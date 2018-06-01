#define NOMINMAX

#include <xdk.h>
#define CALLBACK    __stdcall
#include "httpClient/httpClient.h"
#include "httpClient/httpProvider.h"
#include "httpClient/asyncProvider.h"

#include "xsapi-c/services_c.h"

int test(int a, int b)
{
    return a + b;
}