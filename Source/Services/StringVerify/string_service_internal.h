#pragma once
#include "xsapi-c/string_verify_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class VerifyStringResult
{
public:
    VerifyStringResult();

    VerifyStringResult(
        XblVerifyStringResultCode resultCode,
        xsapi_internal_string firstOffendingSubstring
    );

    const XblVerifyStringResultCode ResultCode() const;

    const xsapi_internal_string& FirstOffendingSubstring() const;

    static Result<xsapi_internal_vector<VerifyStringResult>> DeserializeVerifyStringsResult(
        _In_ const JsonValue& json
    );

    size_t SizeOf() const;

private:
    XblVerifyStringResultCode m_resultCode = XblVerifyStringResultCode::Success;
    xsapi_internal_string m_firstOffendingSubstring;
    
    static Result<VerifyStringResult> DeserializeVerifyStringResult(
        _In_ const JsonValue& json
    );

};

class StringService : public std::enable_shared_from_this<StringService>
{

public:
    StringService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
    );

    HRESULT VerifyStrings(
        _In_ const xsapi_internal_vector<xsapi_internal_string> stringsToVerify,
        _In_ AsyncContext<Result<xsapi_internal_vector<VerifyStringResult>>> async
    );

private:
    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_contextSettings;
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
