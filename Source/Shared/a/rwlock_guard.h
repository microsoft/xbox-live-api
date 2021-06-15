#pragma once

#include <pthread.h>

#if XSAPI_ANDROID_STUDIO
#include "types.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Wrapper class to use pthread_rwlock_t in an RAII style
/// We cannot use std::shared_mutex as it was not introduced until c++17
/// </summary>
class rwlock_guard
{
public:
    /// <summary>
    /// Constructs a rwlock_guard and locks the given pthread_rwlock
    /// </summary>
    rwlock_guard(pthread_rwlock_t& lock, bool exclusive);

    /// <summary>
    /// Destructs the rwlock_guard and unlocks the pthread_rwlock
    /// </summary>
    ~rwlock_guard();

    rwlock_guard(const rwlock_guard&) = delete;
    rwlock_guard& operator=(const rwlock_guard&) = delete;
private:
    pthread_rwlock_t* rwlock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END