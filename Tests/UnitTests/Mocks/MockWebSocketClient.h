// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "web_socket_client.h"
#include "xsapi/real_time_activity.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockWebSocketClient : public xbox_web_socket_client
{
public:
    pplx::task<void> connect(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ const web::uri& uri,
        _In_ const string_t& subProtocol
        ) override
    {
        if (m_waitForSignal)
        {
            m_connectEvent.wait();
        }

        if (m_connectToFail)
        {
            return pplx::task_from_exception<void>(std::runtime_error(""));
        }

        return pplx::task_from_result();
    }

    pplx::task<void> send(
        _In_ const string_t& message
        ) override
    {
        auto jsonValueObj = web::json::value::parse(message).as_array();
        real_time_activity::real_time_activity_message_type messageType = static_cast<real_time_activity::real_time_activity_message_type>(jsonValueObj[0].as_integer());
        
        if (messageType == real_time_activity::real_time_activity_message_type::subscribe)
        {
            uint32_t uniqueId = jsonValueObj[1].as_integer();
            string_t uri = jsonValueObj[2].as_string();
            m_subUriToIdMap[uri] = uniqueId;
        }
        if (m_sendHandler)
        {
            m_sendHandler(message);
        }
        return pplx::task_from_result();
    }

    pplx::task<void> close() override 
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_closeHandler)
        {
            m_closeHandler(static_cast<uint16_t>(m_closeStatus), L"");
        }
        return pplx::task_from_result();
    }

    void set_received_handler(
        _In_ std::function<void(string_t)> handler
        ) override 
    {
        m_receiveHandler = handler;
    }

    void set_closed_handler(
        _In_ std::function<void(uint16_t closeStatus, string_t closeReason)> handler
        ) override
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_closeHandler = handler;
    }

    void set_send_handler(
        _In_ std::function<void(string_t message)> handler
        )
    {
        m_sendHandler = handler;
    }

    void recieve_message(
        _In_ string_t message
        )
    {
        if (m_receiveHandler)
        {
            m_receiveHandler(std::move(message));
        }
        else
        {
            LOGS_DEBUG << "No MockWebSocketClient message received";
        }
    }

    void receive_rta_event_from_uri(const string_t& uri, const string_t& eventData, xbox::services::real_time_activity::real_time_activity_message_type dataType, bool isError)
    {
        if (m_subUriToIdMap.find(uri) == m_subUriToIdMap.end())
        {
            return;
        }
        stringstream_t ss;

        if (isError)
        {
            auto uniqueSubId = m_subUriToIdMap[uri];

            ss << L"[";
            ss << static_cast<uint32_t>(dataType);
            ss << ",";
            ss << uniqueSubId;
            ss << L",";
            ss << _wtoi(eventData.c_str());
            ss << L",";
            ss << L"\"error message\"]";
        }
        else if (dataType == xbox::services::real_time_activity::real_time_activity_message_type::change_event)
        {
            auto eventId = m_eventUriToIdMap[uri];

            ss << L"[";
            ss << static_cast<uint32_t>(dataType);
            ss << ",";
            ss << eventId;
            ss << L",";
            ss << eventData;
            ss << L"]";
        }
        else if (dataType == xbox::services::real_time_activity::real_time_activity_message_type::subscribe)
        {
            auto uniqueSubId = m_subUriToIdMap[uri];

            auto tryIntParse = _wtoi(eventData.c_str());
            if (tryIntParse == 0)
            {
                m_eventUriToIdMap[uri] = m_sequenceNum;
                ss << L"[";
                ss << static_cast<uint32_t>(dataType);
                ss << ",";
                ss << uniqueSubId;
                ss << L",0,";
                ss << m_sequenceNum;
                ss << L",";
                ss << eventData;
                ss << L"]";
            }

            ++m_sequenceNum;
        }

        recieve_message(ss.str());
    }

    void receive_rta_event(int subId, string_t eventData)
    {
        stringstream_t ss;
        ss << L"[3,";
        ss << subId;
        ss << L",";
        ss << eventData;
        ss << L"]";

        recieve_message(ss.str());
    }

    MockWebSocketClient() {}

    void reinit()
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_waitForSignal = false;
        m_connectToFail = false;
        m_closeStatus = web::websockets::client::websocket_close_status::normal;
        m_subUriToIdMap.clear();
        m_eventUriToIdMap.clear();
        m_closeHandler = nullptr;
        m_sendHandler = nullptr;
        m_receiveHandler = nullptr;
        m_connectEvent.reset();
        m_sequenceNum = 0;
    }

    bool m_waitForSignal = false;
    bool m_connectToFail = false;
    web::websockets::client::websocket_close_status m_closeStatus = web::websockets::client::websocket_close_status::normal;
    concurrency::event m_connectEvent;

    std::unordered_map<string_t, uint32_t> m_subUriToIdMap;
    std::unordered_map<string_t, uint32_t> m_eventUriToIdMap;
    uint32_t m_sequenceNum = 0;
    std::function<void(uint16_t closeStatus, string_t closeReason)> m_closeHandler;
    std::function<void(string_t message)> m_sendHandler;
    std::function<void(string_t message)> m_receiveHandler;
    std::mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

