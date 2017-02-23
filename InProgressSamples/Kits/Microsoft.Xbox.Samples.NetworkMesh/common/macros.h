// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

// forward declarations
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN
class Utils;
NAMESPACE_MICROSOFT_XBOX_SERVICES_END;


// preprocessor workaround
// to work with L##__FUNCTION__ and L##__FILE__ statements.
#define TEXTW(quote) _TEXTW(quote)
#define _TEXTW(quote) L##quote

#define LOG_INFO_MSG(msg) { OutputDebugString( msg ); }
#define LOG_ERROR_MSG(msg) if( m_xboxLiveContextSettings->IsAtDiagnosticsTraceLevel(XboxNetworkMeshDiagnosticsTraceLevel::Error) ) { OutputDebugString( msg ); }
#define LOG_EXCEPTION(hr) { Microsoft::Xbox::Samples::NetworkMesh::Utils::LogExceptionDebugInfo(hr, TEXTW(__FUNCTION__), TEXTW(__FILE__), __LINE__ ); }
#define THROW_INVALIDARGUMENT_IF(x) if ( x ) { LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_INVALIDARGUMENT_IF_WITH_LOG(x,msg) if ( x ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_INVALIDARGUMENT_IF_NULL(x) if ( ( x ) == nullptr ) { LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_INVALIDARGUMENT_IF_NULL_WITH_LOG(x,msg) if ( ( x ) == nullptr ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); }
#define THROW_E_POINTER_IF_NULL(x) if ( ( x ) == nullptr ) { LOG_EXCEPTION(E_POINTER); throw ref new Platform::COMException(E_POINTER); }
#define THROW_E_POINTER_IF_NULL_WITH_LOG(x,msg) if ( ( x ) == nullptr ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(E_POINTER); throw ref new Platform::COMException(E_POINTER); }
#define THROW_INVALIDARGUMENT_IF_STRING_EMPTY(x) { auto y = x; if ( y->IsEmpty() ) { LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); } }
#define THROW_INVALIDARGUMENT_IF_STRING_EMPTY_WITH_LOG(x,msg) { auto y = x; if ( y->IsEmpty() ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(E_INVALIDARG); throw ref new Platform::InvalidArgumentException(); } }
#define THROW_IF_HR_FAILED(hr) { HRESULT hr2 = hr; if ( FAILED( hr2 ) ) { LOG_EXCEPTION(hr2); throw ref new Platform::COMException(hr2); } }
#define THROW_IF_HR_FAILED_WITH_LOG(hr,msg) { HRESULT hr2 = hr; if ( FAILED( hr2 ) ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(hr2); throw ref new Platform::COMException(hr2); } }
#define THROW_HR(hr) { LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_HR_WITH_LOG(hr,msg) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_HR_IF(x,hr) if ( x ) { LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_HR_IF_WITH_LOG(x,hr,msg) if ( x ) { LOG_ERROR_MSG( msg ); LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_WIN32_IF(x,e) if ( x ) { HRESULT hr = __HRESULT_FROM_WIN32(e); LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }
#define THROW_WIN32_IF_WITH_LOG(x,e,msg) if ( x ) { HRESULT hr = __HRESULT_FROM_WIN32(e); LOG_ERROR_MSG( msg ); LOG_EXCEPTION(hr); throw ref new Platform::COMException(hr); }


#define E_INVALIDARG_IF(x) if ( x ) { return E_INVALIDARG; }
#define E_POINTER_IF_NULL(x) if ( ( x ) == nullptr ) { return E_POINTER; }
#define E_POINTER_OR_INVALIDARG_IF_STRING_EMPTY(x) { auto y = x; E_POINTER_IF_NULL( y ); E_INVALIDARG_IF( wcslen( y ) == 0 ); }
#define CHECKHR_EXIT(hrResult) { hr = hrResult; if ( FAILED( hr ) ) { goto exit; } }

#define TV_API (WINAPI_FAMILY == WINAPI_FAMILY_TV_APP | WINAPI_FAMILY == WINAPI_FAMILY_TV_TITLE)

#define NAMESPACE_MICROSOFT_XBOX_SAMPLES_NETWORKMESH_BEGIN  namespace Microsoft { namespace Xbox { namespace Samples { namespace NetworkMesh {
#define NAMESPACE_MICROSOFT_XBOX_SAMPLES_NETWORKMESH_END }}}}
#define USING_NAMESPACE_MICROSOFT_XBOX_SAMPLES_NETWORKMESH using namespace Microsoft::Xbox::Samples::NetworkMesh;


inline int64 SECONDS_TO_100NS(int32 x)
{
    return ((int64)x) * 10000000;
}
