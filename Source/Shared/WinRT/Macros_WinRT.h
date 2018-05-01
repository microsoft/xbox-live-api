// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

// preprocessor workaround
// to work with L##__FUNCTION__ and L##__FILE__ statements.
#define TEXTW(quote) _TEXTW(quote)
#define _TEXTW(quote) L##quote

#define LOG_INFO_MSG(msg) {}
#define LOG_ERROR_MSG(msg) {} 
#define LOG_EXCEPTION(hr) { }

#define THROW_INVALIDARGUMENT_IF_NULL(x) if ( ( x ) == nullptr ) { LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_INVALIDARGUMENT_IF(x) if ( x ) { LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_IF_HR_FAILED(hr) { HRESULT hr2 = hr; if ( FAILED( hr2 ) ) { LOG_EXCEPTION(hr2); throw ref new Platform::COMException(hr2); } }
#define THROW_HR_IF(x,hr) if ( x ) { LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_HR_IF_ERR(x) if ( x ) { LOG_EXCEPTION(x.value()); throw ref new Platform::COMException(x.value()); }
#define THROW_HR_IF_WITH_LOG(x,hr,msg) if ( x ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_IF_ERR(x)                                                                                                         \
    if( x.err() )                                                                                                               \
    {                                                                                                                           \
        HRESULT hr = xbox::services::utils::convert_xbox_live_error_code_to_hresult(x.err());                                   \
        Platform::String^ message = ref new Platform::String(utility::conversions::to_utf16string(x.err_message()).c_str());    \
        throw Microsoft::Xbox::Services::System::UtilsWinRT::ConvertHRESULTToException(hr, message);                            \
    }

#define THROW_ON_ERR_CODE(x)                                                                                                    \
    if( x )                                                                                                                     \
    {                                                                                                                           \
        HRESULT hr = utils::convert_xbox_live_error_code_to_hresult(x);                                                         \
        throw Microsoft::Xbox::Services::System::UtilsWinRT::ConvertHRESULTToException(hr, ref new Platform::String());         \
    }

#define DEFINE_PROP_GET_URI(x, cppX) \
    property Windows::Foundation::Uri^ x \
    { \
        Windows::Foundation::Uri^ get() \
        {\
            if (m_cppObj->##cppX().is_empty())\
                  return nullptr;\
            else\
                  return ref new Windows::Foundation::Uri(ref new Platform::String(m_cppObj->##cppX().to_string().c_str()));\
        }\
    }

#define DEFINE_PTR_PROP_GETSET(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj->##cppX();\
        }\
        void set(_In_ y value)\
        {\
            m_cppObj->set_##cppX(value);\
        }\
    }

#define DEFINE_PTR_PROP_GET(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj->##cppX();\
        }\
    }

#define DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(x, cppX) DEFINE_PROP_GETSET_TIMESPAN(x, cppX, std::chrono::seconds)
#define DEFINE_PROP_GETSET_TIMESPAN_IN_MS(x, cppX) DEFINE_PROP_GETSET_TIMESPAN(x, cppX, std::chrono::milliseconds)

#define DEFINE_PROP_GETSET_TIMESPAN(x, cppX, secondT) \
    property Windows::Foundation::TimeSpan x \
    { \
        Windows::Foundation::TimeSpan get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan( m_cppObj->##cppX());\
        }\
        void set(_In_ Windows::Foundation::TimeSpan value)\
        {\
            m_cppObj->set_##cppX(Microsoft::Xbox::Services::System::UtilsWinRT::ConvertTimeSpanToSeconds<secondT>(value));\
        }\
    }

#define DEFINE_PROP_GET_URI_OBJ(x, cppX) \
    property Windows::Foundation::Uri^ x \
    { \
        Windows::Foundation::Uri^ get() \
        {\
            if (m_cppObj.##cppX().is_empty())\
                  return nullptr;\
            else\
                  return ref new Windows::Foundation::Uri(ref new Platform::String(m_cppObj.##cppX().to_string().c_str()));\
        }\
    }

#define DEFINE_PROP_GET_STR_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj.##cppX().c_str());\
        }\
    }

#define DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj.##cppX());\
        }\
    }

#define DEFINE_PTR_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj->##cppX());\
        }\
    }


#define DEFINE_PTR_PROP_GET_STR_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj->##cppX().c_str());\
        }\
    }

#define DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(xbox::services::utils::string_t_from_internal_string(m_cppObj->##cppX()).c_str());\
        }\
    }

#define DEFINE_PROP_GETSET_STR_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj.##cppX().c_str());\
        }\
        void set(_In_ Platform::String^ value)\
        {\
            m_cppObj.set_##cppX(value->Data());\
        }\
    }

#define DEFINE_PTR_PROP_GETSET_STR_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        {\
            return ref new Platform::String(m_cppObj->##cppX().c_str());\
        }\
        void set(_In_ Platform::String^ value)\
        {\
            m_cppObj->set_##cppX(value->Data());\
        }\
    }

#define DEFINE_PROP_GETSET_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj.##cppX();\
        }\
        void set(_In_ y value)\
        {\
            m_cppObj.set_##cppX(value);\
        }\
    }

#define DEFINE_PTR_PROP_GETSET_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj->##cppX();\
        }\
        void set(_In_ y value)\
        {\
            m_cppObj->set_##cppX(value);\
        }\
    }

#define DEFINE_PROP_GET_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj.##cppX();\
        }\
    }

#define DEFINE_PTR_PROP_GET_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        {\
            return m_cppObj->##cppX();\
        }\
    }

#define DEFINE_PROP_GET_DATETIME_OBJ(x, cppX) \
    property Windows::Foundation::DateTime x \
    { \
        Windows::Foundation::DateTime get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertNativeDateTimeToDateTime(m_cppObj.##cppX());\
        }\
    }

#define DEFINE_PROP_GETSET_DATETIME_OBJ(x, cppX) \
    property Windows::Foundation::DateTime x \
    { \
        Windows::Foundation::DateTime get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertNativeDateTimeToDateTime(m_cppObj.##cppX());\
        }\
    }\
    void set(_In_ Windows::Foundation::DateTime value)\
    {\
        m_cppObj.set_##cppX(Microsoft::Xbox::Services::System::UtilsWinRT::ConvertDateTimeToNativeDateTime(value));\
    }\

#define DEFINE_PTR_PROP_GET_DATETIME_OBJ(x, cppX) \
    property Windows::Foundation::DateTime x \
    { \
        Windows::Foundation::DateTime get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertNativeDateTimeToDateTime(m_cppObj->##cppX());\
        }\
    }

#define DEFINE_PROP_GET_TIMESPAN_OBJ(x, cppX) \
    property Windows::Foundation::TimeSpan x \
    { \
        Windows::Foundation::TimeSpan get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj.##cppX());\
        }\
    }

#define DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(x, cppX) \
    property Windows::Foundation::TimeSpan x \
    { \
        Windows::Foundation::TimeSpan get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj->##cppX());\
        }\
    }


#define DEFINE_PROP_GETSET_TIMESPAN_IN_SEC_OBJ(x, cppX) DEFINE_PROP_GETSET_TIMESPAN_OBJ(x, cppX, std::chrono::seconds)
#define DEFINE_PTR_PROP_GETSET_TIMESPAN_IN_SEC_OBJ(x, cppX) DEFINE_PTR_PROP_GETSET_TIMESPAN_OBJ(x, cppX, std::chrono::seconds)
#define DEFINE_PROP_GETSET_TIMESPAN_IN_MS_OBJ(x, cppX) DEFINE_PROP_GETSET_TIMESPAN_OBJ(x, cppX, std::chrono::milliseconds)
#define DEFINE_PTR_PROP_GETSET_TIMESPAN_IN_MS_OBJ(x, cppX) DEFINE_PTR_PROP_GETSET_TIMESPAN_OBJ(x, cppX, std::chrono::milliseconds)

#define DEFINE_PROP_GETSET_TIMESPAN_OBJ(x, cppX, secondT) \
    property Windows::Foundation::TimeSpan x \
    { \
        Windows::Foundation::TimeSpan get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj.##cppX());\
        }\
        void set(_In_ Windows::Foundation::TimeSpan value)\
        {\
            m_cppObj.set_##cppX(Microsoft::Xbox::Services::System::UtilsWinRT::ConvertTimeSpanToSeconds<secondT>(value));\
        }\
    }

#define DEFINE_PTR_PROP_GETSET_TIMESPAN_OBJ(x, cppX, secondT) \
    property Windows::Foundation::TimeSpan x \
    { \
        Windows::Foundation::TimeSpan get() \
        {\
            return Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj->##cppX());\
        }\
        void set(_In_ Windows::Foundation::TimeSpan value)\
        {\
            m_cppObj->set_##cppX(Microsoft::Xbox::Services::System::UtilsWinRT::ConvertTimeSpanToSeconds<secondT>(value));\
        }\
    }

#define DEFINE_PROP_GET_ENUM_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        { \
            return (y)(m_cppObj.##cppX());\
        } \
    }

#define DEFINE_PTR_PROP_GET_ENUM_OBJ(x, cppX, y) \
    property y x \
    { \
        y get() \
        { \
            return (y)(m_cppObj->##cppX());\
        } \
    }

#define DEFINE_PROP_GETSET_ENUM_OBJ(x, cppX, y, cppY) \
    property y x \
    { \
        y get() \
        { \
            return (y)(m_cppObj.##cppX());\
        } \
        void set(_In_ y value)\
        {\
            m_cppObj.set_##cppX(((cppY)(value))); \
        }\
    }

#define DEFINE_PTR_PROP_GETSET_ENUM_OBJ(x, cppX, y, cppY) \
    property y x \
    { \
        y get() \
        { \
            return (y)(m_cppObj->##cppX());\
        } \
        void set(_In_ y value)\
        {\
            m_cppObj->set_##cppX(((cppY)(value))); \
        }\
    }

#define DEFINE_PROP_GET_STR_FROM_JSON_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        { \
            return ref new Platform::String(m_cppObj.##cppX().serialize().c_str()); \
        } \
    }

#define DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        { \
            return ref new Platform::String(m_cppObj->##cppX().serialize().c_str()); \
        } \
    }

#define DEFINE_PROP_GETSET_STR_FROM_JSON_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        { \
            return ref new Platform::String(m_cppObj.##cppX().serialize().c_str()); \
        } \
        void set(_In_ Platform::String^ value)\
        {\
            m_cppObj.set_##cppX(string_t(value->Data()));\
        }\
    }

#define DEFINE_PTR_PROP_GETSET_STR_FROM_JSON_OBJ(x, cppX) \
    property Platform::String^ x \
    { \
        Platform::String^ get() \
        { \
            return ref new Platform::String(m_cppObj->##cppX().serialize().c_str()); \
        } \
        void set(_In_ Platform::String^ value)\
        {\
            m_cppObj->set_##cppX(web::json::value::parse(value->Data()));\
        }\
    }

#define CONVERT_STD_EXCEPTION(operation) \
{                                               \
    try                                         \
    {                                           \
        operation;                              \
    }                                           \
    catch(const std::exception& ex)             \
    {                                           \
        HRESULT hr = xbox::services::utils::convert_exception_to_hresult(); \
        Platform::String^ message = ref new Platform::String(utility::conversions::to_utf16string(ex.what()).c_str()); \
        throw Microsoft::Xbox::Services::System::UtilsWinRT::ConvertHRESULTToException(hr, message);  \
    }                                           \
    catch(...)                                  \
    {                                           \
        throw;                                  \
    }                                           \
}

#define ASYNC_FROM_TASK(task) pplx::create_async([task] { return task; })

