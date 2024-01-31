
/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/RuntimeCheck.hpp>
#include <NewKit/Ref.hpp>

namespace HCore
{
template <typename T> class OwnPtr;

template <typename T> class NonNullRefPtr;

template <typename T> class OwnPtr final
{
  public:
    OwnPtr() {}
    ~OwnPtr() { this->Delete(); }

    OwnPtr &operator=(const OwnPtr &) = default;
    OwnPtr(const OwnPtr &) = default;

  public:
    template <typename... Args> bool New(Args &&...arg)
    {
        m_Cls = new T(arg...);
        return m_Cls;
    }

    void Delete()
    {
        if (m_Cls)
            delete m_Cls;

        m_Cls = nullptr;
    }

    T *operator->() const { return m_Cls; };
    T *Raw() { return m_Cls; }

    Ref<T> AsRef() { return Ref<T>(m_Cls); }

    operator bool() { return m_Cls; }
    bool operator!() { return !m_Cls; }

  private:
    T *m_Cls;
};

template <typename T, typename... Args> OwnPtr<T> make_ptr(Args... args)
{
    OwnPtr<T> ret;
    ret.template New<Args...>(forward(args)...);
    MUST_PASS(ret);

    return ret;
}
} // namespace HCore
