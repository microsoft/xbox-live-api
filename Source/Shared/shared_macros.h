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
#define RETURN_C_INVALIDARGUMENT_IF(x) { if ( x ) { return XBL_RESULT_INVALID_ARG; } }
#define RETURN_C_INVALIDARGUMENT_IF_NULL(x) { if ( ( x ) == nullptr ) { return XBL_RESULT_INVALID_ARG; } }
#else
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
#define RETURN_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message); } }
#define RETURN_C_INVALIDARGUMENT_IF(x) { assert(!(x)); if ( x ) { return XBL_RESULT_INVALID_ARG; } }
#define RETURN_C_INVALIDARGUMENT_IF_NULL(x) { assert(!(( x ) == nullptr)); if ( ( x ) == nullptr ) { return XBL_RESULT_INVALID_ARG; } }
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

#define AUTH_HEADER (_T("Authorization"))
#define SIG_HEADER (_T("Signature"))
#define ETAG_HEADER (_T("ETag"))
#define DATE_HEADER (_T("Date"))
#define DEFAULT_USER_AGENT _T("XboxServicesAPI/") XBOX_SERVICES_API_VERSION_STRING

#define RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(func, type) \
{ \
    try { return func; } \
    catch (const std::exception& e) \
    { \
        xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code(); \
        return xbox_live_result<type>(err, e.what()); \
    } \
}

#define CATCH_RETURN() \
    catch (std::bad_alloc const& e) { return utils::std_bad_alloc_to_xbl_result(e); } \
    catch (std::exception const& e) { return utils::std_exception_to_xbl_result(e); } \
    catch (...) { return utils::unknown_exception_to_xbl_result(); }

#define CATCH_RETURN_WITH(errCode) \
    catch (std::bad_alloc const& e) { utils::std_bad_alloc_to_xbl_result(e); return errCode; } \
    catch (std::exception const& e) { utils::std_exception_to_xbl_result(e); return errCode; } \
    catch (...) { utils::unknown_exception_to_xbl_result(); return errCode; }

#define DEFINE_GET_STRING(className, methodName) \
    string_t className::methodName() const \
    { \
        return utils::external_string_from_internal_string(m_internalObj->methodName()); \
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