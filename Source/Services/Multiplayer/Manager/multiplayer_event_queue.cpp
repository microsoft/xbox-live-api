// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerEventQueue::MultiplayerEventQueue()
{
}

MultiplayerEventQueue::MultiplayerEventQueue(const MultiplayerEventQueue& other)
{
    {
        std::lock_guard<std::mutex> lock(other.m_lock);
        m_events = other.m_events;
    }

    for (auto& event : m_events)
    {
        if (event.ErrorMessage)
        {
            event.ErrorMessage = Make(event.ErrorMessage);
        }
        if (event.EventArgsHandle)
        {
            event.EventArgsHandle->AddRef();
        }
    }
}

MultiplayerEventQueue& MultiplayerEventQueue::operator=(MultiplayerEventQueue other)
{
    std::lock_guard<std::mutex> lock(m_lock);
    std::swap(m_events, other.m_events);
    return *this;
}

MultiplayerEventQueue::~MultiplayerEventQueue()
{
    for (const auto& event : m_events)
    {
        if (event.ErrorMessage)
        {
            Delete(event.ErrorMessage);
        }
        if (event.EventArgsHandle)
        {
            event.EventArgsHandle->DecRef();
        }
    }
}

size_t MultiplayerEventQueue::Size() const
{
    return m_events.size();
}

bool MultiplayerEventQueue::Empty() const
{
    return m_events.empty();
}

void MultiplayerEventQueue::Clear()
{
    std::lock_guard<std::mutex> lock(m_lock);
    for (auto& e : m_events)
    {
        if (e.ErrorMessage)
        {
            Delete(e.ErrorMessage);
        }
        if (e.EventArgsHandle)
        {
            e.EventArgsHandle->DecRef();
        }
    }
    m_events.clear();
}

xsapi_internal_vector<XblMultiplayerEvent>::const_iterator MultiplayerEventQueue::begin() const
{
    return m_events.begin();
}

xsapi_internal_vector<XblMultiplayerEvent>::const_iterator MultiplayerEventQueue::end() const
{
    return m_events.end();
}

void MultiplayerEventQueue::AddEvent(
    _In_ XblMultiplayerEventType eventType,
    _In_ XblMultiplayerSessionType sessionType,
    _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs,
    _In_ Result<void> error,
    _In_opt_ context_t context
)
{
    std::lock_guard<std::mutex> lock(m_lock);

    XblMultiplayerEvent event{};
    event.EventType = eventType;
    event.SessionType = sessionType;
    if (eventArgs)
    {
        eventArgs->AddRef();
        event.EventArgsHandle = eventArgs.get();
    }
    event.Result = error.Hresult();
    event.ErrorMessage = Make(error.ErrorMessage());

#if XSAPI_WINRT
    event.Context = reinterpret_cast<void*>(context);
#else
    event.Context = context;
#endif

    m_events.push_back(event);
}

void MultiplayerEventQueue::AddEvent(_In_ const XblMultiplayerEvent& multiplayerEvent)
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_events.push_back(multiplayerEvent);
    auto& addedEvent = m_events.back();
    if (addedEvent.ErrorMessage)
    {
        addedEvent.ErrorMessage = Make(addedEvent.ErrorMessage);
    }
    if (addedEvent.EventArgsHandle)
    {
        addedEvent.EventArgsHandle->AddRef();
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END