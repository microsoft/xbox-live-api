// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "StringService_WinRT.h"
#include "VerifyStringResult_WinRT.h"
#include "Utils_WinRT.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;
using namespace xbox::services::system;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

IAsyncOperation<VerifyStringResult^>^
StringService::VerifyStringAsync(
    _In_ Platform::String^ stringToVerify
    )
{
    auto task = m_cppObj.verify_string(
        STRING_T_FROM_PLATFORM_STRING(stringToVerify)
        )
    .then([](xbox_live_result<verify_string_result> result)
    {
        THROW_IF_ERR(result);
        return ref new VerifyStringResult(result.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<VerifyStringResult^>^>^ 
StringService::VerifyStringsAsync(
    _In_ IVectorView<Platform::String^>^ stringsToVerify
)
{
    auto task = m_cppObj.verify_strings(
        UtilsWinRT::CovertVectorViewToStdVectorString(stringsToVerify)
        )
    .then([](xbox_live_result<std::vector<verify_string_result>> results)
    {
        THROW_IF_ERR(results);
        auto responseVector = ref new Vector<VerifyStringResult^>();
        for (const auto& result : results.payload())
        {
            responseVector->Append(ref new VerifyStringResult(result));
        }
        return responseVector->GetView();
    });

    return ASYNC_FROM_TASK(task);
}


StringService::StringService(
    xbox::services::system::string_service cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
