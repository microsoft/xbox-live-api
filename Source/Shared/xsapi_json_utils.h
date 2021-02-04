#pragma once
#include "internal_errors.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class JsonAllocator
{
public:
    static const bool kNeedFree = true;

    void* Malloc(size_t size);
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize);

    static void Free(void* ptr);
};

typedef rapidjson::GenericDocument<rapidjson::UTF8<>, JsonAllocator> JsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator> JsonValue;

class JsonUtils
{
public:
    static HRESULT ExtractJsonXuid(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Out_ uint64_t& xuid,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonString(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _Inout_ xsapi_internal_string& outString,
        _In_ bool required = false
    );
    
    static HRESULT ExtractJsonStringToCharArray(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _Inout_updates_bytes_(size) char* charArr,
        _In_ size_t size
    );

    static HRESULT ExtractJsonAsString(
        _In_ const JsonValue& jsonValue,
        _Inout_ xsapi_internal_string& outString
    );

    static JsonValue::ConstArray ExtractJsonArray(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& arrayName,
        _In_ bool required
    );

    static HRESULT ExtractJsonBool(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _Inout_ bool& outBool,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonInt(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ int32_t& outInt,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonInt(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ uint32_t& outInt,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonInt(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ int64_t& outInt,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonInt(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ uint64_t& outInt,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonSizeT(
        _In_ const JsonValue& jsonValue,
        _In_ const String& name,
        _Inout_ size_t& size,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonStringToUInt64(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ uint64_t& outUInt64,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonUInt64(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ uint64_t& outUInt64,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonTime(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ xbox::services::datetime& outTime,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonTimeT(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ time_t& outTime,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonStringTimespanInSeconds(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _Inout_ std::chrono::seconds& outTime,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonDouble(
        _In_ const JsonValue& jsonValue,
        _In_ const xsapi_internal_string& name,
        _Inout_ double& outDouble,
        _In_ bool required = false
    );

    static HRESULT ExtractJsonFieldAsString(
        _In_ const JsonValue& json,
        _In_ const xsapi_internal_string& name,
        _Inout_ xsapi_internal_string& outString,
        _In_ bool required
    );

    static HRESULT ExtractJsonStringVector(
        _In_ const JsonValue& json,
        _In_ const xsapi_internal_string& name,
        _Inout_ xsapi_internal_vector<xsapi_internal_string>& outVector,
        _In_ bool required
    );

    static HRESULT ExtractJsonStringVector(
        _In_ const JsonValue& json,
        _Inout_ xsapi_internal_vector<xsapi_internal_string>& outVector
    );

    template<typename T, typename F>
    static HRESULT ExtractJsonVector(
        _In_ F deserialize,
        _In_ const JsonValue& json,
        _In_ const xsapi_internal_string& name,
        _Inout_ xsapi_internal_vector<T>& outVector,
        _In_ bool required
    )
    {
        outVector = xsapi_internal_vector<T>();
        if (json.IsObject())
        {
            if (json.HasMember(name.c_str()))
            {
                const JsonValue& field = json[name.c_str()];
                if (field.IsArray())
                {
                    for (auto it = field.Begin(); it != field.End(); ++it)
                    {
                        auto obj = deserialize(*it);
                        if (Failed(obj))
                        {
                            return obj.Hresult();
                            break;
                        }
                        outVector.push_back(obj.Payload());
                    }
                    return S_OK;
                }
            }
            else if (!required)
            {
                return S_OK;
            }
        }
        return WEB_E_INVALID_JSON_STRING;
    }

    template<typename T, typename F>
    static HRESULT ExtractJsonVector(
        _In_ F deserialize,
        _In_ const JsonValue& json,
        _Inout_ xsapi_internal_vector<T>& outVector
    )
    {
        outVector = xsapi_internal_vector<T>();
        if (!json.IsArray())
        {
            return WEB_E_INVALID_JSON_STRING;
        }

        for (auto it = json.Begin(); it != json.End(); ++it)
        {
            auto obj = deserialize(*it);
            if (Failed(obj))
            {
                return obj.Hresult();
                break;
            }
            outVector.push_back(obj.Payload());
        }

        return S_OK;
    }

    static Result<xsapi_internal_string> JsonStringExtractor(_In_ const JsonValue& json);

    static void JsonStringSerializer(_In_ const xsapi_internal_string& value, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

    static void JsonXuidSerializer(_In_ uint64_t xuid, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

    static Result<int> JsonIntExtractor(_In_ const JsonValue& json);

    static Result<uint64_t> JsonXuidExtractor(_In_ const JsonValue& json);

    static void JsonUtf8Serializer(_In_ const char* value, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

    // Note that this function allocates memory that must be freed by the caller.
    static Result<const char*> JsonUtf8Extractor(_In_ const JsonValue& json);

    static void JsonIntSerializer(_In_ int32_t value, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& UNUSED);

    template<typename T, typename F>
    static void SerializeVector(
        _In_ F serializer,
        _In_ xsapi_internal_vector<T> inputVector,
        _Out_ JsonValue& jsonArray,
        _In_ JsonDocument::AllocatorType& allocator
    )
    {
        jsonArray.SetArray();

        for (auto& s : inputVector)
        {
            JsonValue val;
            serializer(s, val, allocator);
            jsonArray.PushBack(val, allocator);
        }
    }

    static void SerializeUInt52ToJson(_In_ uint64_t integer, _Inout_ JsonValue& json);

    static JsonValue SerializeTime(
        _In_ time_t time,
        _In_ JsonDocument::AllocatorType& allocator
    ) noexcept;

    static HRESULT ValidateJson(_In_ const char* jsonString);
    static HRESULT ValidateJson(_In_ const char* jsonString, _Out_ JsonDocument& jsonDocument);

    static void CopyFrom(JsonDocument& dest, const JsonValue& src);

    // Set a member of an json object to a new value. The added value to be added will be deep copied.
    // Note that the semantics of JsonValue::AddMember are to add a second member with
    // the same key if one already exists; this method instead updates an existing member.
    static HRESULT SetMember(
        _In_ JsonDocument& document,
        _In_ const String& key,
        _In_ const JsonValue& value
    ) noexcept;

    static HRESULT SetMember(
        _In_ JsonValue& object,
        _In_ JsonDocument::AllocatorType& allocator,
        _In_ const String& key,
        _In_ const JsonValue& value
    ) noexcept;

    static xsapi_internal_string SerializeJson(_In_ const JsonValue& json);
};



NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
