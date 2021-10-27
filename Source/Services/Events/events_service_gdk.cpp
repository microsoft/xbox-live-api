// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_gdk.h"

#if XSAPI_BUILD_WITH_1910_GRTS

#include "XGameEvent.h"
#include "XGameRuntimeFeature.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

EventsService::EventsService(
    User&& user
) :
    m_user{ std::move(user) },
    m_playSession{ utils::create_guid(true) }
{
}

EventsService::~EventsService()
{
}

HRESULT EventsService::Initialize()
{
    return S_OK;
}

HRESULT EventsService::WriteInGameEvent(
    _In_z_ const char* eventName,
    _In_opt_z_ const char* dimensions,
    _In_opt_z_ const char* measurements
)
{
    if (XGameRuntimeIsFeatureAvailable(XGameRuntimeFeature::XGameEvent))
    {
        const char* scid{ nullptr };
        HRESULT hr = XblGetScid(&scid);
        if (FAILED(hr))
        {
            return hr;
        }

        String lowercaseScid = utils::ToLower(scid);

        return XGameEventWrite(
            m_user.Handle(),
            lowercaseScid.c_str(),
            m_playSession.c_str(), 
            eventName, 
            dimensions, 
            measurements);
    }
    else
    {
        return E_NOTIMPL;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#endif