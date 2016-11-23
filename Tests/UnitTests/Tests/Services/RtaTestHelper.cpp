#include "pch.h"
#include <sstream>
#include <iomanip>
#include "RtaTestHelper.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::RealTimeActivity;
using namespace xbox::services::system;

void SetMultipleClientWebSocketRTAAutoResponser(std::vector<std::shared_ptr<MockWebSocketClient>> ws, string_t initData, int subId, bool useAutoReply)
{
    for (uint32_t i = 0; i < ws.size(); ++i)
    {
        auto socket = ws.at(i);
        SetWebSocketRTAAutoResponser(socket, initData, subId, useAutoReply);
    }
}

void SetWebSocketRTAAutoResponser(std::shared_ptr<MockWebSocketClient> ws, string_t subscriptionInitData, int subId, bool useAutoReply)
{
    ws->set_send_handler([ws, subscriptionInitData, subId, useAutoReply](string_t msg)
    {
        auto msgJson = web::json::value::parse(msg);
        int apiId = msgJson[0].as_integer();

        if (useAutoReply)
        {
            if (apiId == 1) //subscribe
            {
                int sequence = msgJson[1].as_integer();

                stringstream_t subscriptionResponse;
                subscriptionResponse << "[1,";
                subscriptionResponse << sequence;
                subscriptionResponse << ",0,";
                subscriptionResponse << (subId == -1 ? sequence : subId);
                subscriptionResponse << ",";
                subscriptionResponse << subscriptionInitData,
                    subscriptionResponse << "]";
                string_t response = subscriptionResponse.str();

                // will deadlock if return directly here, return in other thread
                pplx::create_task([ws, response]()
                {
                    ws->recieve_message(response);
                });
            }
            else if (apiId == 2) //unsubscribe
            {
                int sequence = msgJson[1].as_integer();
                stringstream_t subscriptionResponse;
                subscriptionResponse << "[2,";
                subscriptionResponse << sequence;
                subscriptionResponse << ",0]";
                string_t response = subscriptionResponse.str();

                // will deadlock if return directly here, return in other thread
                pplx::create_task([ws, response]()
                {
                    ws->recieve_message(response);
                });
            }
        }
    });
}

std::shared_ptr<StateChangeHelper> SetupStateChangeHelper(RealTimeActivityService^ rtaService)
{
    auto helper = std::make_shared<StateChangeHelper>();

    rtaService->RealTimeActivityConnectionStateChange += ref new Windows::Foundation::EventHandler<RealTimeActivityConnectionState>([helper](Platform::Object^ sender, RealTimeActivityConnectionState state)
    {
        if (state == RealTimeActivityConnectionState::Disconnected)
        {
            ++helper->disconnected;
            helper->disconnectedEvent.set();
        }

        if (state == RealTimeActivityConnectionState::Connecting)
        {
            ++helper->connecting;
            helper->connectingEvent.set();
        }

        if (state == RealTimeActivityConnectionState::Connected)
        {
            ++helper->connected;
            helper->connectedEvent.set();
        }
    });

    return helper;
}