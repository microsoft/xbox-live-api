// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"

using namespace xbox::services;
using namespace xbox::services::social;

Result<std::shared_ptr<XblSocialRelationshipResult>> XblSocialRelationshipResult::Deserialize(
    const JsonValue& json
)
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    auto result{ MakeShared<XblSocialRelationshipResult>() };

    HRESULT errc = S_OK;
    
    uint64_t totalCount = 0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "totalCount", totalCount, false));
    result->m_totalCount = static_cast<size_t>(totalCount);

    if (json.IsObject() && json.HasMember("people"))
    {
        const JsonValue& peopleJson = json["people"];
        if (peopleJson.IsArray())
        {
            result->m_socialRelationships.reserve(peopleJson.Size());
            result->m_socialNetworks.reserve(peopleJson.Size());

            for (const JsonValue& person : peopleJson.GetArray())
            {
                if (person.IsObject())
                {
                    XblSocialRelationship relationship{};
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(person, "xuid", relationship.xboxUserId, true));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(person, "isFavorite", relationship.isFavorite));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(person, "isFollowingCaller", relationship.isFollowingCaller));

                    xsapi_internal_vector<xsapi_internal_string> socialNetworksInternalVector;
                    JsonUtils::ExtractJsonVector<xsapi_internal_string>(JsonUtils::JsonStringExtractor, person, "socialNetworks", socialNetworksInternalVector, false);
                    UTF8StringArray socialNetworks(socialNetworksInternalVector);
                    relationship.socialNetworks = socialNetworks.Data();
                    relationship.socialNetworksCount = socialNetworks.Size();

                    result->m_socialRelationships.push_back(std::move(relationship));
                    result->m_socialNetworks.push_back(std::move(socialNetworks));
                }
            }
        }
    }

    return Result<std::shared_ptr<XblSocialRelationshipResult>>{ result, errc };
}

const xsapi_internal_vector<XblSocialRelationship>& XblSocialRelationshipResult::SocialRelationships() const noexcept
{
    return m_socialRelationships;
}

bool XblSocialRelationshipResult::HasNext() const noexcept
{
    return ContinuationSkip < m_totalCount;
}

size_t XblSocialRelationshipResult::TotalCount() const noexcept
{
    return m_totalCount;
}

std::shared_ptr<RefCounter> XblSocialRelationshipResult::GetSharedThis()
{
    return shared_from_this();
}
