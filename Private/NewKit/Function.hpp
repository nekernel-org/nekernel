#ifndef _INC_FUNCTION_HPP__
#define _INC_FUNCTION_HPP__

#include <NewKit/Defines.hpp>

namespace hCore
{
    template <typename T, typename... Args>
    class Function final
    {
    public:
        Function() = default;

    public:
        explicit Function(T (*Fn)(Args... args))
            : m_Fn(Fn)
        {}

        ~Function() = default;

        Function &operator=(const Function &) = default;
        Function(const Function &) = default;

        template <typename... XArgs> T operator()(Args... args)
        {
            return m_Fn(args...);
        }

        template <typename... XArgs> T Call(Args... args)
        {
            return m_Fn(args...);
        }

        operator bool()
        {
            return m_Fn;
        }
        bool operator!()
        {
            return !m_Fn;
        }

    private:
        T (*m_Fn)(Args... args);

    };
} // namespace hCore

#endif // !_INC_FUNCTION_HPP__
