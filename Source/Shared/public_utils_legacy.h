// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "HookedUri/uri.h"
#include "HookedUri/uri_builder.h"
#include "HookedUri/asyncrt_utils.h"

#ifndef MAKE_HTTP_HRESULT
#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbl_error_code ConvertHrToXblErrorCode(HRESULT hr);

namespace legacy
{
    string_t StringTFromUtf8( _In_z_ const char* utf8 );

    std::string StringFromStringT(_In_ const string_t& stringt);

    int Utf8FromCharT( _In_z_ const char_t* inArray,
                       _Out_writes_z_(cchOutArray) char* outArray,
                       _In_ int cchOutArray );

    int CharTFromUft8( _In_z_ const char* inArray,
                       _Out_writes_z_(cchOutArray) char_t* outArray,
                       _In_ int cchOutArray );

    size_t CopyUtf8( _In_ char* destinationCharArr,
                     _In_ size_t sizeInWords,
                     _In_ const char* sourceCharArr );

    string_t StringTFromUint64(_In_ uint64_t val);

    std::string StringFromUint64(_In_ uint64_t val);

    uint64_t Uint64FromStringT(_In_ const string_t& str);

    int Stricmp(const char* left, const char* right) noexcept;

    int Stricmp(const string_t& left, const string_t& right);
    
    std::string SerializeJson(const rapidjson::Value& json);

    const rapidjson::Value& ExtractJsonField(
        _In_ const rapidjson::Value& json,
        _In_ const std::string& name,
        _In_ bool required
    );

    uint64_t ExtractJsonUint64(
        _In_ const rapidjson::Value& jsonValue,
        _In_ const std::string& name,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    template<typename TOut, typename InputIt, typename Transformer>
    std::vector<TOut> Transform(InputIt first, InputIt last, Transformer op)
    {
        std::vector<TOut> out;
        std::transform(first, last, std::back_inserter(out), op);
        return out;
    }

    template<typename TOut, typename TIn, typename Transformer>
    std::vector<TOut> Transform(const std::vector<TIn>& in, Transformer op)
    {
        return Transform<TOut>(in.begin(), in.end(), op);
    }

    template<typename TOut, typename TIn, typename Transformer>
    std::vector<TOut> Transform(TIn* inArray, size_t inArrayCount, Transformer op)
    {
        return Transform<TOut>(inArray, inArray + inArrayCount, op);
    }

    template<typename TOut, typename TIn>
    std::vector<TOut> Transform(TIn* inArray, size_t inArrayCount)
    {
        return Transform<TOut>(inArray, inArrayCount, [](const TIn& in)
            {
                return TOut(in);
            });
    }

    std::vector<string_t> XuidStringVectorFromXuidArray(const uint64_t* xuids, size_t xuidsCount);

    std::vector<uint64_t> XuidVectorFromXuidStringVector(const std::vector<string_t>& xuidStrings);

    std::vector<string_t> StringTVectorFromCStringArray(const char** stringArray, size_t arrayCount);

    xbox::services::cppresturi::utility::datetime DatetimeFromTimeT(time_t time);

    time_t TimeTFromDatetime(const xbox::services::cppresturi::utility::datetime& datetime);

    char_t ToLower(char_t c);

    std::error_code ConvertHr(HRESULT hr);


#if !XSAPI_NO_PPL

    template<typename T>
    struct AsyncWrapper
    {
        typedef std::function<HRESULT(XAsyncBlock*, T&)> ResultExtractor;

        AsyncWrapper(ResultExtractor resultExtractor)
            : m_resultExtractor(std::move(resultExtractor))
        {
            async.queue = XblGetAsyncQueue();
            async.context = this;
            async.callback = [](XAsyncBlock* async)
            {
                auto thisPtr = static_cast<AsyncWrapper<T>*>(async->context);
                T result;
                auto hr = thisPtr->m_resultExtractor(async, result);
                if (SUCCEEDED(hr))
                {
                    thisPtr->m_taskCompletionEvent.set(xbl_result<T>(result));
                }
                else
                {
                    thisPtr->m_taskCompletionEvent.set(xbl_result<T>(ConvertHr(hr)));
                }
                delete thisPtr;
            };
        }

        XAsyncBlock async{};

        // If the Async API fails, the callback will never be invoked. Return a failure task and self destruct.
        pplx::task<xbl_result<T>> Task(HRESULT asyncApiResult)
        {
            if (SUCCEEDED(asyncApiResult))
            {
                return pplx::task<xbl_result<T>>(m_taskCompletionEvent);
            }
            else
            {
                delete this;
                return pplx::task_from_result(xbl_result<T>(ConvertHr(asyncApiResult)));
            }
        }

        private:
        AsyncWrapper(const AsyncWrapper&) = delete;
        AsyncWrapper& operator=(AsyncWrapper) = delete;

        ResultExtractor m_resultExtractor;
        pplx::task_completion_event<xbl_result<T>> m_taskCompletionEvent;
    };

    template<>
    struct AsyncWrapper<void>
    {
        typedef std::function<HRESULT(XAsyncBlock*)> ResultExtractor;

        AsyncWrapper() : AsyncWrapper{ [](XAsyncBlock* async) { return XAsyncGetStatus(async, false); } }
        {
        }

        AsyncWrapper(ResultExtractor resultExtractor)
            : m_resultExtractor{ std::move(resultExtractor) }
        {
            async.queue = XblGetAsyncQueue();
            async.context = this;
            async.callback = [](XAsyncBlock* async)
            {
                auto thisPtr = static_cast<AsyncWrapper<void>*>(async->context);
                auto hr = thisPtr->m_resultExtractor(async);
                thisPtr->m_taskCompletionEvent.set(xbl_result<void>(ConvertHr(hr)));
                delete thisPtr;
            };
        }

        XAsyncBlock async{};

        pplx::task<xbl_result<void>> Task(HRESULT asyncApiResult)
        {
            if (SUCCEEDED(asyncApiResult))
            {
                return pplx::task<xbl_result<void>>(m_taskCompletionEvent);
            }
            else
            {
                // If the Async API fails, the callback will never be invoked. Return a failure task and self destruct.
                delete this;
                return pplx::task_from_result(xbl_result<void>(ConvertHr(asyncApiResult)));
            }
        }

        private:
        AsyncWrapper(const AsyncWrapper&) = delete;
        AsyncWrapper& operator=(AsyncWrapper) = delete;

        ResultExtractor m_resultExtractor;
        pplx::task_completion_event<xbl_result<void>> m_taskCompletionEvent;
    };

#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
