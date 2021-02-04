#include "pch.h"
#include "xsapi_json_utils.h"

using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

HRESULT JsonUtils::ExtractJsonFieldAsString(
    _In_ const JsonValue& json,
    _In_ const xsapi_internal_string& name,
    _Inout_ xsapi_internal_string& outString,
    _In_ bool required
)
{
    if (json.IsObject())
    {
        if (json.HasMember(name.c_str()))
        {
            const JsonValue& jsonField = json[name.c_str()];
            outString = SerializeJson(jsonField);
            return S_OK;
        }
        else if (!required)
        {
            return S_OK;
        }
    }

    return WEB_E_INVALID_JSON_STRING;
}

 HRESULT JsonUtils::ExtractJsonStringVector(
    _In_ const JsonValue& json,
    _In_ const xsapi_internal_string& name,
    _Inout_ xsapi_internal_vector<xsapi_internal_string>& outVector,
    _In_ bool required
)
{
    if (json.IsObject())
    {
        if (json.HasMember(name.c_str()))
        {
            return ExtractJsonStringVector(
                json[name.c_str()],
                outVector
            );
        }
        else if (!required)
        {
            outVector = xsapi_internal_vector<xsapi_internal_string>();
            return S_OK;
        }
    }

    outVector = xsapi_internal_vector<xsapi_internal_string>();
    return WEB_E_INVALID_JSON_STRING;
}

 HRESULT JsonUtils::ExtractJsonStringVector(
     _In_ const JsonValue& json,
     _Inout_ xsapi_internal_vector<xsapi_internal_string>& outVector
 )
 {
     outVector = xsapi_internal_vector<xsapi_internal_string>();
     if (!json.IsArray())
     {
         return WEB_E_INVALID_JSON_STRING;
     }

     for (const auto& string : json.GetArray())
     {
         if (!string.IsString())
         {
             return WEB_E_INVALID_JSON_STRING;
         }
         outVector.push_back(string.GetString());
     }

     return S_OK;
}

Result<xsapi_internal_string> JsonUtils::JsonStringExtractor(_In_ const JsonValue& json)
{
    if (!json.IsString())
    {
        return Result<xsapi_internal_string>(WEB_E_INVALID_JSON_STRING);
    }
    return Result<xsapi_internal_string>(json.GetString());
}

void JsonUtils::JsonStringSerializer(_In_ const xsapi_internal_string& value, _Out_ JsonValue& json, JsonDocument::AllocatorType& allocator)
{
    json.SetString(value.c_str(), allocator);
}

void JsonUtils::JsonXuidSerializer(_In_ uint64_t xuid, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    json.SetString(utils::uint64_to_internal_string(xuid).c_str(), allocator);
}

Result<int> JsonUtils::JsonIntExtractor(_In_ const JsonValue& json)
{
    if (!json.IsInt())
    {
        return Result<int>(0, WEB_E_INVALID_JSON_STRING);
    }
    return Result<int>(json.GetInt(), S_OK);
}

Result<uint64_t>
JsonUtils::JsonXuidExtractor(_In_ const JsonValue& json)
{
    if (!json.IsString())
    {
        return Result<uint64_t>(WEB_E_INVALID_JSON_STRING);
    }

    return Result<uint64_t>(utils::internal_string_to_uint64(json.GetString()));
}

void JsonUtils::JsonUtf8Serializer(_In_ const char* value, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    //TODO: Verify UTF8 format is maintained
    json.SetString(value, allocator);
}

Result<const char*> JsonUtils::JsonUtf8Extractor(_In_ const JsonValue& json)
{
    if (!json.IsString())
    {
        return Result<const char*>(WEB_E_INVALID_JSON_STRING);
    }
    return Result<const char*>(Make(json.GetString()));
}

void JsonUtils::JsonIntSerializer(_In_ int32_t value, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType&)
{
    json.SetInt(value);
}

HRESULT JsonUtils::ExtractJsonXuid(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Out_ uint64_t& xuid,
    _In_ bool required /*= false*/
)
{
    xsapi_internal_string xuidString;
    RETURN_HR_IF_FAILED(ExtractJsonString(jsonValue, name, xuidString, required));
    xuid = utils::internal_string_to_uint64(xuidString);
    return S_OK;
}

HRESULT JsonUtils::ExtractJsonString(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _Inout_ xsapi_internal_string& outString,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(stringName.c_str()))
        {
            const JsonValue& field = jsonValue[stringName.c_str()];
            if (field.IsString())
            {
                outString = field.GetString();
                return S_OK;
            }
            else if (field.IsNull())
            {
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

HRESULT JsonUtils::ExtractJsonStringToCharArray(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _Inout_updates_bytes_(size) char* charArray,
    _In_ size_t size
)
{
    xsapi_internal_string jsonStr;
    RETURN_HR_IF_FAILED(ExtractJsonString(jsonValue, stringName, jsonStr));
    utils::strcpy(charArray, size, jsonStr.data());

    return S_OK;
}

JsonValue::ConstArray JsonUtils::ExtractJsonArray(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& arrayName,
    _In_ bool required
)
{
    if (jsonValue.IsObject() && jsonValue.HasMember(arrayName.c_str()))
    {

        const JsonValue& field = jsonValue[arrayName.c_str()];
        if ((!field.IsArray() && !required) || field.IsNull())
        {
            const JsonValue emptyArrayJson(rapidjson::kArrayType);
            return emptyArrayJson.GetArray();
        }
        return field.GetArray();
    }

    const JsonValue emptyArrayJson(rapidjson::kArrayType);
    return emptyArrayJson.GetArray();
}

 HRESULT JsonUtils::ExtractJsonAsString(
    _In_ const JsonValue& jsonValue,
    _Inout_ xsapi_internal_string& outString
)
{
    if (jsonValue.IsString())
    {
        outString = jsonValue.GetString();
        return S_OK;
    }

    return WEB_E_INVALID_JSON_STRING;
}

HRESULT JsonUtils::ExtractJsonBool(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _Inout_ bool& outBool,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(stringName.c_str()))
        {
            const JsonValue& field = jsonValue[stringName.c_str()];
            if (field.IsBool())
            {
                outBool = field.GetBool();
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

HRESULT JsonUtils::ExtractJsonInt(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ int32_t& outInt,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];

            if (field.IsInt())
            {
                outInt = field.GetInt();
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

HRESULT JsonUtils::ExtractJsonInt(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ uint32_t& outInt,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];

            if (field.IsUint())
            {
                outInt = field.GetUint();
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

HRESULT JsonUtils::ExtractJsonInt(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ int64_t& outInt,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];

            if (field.IsInt64())
            {
                outInt = field.GetInt64();
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

HRESULT JsonUtils::ExtractJsonInt(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ uint64_t& outInt,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];

            if (field.IsUint64())
            {
                outInt = field.GetUint64();
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

HRESULT JsonUtils::ExtractJsonSizeT(
    _In_ const JsonValue& jsonValue,
    _In_ const String& name,
    _Inout_ size_t& size,
    _In_ bool required
)
{
    uint64_t temp{};
    RETURN_HR_IF_FAILED(ExtractJsonInt(jsonValue, name, temp, required));
    size = static_cast<size_t>(temp);
    return S_OK;
}

HRESULT JsonUtils::ExtractJsonStringToUInt64(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ uint64_t& outUInt64,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];
            if (field.IsString())
            {
                outUInt64 = utils::internal_string_to_uint64(field.GetString());
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

HRESULT JsonUtils::ExtractJsonUInt64(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ uint64_t& outUInt64,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];
            if (field.IsNumber())
            {
                outUInt64 = field.GetUint64();
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

 HRESULT JsonUtils::ExtractJsonTime(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ xbox::services::datetime& outTime,
    _In_ bool required
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];

            if (field.IsString())
            {
                //convert to wstring for use with xbox::services::datetime
                //xbox::services::datetime is still part of cpprestsdk
                outTime = xbox::services::datetime::from_string(field.GetString(), xbox::services::datetime::date_format::ISO_8601);
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

HRESULT JsonUtils::ExtractJsonTimeT(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ time_t& outTime,
    _In_ bool required
)
{
    xbox::services::datetime time;
    RETURN_HR_IF_FAILED(ExtractJsonTime(jsonValue, name, time, required));
    outTime = utils::time_t_from_datetime(time);
    return S_OK;
}

 HRESULT JsonUtils::ExtractJsonStringTimespanInSeconds(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ std::chrono::seconds& outTime,
    _In_ bool required)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];
            if (field.IsString())
            {
                char delimiter;
                int hour = 0, min = 0, sec = 0;
                xsapi_internal_stringstream ss(field.GetString());
                ss >> hour >> delimiter >> min >> delimiter >> sec;

                outTime = std::chrono::hours(hour) + std::chrono::minutes(min) + std::chrono::seconds(sec);
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

HRESULT JsonUtils::ExtractJsonDouble(
    _In_ const JsonValue& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ double& outDouble,
    _In_ bool required /* = false */
)
{
    if (jsonValue.IsObject())
    {
        if (jsonValue.HasMember(name.c_str()))
        {
            const JsonValue& field = jsonValue[name.c_str()];
            if (field.IsDouble())
            {
                outDouble = field.GetDouble();
                return S_OK;
            }

        }
        else if(!required)
        {
            return S_OK;
        }
    } 

    return WEB_E_INVALID_JSON_STRING;
}


void
JsonUtils::SerializeUInt52ToJson(
    _In_ uint64_t integer,
    _Inout_ JsonValue& json
)
{
    if ((integer & 0xFFF0000000000000) != 0)
    {
        //TODO: Throw exception here
        return;
    }

    json.SetUint64(integer);
}

JsonValue JsonUtils::SerializeTime(
    _In_ time_t time,
    _In_ JsonDocument::AllocatorType& a
) noexcept
{
    auto timestampString = DatetimeFromTimeT(time).to_string_internal(xbox::services::cppresturi::utility::datetime::ISO_8601);
    return JsonValue{ timestampString.data(), a };
}

HRESULT JsonUtils::ValidateJson(
    _In_ const char* jsonString
)
{
    if (jsonString != nullptr)
    {
        JsonDocument d;
        d.Parse(jsonString);
        if (d.HasParseError())
        {
            return WEB_E_INVALID_JSON_STRING;
        }
    }

    return S_OK;
}

HRESULT JsonUtils::ValidateJson(
    _In_ const char* jsonString,
    _Out_ JsonDocument& jsonDocument
)
{
    if (jsonString == nullptr)
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    jsonDocument.Parse(jsonString);
    if (jsonDocument.HasParseError())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return S_OK;
}

void JsonUtils::CopyFrom(JsonDocument& dest, const JsonValue& src)
{
    if (static_cast<void*>(&dest) != static_cast<void const*>(&src))
    {
        dest.CopyFrom(src, dest.GetAllocator());
    }
}

HRESULT JsonUtils::SetMember(
    _In_ JsonDocument& document,
    _In_ const String& key,
    _In_ const JsonValue& value
) noexcept
{
    return SetMember(document, document.GetAllocator(), key, value);
}

HRESULT JsonUtils::SetMember(
    _In_ JsonValue& object,
    _In_ JsonDocument::AllocatorType& a,
    _In_ const String& key,
    _In_ const JsonValue& value
) noexcept
{
    if (!object.IsObject())
    {
        return E_UNEXPECTED;
    }

    auto existingMember = object.FindMember(key.data());
    if (existingMember == object.MemberEnd())
    {
        object.AddMember(JsonValue{ key.data(), a }.Move(), JsonValue{}.CopyFrom(value, a).Move(), a);
    }
    else
    {
        existingMember->value.CopyFrom(value, a);
    }
    return S_OK;
}

xsapi_internal_string JsonUtils::SerializeJson(_In_ const JsonValue& json)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

void* JsonAllocator::Malloc(size_t size)
{
    if (size)
    {
        return xbox::services::Alloc(size);
    }
    return nullptr;
}

void* JsonAllocator::Realloc(void* originalPtr, size_t originalSize, size_t newSize)
{
    void* newPtr = nullptr;
    if (newSize > 0)
    {
        newPtr = Alloc(newSize);
        memcpy(newPtr, originalPtr, (originalSize < newSize ? originalSize : newSize));
    }
    xbox::services::Free(originalPtr);
    return newPtr;

}

void JsonAllocator::Free(void* ptr)
{
    xbox::services::Free(ptr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
