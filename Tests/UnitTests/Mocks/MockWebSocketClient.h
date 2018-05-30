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
    void connect(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ const xsapi_internal_string& uri,
        _In_ const xsapi_internal_string& subProtocol,
        _In_ xbox::services::xbox_live_callback<WebSocketCompletionResult> callback
        ) override
    {
        UNREFERENCED_PARAMETER(subProtocol);
        UNREFERENCED_PARAMETER(uri);
        UNREFERENCED_PARAMETER(userContext);
        if (m_waitForSignal)
        {
            m_connectEvent.wait();
        }
        callback(WebSocketCompletionResult{
            nullptr,
            m_connectToFail ? E_FAIL : S_OK,
            0
        });
    }

    void send(
        _In_ const xsapi_internal_string& message,
        _In_ xbox::services::xbox_live_callback<WebSocketCompletionResult> callback
        ) override
    {
        auto jsonValueObj = web::json::value::parse(utils::string_t_from_internal_string(message)).as_array();
        real_time_activity::real_time_activity_message_type messageType = static_cast<real_time_activity::real_time_activity_message_type>(jsonValueObj[0].as_integer());
        
        if (messageType == real_time_activity::real_time_activity_message_type::subscribe)
        {
            uint32_t uniqueId = jsonValueObj[1].as_integer();
            xsapi_internal_string uri = utils::internal_string_from_string_t(jsonValueObj[2].as_string());
            m_subUriToIdMap[uri] = uniqueId;
        }
        if (m_sendHandler != nullptr)
        {
            m_sendHandler(message);
        }
        callback(WebSocketCompletionResult{
            nullptr,
            S_OK,
            0
        });
    }

    void close() override 
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_closeHandler != nullptr)
        {
            m_closeHandler(m_closeStatus);
        }
    }

    void set_received_handler(
        _In_ xbox_live_callback<xsapi_internal_string> handler
        ) override 
    {
        m_receiveHandler = handler;
    }

    void set_closed_handler(
        _In_ xbox_live_callback<HCWebSocketCloseStatus> handler
        ) override
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_closeHandler = handler;
    }

    void set_send_handler(
        _In_ xbox_live_callback<xsapi_internal_string> handler
        )
    {
        m_sendHandler = handler;
    }

    void recieve_message(
        _In_ string_t message
    )
    {
        receive_message(utils::internal_string_from_string_t(message));
    }


    void receive_message(
        _In_ xsapi_internal_string message
        )
    {
        if (m_receiveHandler != nullptr)
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
        receive_rta_event_from_uri(
            utils::internal_string_from_string_t(uri),
            utils::internal_string_from_string_t(eventData),
            dataType,
            isError
        );
    }

    void receive_rta_event_from_uri(const xsapi_internal_string& uri, const xsapi_internal_string& eventData, xbox::services::real_time_activity::real_time_activity_message_type dataType, bool isError)
    {
        if (m_subUriToIdMap.find(uri) == m_subUriToIdMap.end())
        {
            return;
        }
        xsapi_internal_stringstream ss;

        if (isError)
        {
            auto uniqueSubId = m_subUriToIdMap[uri];

            ss << "[";
            ss << static_cast<uint32_t>(dataType);
            ss << ",";
            ss << uniqueSubId;
            ss << ",";
            ss << atoi(eventData.c_str());
            ss << ",";
            ss << "\"error message\"]";
        }
        else if (dataType == xbox::services::real_time_activity::real_time_activity_message_type::change_event)
        {
            auto eventId = m_eventUriToIdMap[uri];

            ss << "[";
            ss << static_cast<uint32_t>(dataType);
            ss << ",";
            ss << eventId;
            ss << ",";
            ss << eventData;
            ss << "]";
        }
        else if (dataType == xbox::services::real_time_activity::real_time_activity_message_type::subscribe)
        {
            auto uniqueSubId = m_subUriToIdMap[uri];

            auto tryIntParse = atoi(eventData.c_str());
            if (tryIntParse == 0)
            {
                m_eventUriToIdMap[uri] = m_sequenceNum;
                ss << "[";
                ss << static_cast<uint32_t>(dataType);
                ss << ",";
                ss << uniqueSubId;
                ss << ",0,";
                ss << m_sequenceNum;
                ss << ",";
                ss << eventData;
                ss << "]";
            }

            ++m_sequenceNum;
        }

        receive_message(ss.str());
    }

    void receive_rta_event(int subId, string_t eventData)
    {
        receive_rta_event(subId, utils::internal_string_from_string_t(eventData));
    }

    void receive_rta_event(int subId, xsapi_internal_string eventData)
    {
        xsapi_internal_stringstream ss;
        ss << "[3,";
        ss << subId;
        ss << ",";
        ss << eventData;
        ss << "]";

        receive_message(ss.str());
    }

    MockWebSocketClient() {}

    void reinit()
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_waitForSignal = false;
        m_connectToFail = false;
        m_closeStatus = HCWebSocketCloseStatus_Normal;
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
    HCWebSocketCloseStatus m_closeStatus = HCWebSocketCloseStatus_Normal;
    concurrency::event m_connectEvent;

    std::unordered_map<xsapi_internal_string, uint32_t> m_subUriToIdMap;
    std::unordered_map<xsapi_internal_string, uint32_t> m_eventUriToIdMap;
    uint32_t m_sequenceNum = 0;
    xbox_live_callback<HCWebSocketCloseStatus> m_closeHandler;
    xbox_live_callback<xsapi_internal_string> m_sendHandler;
    xbox_live_callback<xsapi_internal_string> m_receiveHandler;
    std::mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

