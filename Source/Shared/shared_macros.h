// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "build_version.h"

#ifndef XSAPI_UNIT_TESTS
#define XSAPI_ASSERT(x) assert(x);
#else
#define XSAPI_ASSERT(x) if(!(x)) throw std::invalid_argument("");
#endif

#define VERIFY_XBL_INITIALIZED() { if (xbox::services::GlobalState::Get() == nullptr) return E_XBL_NOT_INITIALIZED; }

#if defined(XSAPI_UNIT_TESTS)
#if !XSAPI_NO_PPL
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
#endif // !XSAPI_NO_PPL
#define RETURN_CPP_INVALIDARGUMENT_IF(x, type, message) { if ( x ) { return xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message); } }
#define RETURN_HR_INVALIDARGUMENT_IF(x) { if ( x ) { return E_INVALIDARG; } }
#define RETURN_HR_INVALIDARGUMENT_IF_NULL(x) { if ( ( x ) == nullptr ) { return E_INVALIDARG; } }
#define RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(x) { if ( x[0] == 0) { return E_INVALIDARG; } }
#else
#if !XSAPI_NO_PPL
#define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
#endif // !XSAPI_NO_PPL
#define RETURN_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message); } }
#define RETURN_HR_INVALIDARGUMENT_IF(x) { assert(!(x)); if ( x ) { return E_INVALIDARG; } }
#define RETURN_HR_INVALIDARGUMENT_IF_NULL(x) { assert(!(( x ) == nullptr)); if ( ( x ) == nullptr ) { return E_INVALIDARG; } }
#define RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(x) { assert(!(x[0] == 0)); if ( x[0] == 0) { return E_INVALIDARG; } }
#endif
#define INIT_OUT_PTR_PARAM(x) { if ( x ) { *x = nullptr; } }
#define THROW_CPP_INVALIDARGUMENT_IF(x) if ( x ) { throw std::invalid_argument(""); }
#define THROW_CPP_INVALIDARGUMENT_IF_NULL(x) if ( ( x ) == nullptr ) { throw std::invalid_argument(""); }

#if !XSAPI_NO_PPL
    #if defined(XSAPI_UNIT_TESTS)
    #define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
    #define RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x, type, message) { if ( x.empty() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message));  } }
    #else
    #define RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x, type, message) { assert(!x.empty()); if ( x.empty() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message));  } }
    #define RETURN_TASK_CPP_INVALIDARGUMENT_IF(x, type, message) { assert(!(x)); if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::invalid_argument, message)); } }
    #endif

    #define RETURN_TASK_CPP_IF_ERR(x, type) if ( x.err() ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(x.err(), x.err_message())); }
    #define RETURN_TASK_CPP_IF(x, type, message) { if ( x ) { return pplx::task_from_result(xbox::services::xbox_live_result<type>(xbox_live_error_code::logic_error, message)); } }
#endif // !XSAPI_NO_PPL

#define THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x) { auto y = x; if ( y.empty() ) { throw std::invalid_argument(""); } }
#define RETURN_CPP_IF_ERR(x, type) if ( x.err() ) { return xbox::services::xbox_live_result<type>(x.err(), x.err_message()); }
#define RETURN_CPP_IF(x, type, errc, message) { if ( x ) { return xbox::services::xbox_live_result<type>(errc, message); } }
#define RETURN_HR_IF(x, hr) { if (x) { return hr; } }
#define RETURN_HR_IF_FAILED(expr) { HRESULT exprResult{ expr }; if (FAILED(exprResult)) { return exprResult; } }
#define RETURN_PENDING_OR_HR(expr) { HRESULT exprResult{ expr }; return SUCCEEDED(exprResult) ? E_PENDING : exprResult; }
#define RETURN_HR_IF_LOG_DEBUG(x, hr, message) { if (x) { LOG_DEBUG(message); return hr; } }

#define ASSERT_CPP_INVALIDARGUMENT_IF_NULL(x) XSAPI_ASSERT(x != nullptr);
#define ASSERT_CPP_INVALIDARGUMENT_IF(x) XSAPI_ASSERT(x)
#define ASSERT_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(x) XSAPI_ASSERT(!x.empty());

#define THROW_CPP_RUNTIME_IF(x,y) if ( x ) { throw std::runtime_error(y); }

#define COMPLETE_ASYNC_AND_RETURN(async, result, resultSize, returnValue) { XAsyncComplete(async, result, resultSize); return returnValue; }
#define COMPLETE_ASYNC_AND_RETURN_VOID(async, result, resultSize) { XAsyncComplete(async, result, resultSize); return; }

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

#define RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT_FROM_HR(func, type) \
{ \
    try \
    { \
        auto hr = func; \
        return xbox_live_result<type>(xbox_live_error_code(hr)); \
    } \
    catch (const std::exception& e) \
    { \
        xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code(); \
        return xbox_live_result<type>(err, e.what()); \
    } \
}

#define RETURN_EXCEPTION_FREE_HRESULT(func) \
{ \
    try { return func; } \
    catch (const std::exception& e) \
    { \
        e; \
        return utils::convert_exception_to_hresult(); \
    } \
}

#define CREATE_EXTERNAL_XBOX_LIVE_RESULT(type, internalReturnObj) \
    xbox_live_result<type>(type(internalReturnObj.payload()), internalReturnObj.err(), internalReturnObj.err_message())


#define CATCH_RETURN() \
    catch (...) { return xbox::services::utils::convert_exception_to_hresult(); }

#define CATCH_RETURN_WITH(errCode) \
    catch (...) \
    { \
        HRESULT hr = xbox::services::utils::convert_exception_to_hresult(); \
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

#define DEFINE_GET_ENUM_TYPE(className, enumType, methodName) \
    enumType className::methodName() const \
    { \
        return m_internalObj->methodName(); \
    }

#define DEFINE_GET_URI(className, methodName) \
    const xbox::services::uri& className::methodName() const \
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


// Disable Warning macros

// if msvc
#if defined (_MSC_VER)

#define DISABLE_WARNING_PUSH                    __pragma(warning(push))
#define DISABLE_WARNING_POP                     __pragma(warning(pop))
#define DISABLE_WARNING(warningCode)            __pragma(warning(disable:warningCode))  // expects numeric code for msvc
#define SUPPRESS_ANALYSIS_WARNING(warningCode)  __pragma(warning(suppress:warningCode))

#define SUPPRESS_WARNING_NULL_PTR_DEREFERENCE   SUPPRESS_ANALYSIS_WARNING(6011)
#define SUPPRESS_WARNING_UNINITIALIZED_MEMORY   SUPPRESS_ANALYSIS_WARNING(6001)
#define SUPPRESS_WARNING_EXPRESSION_NOT_TRUE    SUPPRESS_ANALYSIS_WARNING(28020)
#define SUPPRESS_WARNING_UNINITIALIZED_MEMBER   SUPPRESS_ANALYSIS_WARNING(26495)
#define SUPPRESS_WARNING_UNNAMED_CUSTOM_OBJ     SUPPRESS_ANALYSIS_WARNING(26444)


#elif defined(__GNUC__)

#define DO_PRAGMA(X)                            _Pragma(#X)
#define DISABLE_WARNING_PUSH                    DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP                     DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningCode)            DO_PRAGMA(GCC diagnostic ignored #warningCode)  // expects arg name for clang and gnu compilers
#define SUPPRESS_ANALYSIS_WARNING(warningCode)  // gnu doesn't support per-instance static analyzer warning suppression

#define SUPPRESS_WARNING_NULL_PTR_DEREFERENCE
#define SUPPRESS_WARNING_UNINITIALIZED_MEMORY
#define SUPPRESS_WARNING_EXPRESSION_NOT_TRUE 
#define SUPPRESS_WARNING_UNINITIALIZED_MEMBER
#define SUPPRESS_WARNING_UNNAMED_CUSTOM_OBJ  


#elif defined(__clang__)

#define DO_PRAGMA(X)                            _Pragma(#X)
#define DISABLE_WARNING_PUSH                    DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP                     DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningCode)            DO_PRAGMA(GCC diagnostic ignored #warningCode)  // expects arg name for clang and gnu compilers
#define SUPPRESS_ANALYSIS_WARNING(warningCode)  // clang doesn't support per-instance static analyzer warning suppression

#define SUPPRESS_WARNING_NULL_PTR_DEREFERENCE
#define SUPPRESS_WARNING_UNINITIALIZED_MEMORY
#define SUPPRESS_WARNING_EXPRESSION_NOT_TRUE 
#define SUPPRESS_WARNING_UNINITIALIZED_MEMBER
#define SUPPRESS_WARNING_UNNAMED_CUSTOM_OBJ  


// default for non-defined platforms
#else

#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING(warningCode)

#define SUPPRESS_WARNING_NULL_PTR_DEREFERENCE
#define SUPPRESS_WARNING_UNINITIALIZED_MEMORY
#define SUPPRESS_WARNING_EXPRESSION_NOT_TRUE 
#define SUPPRESS_WARNING_UNINITIALIZED_MEMBER
#define SUPPRESS_WARNING_UNNAMED_CUSTOM_OBJ  


#endif