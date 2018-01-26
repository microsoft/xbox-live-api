// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename... Args>
class xbox_live_callback
{
public:
    xbox_live_callback() : m_callable(nullptr) {}

    template <typename Functor>
    xbox_live_callback(Functor functor)
    {
        m_callable = xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(functor).release());
    }

    xbox_live_callback(const xbox_live_callback& rhs)
    {
        *this = rhs;
    }

    xbox_live_callback(xbox_live_callback&& rhs)
    {
        *this = std::move(rhs);
    }

    template <typename Functor>
    xbox_live_callback& operator=(Functor f)
    {
        m_callable = xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(f).release());
        return *this;
    }

    xbox_live_callback& operator=(const xbox_live_callback& rhs)
    {
        m_callable = rhs.m_callable->copy();
        return *this;
    }

    xbox_live_callback& operator=(xbox_live_callback&& rhs)
    {
        m_callable = std::move(rhs.m_callable);
        return *this;
    }

    void operator()(Args... args) const
    {
        (*m_callable)(args...);
    }

    bool operator==(std::nullptr_t) noexcept
    {
        return m_callable == nullptr;
    }

    bool operator!=(std::nullptr_t) noexcept
    {
        return m_callable != nullptr;
    }

private:
    struct ICallable
    {
        virtual ~ICallable() = default;
        virtual void operator()(Args...) = 0;
        virtual xsapi_unique_ptr<ICallable> copy() = 0;
    };

    template <typename Functor>
    struct callable : public ICallable
    {
        callable(const Functor& functor) : m_functor(functor) { }
        ~callable() override = default;

        void operator()(Args... args) override
        {
            m_functor(args...);
        }

        xsapi_unique_ptr<ICallable> copy() override
        {
            return xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(m_functor).release());
        }

        Functor m_functor;
    };

    xsapi_unique_ptr<ICallable> m_callable;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END