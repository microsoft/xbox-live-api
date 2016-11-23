#pragma once
#include "pplx/threadpool.h"

namespace Concurrency
{
    namespace extras
    {
        template <typename Func>
        pplx::task<void> create_delayed_task(std::chrono::milliseconds delay, Func func)
        {
            pplx::task_completion_event<void> tce;
            
            auto pTimer = new boost::asio::deadline_timer(crossplat::threadpool::shared_instance().service());
            pTimer->expires_from_now(boost::posix_time::milliseconds(delay.count()));
            pTimer->async_wait([tce](const boost::system::error_code& ec)
            {
                tce.set();
            });

            return create_task(tce).then([pTimer]() {
                delete pTimer;
            }).then(func);
        }
    }
}