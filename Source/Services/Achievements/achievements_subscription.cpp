// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievements_internal.h"

#include "xsapi_utils.h"
#include "xsapi-c/errors_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

AchievementProgressChangeSubscription::AchievementProgressChangeSubscription(
    _In_ uint64_t xuid,
    _In_ const xbox::services::String& scid
) noexcept
    : m_userId(xuid)
{
    Stringstream uri;
    uri << "https://achievements.xboxlive.com/users/xuid(" << m_userId << ")/achievements/" << utils::ToLower(scid);
    m_resourceUri = uri.str();
}

XblFunctionContext AchievementProgressChangeSubscription::AddHandler(
    AchievementProgressChangeHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

size_t AchievementProgressChangeSubscription::RemoveHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers.erase(token);
    return m_handlers.size();
}

XblAchievementProgressChangeEntry AchievementProgressChangeSubscription::DeepCopyProgressChangeEntry(XblAchievementProgressChangeEntry & source)
{
    XblAchievementProgressChangeEntry entry;
    
    entry.achievementId = Make(source.achievementId);
    entry.progressState = source.progressState;
    entry.progression.requirementsCount = source.progression.requirementsCount;
    entry.progression.timeUnlocked = source.progression.timeUnlocked;
    entry.progression.requirements = MakeArray<XblAchievementRequirement>(source.progression.requirementsCount);
    
    XblAchievementRequirement*& eventRequirements = entry.progression.requirements;
    for (uint64_t i = 0; i < entry.progression.requirementsCount; ++i)
    {
        eventRequirements[i].id = Make(source.progression.requirements[i].id);
        eventRequirements[i].currentProgressValue = Make(source.progression.requirements[i].currentProgressValue);
        eventRequirements[i].targetProgressValue = Make(source.progression.requirements[i].targetProgressValue);
    }

    return entry;
}

void AchievementProgressChangeSubscription::CleanUpProgressChangeEntry(XblAchievementProgressChangeEntry & entry)
{
    Delete(entry.achievementId);
    auto& progression = entry.progression;

    for (uint64_t i = 0; i < progression.requirementsCount; ++i)
    {
        Delete(progression.requirements[i].id);
        Delete(progression.requirements[i].currentProgressValue);
        Delete(progression.requirements[i].targetProgressValue);
    }
    DeleteArray(progression.requirements, progression.requirementsCount);
}

void AchievementProgressChangeSubscription::OnEvent(
    const JsonValue& data
) noexcept
{
    //  Payload format at http://xboxwiki/wiki/RTA%3AEVENT#Achievements is currently out of
    //  date. Below is the payload format discovered while debugging.
    //  [<API_ID>, <SUB_ID>, 
    //  {
    //      "progression": [
    //          {
    //              "id" : "1",
    //              "progressState" : "InProgress",
    //              "timeUnlocked" : "2013-01-17T03:19:00.3087016Z",
    //              "requirements" : [
    //                  {
    //                      "id":"12345678-1234-1234-1234-123456789012",
    //                      "current" : "1",
    //                      "target" : "100",
    //                      "operationType" : "sum",
    //                      "valueType" : "Integer",
    //                      "ruleParticipationType" : "Individual"
    //                  }
    //              ]
    //          }
    //      ],
    //      "serviceConfigId" : "87654321-4321-4321-4321-210987654321"
    //  }
    
    HRESULT hr(S_OK);
    xbox::services::Vector<XblAchievementProgressChangeEntry> progressEntries;

    auto deserializeRequirement = [](const JsonValue& json) -> Result<XblAchievementRequirement> {
        XblAchievementRequirement requirement{};

        if (json.IsNull())
        {
            return Result<XblAchievementRequirement>{ requirement };
        }

        xbox::services::String id, current, target;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", id, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "current", current, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "target", target, false));
        requirement.id = Make(id);
        requirement.currentProgressValue = Make(current);
        requirement.targetProgressValue = Make(target);

        return Result<XblAchievementRequirement>{ requirement, S_OK };
    };

    auto deserializeProgressEntry = [&deserializeRequirement](const JsonValue& json) -> Result<XblAchievementProgressChangeEntry>
    {
        XblAchievementProgressChangeEntry progressEntry{};
        XblAchievementProgression progression{};

        if (json.IsNull())
        {
            return Result<XblAchievementProgressChangeEntry>{ progressEntry };
        }

        xbox::services::String achievementId, progressState;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", achievementId, true));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "progressState", progressState, true));
        
        xbox::services::Vector<XblAchievementRequirement> requirementsVector;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievementRequirement>(
            deserializeRequirement,
            json, "requirements", requirementsVector, true
        ));

        progression.requirements = MakeArray(requirementsVector);
        progression.requirementsCount = requirementsVector.size();
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "timeUnlocked", progression.timeUnlocked, true));

        progressEntry.progression = progression;
        progressEntry.achievementId = Make(achievementId);
        progressEntry.progressState = EnumValue<XblAchievementProgressState>(progressState.data());

        return Result<XblAchievementProgressChangeEntry>{progressEntry, S_OK};
    };

    hr = JsonUtils::ExtractJsonVector<XblAchievementProgressChangeEntry>(
        deserializeProgressEntry,
        data,
        "progression",
        progressEntries,
        true
    );

    if (!SUCCEEDED(hr))
    {
        LOGS_ERROR << __FUNCTION__ << ": Ignoring malformed payload";
        return;
    }

    XblAchievementProgressChangeEventArgs args{ progressEntries.data(), progressEntries.size() };

    if (SUCCEEDED(hr))
    {
        std::unique_lock<std::mutex> lock{ m_lock };
        auto handlers{ m_handlers };
        lock.unlock();

        for (auto& handler : handlers)
        {
            handler.second(args);
        }
    }
    else
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
    }
    
    // clean up allocations for each of the entries.
    for (auto& entry : progressEntries)
    {
        Delete(entry.achievementId);
        auto& progression = entry.progression;
        
        for (uint64_t i = 0; i < progression.requirementsCount; ++i)
        {
            Delete(progression.requirements[i].id);
            Delete(progression.requirements[i].currentProgressValue);
            Delete(progression.requirements[i].targetProgressValue);
        }

        DeleteArray(progression.requirements, progression.requirementsCount);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END
