#ifndef _INC_FUNCTION_H__
#define _INC_FUNCTION_H__

#include <NewKit/Defines.h>

namespace Kernel
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
} // namespace Kernel

#endif // !_INC_FUNCTION_H__
