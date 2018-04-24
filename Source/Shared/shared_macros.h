// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "build_version.h"

#ifndef UNIT_TEST_SERVICES
#define XSAPI_ASSERT(x) assert(x);
#else
#define XSAPI_ASSERT(x) if(!(x)) throw std::invalid_argument("");
#endif

#if defined(UNIT_TEST_SERVICES)
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
#define RETURN_CPP_INVALIDARGUMENT_IF(x, type, message) { if ( x ) { return xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message); } }
#define RETURN_C_INVALIDARGUMENT_IF(x) { if ( x ) { return E_INVALIDARG; } }
#define RETURN_C_INVALIDARGUMENT_IF_NULL(x) { if ( ( x ) == nullptr ) { return E_INVALIDARG; } }
#else
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
#define RETURN_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message); } }
#define RETURN_C_INVALIDARGUMENT_IF(x) { assert(!(x)); if ( x ) { return E_INVALIDARG; } }
#define RETURN_C_INVALIDARGUMENT_IF_NULL(x) { assert(!(( x ) == nullptr)); if ( ( x ) == nullptr ) { return E_INVALIDARG; } }
#endif
#define THROW_CPP_INVALIDARGUMENT_IF(x) if ( x ) { throw std::invalid_argument(""); }
#define THROW_CPP_INVALIDARGUMENT_IF_NULL(x) if ( ( x ) == nullptr ) { throw std::invalid_argument(""); }

#if defined(UNIT_TEST_SERVICES)
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x, type, message) { if ( x.empty() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message));  } }
#else
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x, type, message) { assert(!x.empty()); if ( x.empty() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message));  } }
#endif

#define THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x) { auto y = x; if ( y.empty() ) { throw std::invalid_argument(""); } }
#define RETURN_TASK_CPP_IF_ERR(x, type) if ( x.err() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(x.err(), x.err_message())); }
#define RETURN_TASK_CPP_IF(x, type, message) { if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::logic_error, message)); } }
#define RETURN_CPP_IF_ERR(x, type) if ( x.err() ) { return xbox::services::xbox_live_result<type>(x.err(), x.err_message()); }
#define RETURN_CPP_IF(x, type, errc, message) { if ( x ) { return xbox::services::xbox_live_result<type>(errc, message); } }

#define ASSERT_CPP_INVALIDARGUMENT_IF_NULL(x) XSAPI_ASSERT(x != nullptr);
#define ASSERT_CPP_INVALIDARGUMENT_IF(x) XSAPI_ASSERT(x)
#define ASSERT_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x) XSAPI_ASSERT(!x.empty());

#define THROW_CPP_INVALID_JSON_STRING_IF_STRING_EMPTY(x) { auto y = x; if ( y.empty() ) { throw web::json::json_exception(); } }
#define THROW_CPP_RUNTIME_IF(x,y) if ( x ) { throw std::runtime_error(y); }

#define NO_COPY_AND_ASSIGN(T) \
    T(const T&); \
    T& operator=(const T&);

#define SECONDS_PER_DAY 86400

#define STRING_T_FROM_PLATFORM_STRING(x) \
    (x->IsEmpty() ? string_t() : string_t(x->Data()))

#define PLATFORM_STRING_FROM_STRING_T(x) \
    (x.empty() ? nullptr : ref new Platform::String(x.c_str()))

#define PLATFORM_STRING_FROM_INTERNAL_STRING(x) \
    (x.empty() ? nullptr : ref new Platform::String(xbox::services::utils::string_t_from_internal_string(x).c_str()))

#define INTERNAL_STRING_FROM_PLATFORM_STRING(x) \
    (x->IsEmpty() ? xsapi_internal_string() : xbox::services::utils::internal_string_from_utf16(x->Data()))

#define AUTH_HEADER ("Authorization")
#define SIG_HEADER ("Signature")
#define ETAG_HEADER ("ETag")
#define DATE_HEADER ("Date")
#define RETRY_AFTER_HEADER ("Retry-After")
#define DEFAULT_USER_AGENT "XboxServicesAPI/" XBOX_SERVICES_API_VERSION_STRING

#define RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(func, type) \
{ \
    try { return func; } \
    catch (const std::exception& e) \
    { \
        xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code(); \
        return xbox_live_result<type>(err, e.what()); \
    } \
}

#define CREATE_EXTERNAL_XBOX_LIVE_RESULT(type, internalReturnObj) \
    xbox_live_result<type>(type(internalReturnObj.payload()), internalReturnObj.err(), internalReturnObj.err_message())


#define CATCH_RETURN() \
    catch (...) { return utils::convert_exception_to_hresult(); }

#define CATCH_RETURN_WITH(errCode) \
    catch (...) \
    { \
        HRESULT hr = utils::convert_exception_to_hresult(); \
        xsapi_internal_stringstream ss; \
        ss << "Exception reached api boundry: HR" << hr; \
        LOG_ERROR(ss.str().data()); \
        return errCode; \
    }

#define DEFINE_GET_STRING(className, methodName) \
    string_t className::methodName() const \
    { \
        return utils::string_t_from_internal_string(m_internalObj->methodName()); \
    }

#define DEFINE_GET_STD_STRING(className, methodName) \
    std::string className::methodName() const \
    { \
        return std::string(m_internalObj->methodName().data()); \
    }

#define DEFINE_GET_BOOL(className, methodName) \
    bool className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_UINT32(className, methodName) \
    uint32_t className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_STRING_VECTOR(className, methodName) \
    std::vector<string_t> className::methodName() const \
    { \
        return utils::std_string_vector_from_internal_string_vector(m_internalObj->methodName()); \
    }

#define DEFINE_GET_ENUM_TYPE(className, enumType, methodName) \
    enumType className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_URI(className, methodName) \
    const web::uri& className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_VECTOR_INTERNAL_TYPE(className, externalType, methodName) \
    std::vector<externalType> className::methodName() const \
    { \
        return utils::std_vector_external_from_internal_vector<externalType, std::shared_ptr<externalType##_internal>>(m_internalObj->methodName()); \
    }

#define DEFINE_GET_VECTOR(className, typeName, methodName) \
    std::vector<typeName> className::methodName() const \
    { \
        return utils::std_vector_from_internal_vector<typeName>(m_internalObj->methodName()); \
    }

#define DEFINE_GET_OBJECT(className, objectType, methodName) \
    objectType className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_OBJECT_REF(className, objectType, methodName) \
    const objectType& className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

