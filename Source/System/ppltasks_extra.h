// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <ppltasks.h>
#include <agents.h>
#include <memory>
#include <chrono>

namespace Concurrency
{
    namespace extras
    {
        namespace details
        {
            static void iterative_task_impl(concurrency::task_completion_event<void> finished, std::function<concurrency::task<bool>()> body, concurrency::cancellation_token ct)
            {
                body().then([=](concurrency::task<bool> previous) {
                    try {
                        if (previous.get())
                            iterative_task_impl(finished, body, ct);
                        else
                            finished.set();
                    }
                    catch (...) {
                        finished.set_exception(std::current_exception());
                    }
                }, ct);
            }
        } // namespace details


          /// <summary>
          ///     Creates a task from given user Functor, which will be scheduled after <paramref name="delay"/> million senconds.
          /// </summary>
          /// <typeparam name="Func">
          ///     The type of the user task Functor.
          /// </typeparam>
          /// <param name="delay">
          ///     The delay before starting the task.
          /// </param>
          /// <param name="func">
          ///     The user task Functor.
          /// </param>
          /// <param name="token">
          ///     The optional cancellation token, which will be passed to the return task.
          /// </param>
          /// <returns>
          ///     It will return a task constructed with user Functor <paramref name="func"/>, and cancellation token <paramref name="token"/>.
          ///     The returning task will be scheduled after <paramref name="delay"/> million seconds.
          /// </returns>
          /// <seealso cref="Task Parallelism (Concurrency Runtime)"/>
          /**/
        template <typename Func>
        auto create_delayed_task(std::chrono::milliseconds delay, Func func, concurrency::cancellation_token token = concurrency::cancellation_token::none()) -> decltype(create_task(func))
        {
            concurrency::task_completion_event<void> tce;

            auto pTimer = new concurrency::timer<int>(static_cast<int>(delay.count()), 0, NULL, false);
            auto pCallback = new concurrency::call<int>([tce](int) {
                tce.set();
            });

            pTimer->link_target(pCallback);
            pTimer->start();

            return create_task(tce).then([pCallback, pTimer]() {
                delete pCallback;
                delete pTimer;
            }).then(func, token);
        }

        /// <summary>
        ///     Creates a task iteratively execute user Functor. During the process, each new iteration will be the continuation of the
        ///     last iteration's returning task, and the process will keep going on until the Boolean value from returning task becomes False.
        /// </summary>
        /// <param name="body">
        ///     The user Functor used as loop body. It is required to return a task with type bool, which used as predictor that decides
        ///     whether the loop needs to be continued.
        /// </param>
        /// <param name="ct">
        ///     The cancellation token linked to the iterative task.
        /// </param>
        /// <returns>
        ///     The task that will perform the asynchronous iterative execution.
        /// </returns>
        /// <remarks>
        ///     This function dynamically creates a long chain of continuations by iteratively concating tasks created by user Functor <paramref name="body"/>,
        ///     The iteration will not stop until the result of the returning task from user Functor <paramref name="body"/> is <c> False </c>.
        /// </remarks>
        /**/
        inline task<void> create_iterative_task(std::function<concurrency::task<bool>()> body, cancellation_token ct = cancellation_token::none())
        {
            concurrency::task_completion_event<void> finished;
            create_task([=] {
                try {
                    details::iterative_task_impl(finished, body, ct);
                }
                catch (...) {
                    finished.set_exception(std::current_exception());
                }
            }, ct);
            return create_task(finished, ct);
        }

    }
}

