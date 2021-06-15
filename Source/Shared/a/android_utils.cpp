#include "android_utils.h"
#include <assert.h>
#include "Logger/log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
JString::JString(JNIEnv* env, jstring string) noexcept :
    m_env{ env },
    m_string{ string },
    m_cStr{ nullptr }
{
    assert(env);
    assert(string);
}

JString::~JString() noexcept
{
    if (m_cStr != nullptr)
    {
        assert(m_env);
        assert(m_string);

        m_env->ReleaseStringUTFChars(m_string, m_cStr);
    }
}

char const* JString::c_str()
{
    assert(m_env);
    assert(m_string);

    if (m_cStr == nullptr)
    {
        m_cStr = m_env->GetStringUTFChars(m_string, nullptr);

        if (m_cStr == nullptr)
        {
            LOG_ERROR("GetStringUTFChars failed");
        }
    }

    return m_cStr;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END