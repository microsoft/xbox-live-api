// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "Club_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubsOwnedResult sealed
{
public:
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ ClubIds
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    DEFINE_PROP_GET_OBJ(RemainingClubs, remaining_clubs, uint32);

internal:
    ClubsOwnedResult(_In_ xbox::services::clubs::clubs_owned_result cppObj);

private:
    xbox::services::clubs::clubs_owned_result m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_clubIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END