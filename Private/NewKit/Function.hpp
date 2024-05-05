#ifndef _INC_FUNCTION_HPP__
#define _INC_FUNCTION_HPP__

#include <NewKit/Defines.hpp>

namespace NewOS
{
	template <typename T, typename... Args>
	class Function final
	{
	public:
		Function() = default;

	public:
		explicit Function(T (*Fn)(Args... args))
			: fFn(Fn)
		{
		}

		~Function() = default;

		Function& operator=(const Function&) = default;
		Function(const Function&)			 = default;

		template <typename... XArgs>
		T operator()(Args... args)
		{
			return fFn(args...);
		}

		template <typename... XArgs>
		T Call(Args... args)
		{
			return fFn(args...);
		}

		operator bool()
		{
			return fFn;
		}

		bool operator!()
		{
			return !fFn;
		}

	private:
		T(*fFn)
		(Args... args);
	};
} // namespace NewOS

#endif // !_INC_FUNCTION_HPP__
