#include "a/rwlock_guard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

rwlock_guard::rwlock_guard(pthread_rwlock_t& lock, bool exclusive) :
    rwlock(&lock)
{
    if (exclusive)
    {
        pthread_rwlock_wrlock(rwlock);
    }
    else
    {
        pthread_rwlock_rdlock(rwlock);
    }
}

rwlock_guard::~rwlock_guard()
{
    pthread_rwlock_unlock(rwlock);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END