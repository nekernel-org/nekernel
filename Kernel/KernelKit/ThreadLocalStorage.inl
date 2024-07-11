/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

//! @brief Allocates a pointer from the process's tls.

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#include <KernelKit/ProcessScheduler.hxx>
#endif

template <typename T>
inline T* tls_new_ptr(void)
{
	using namespace Kernel;

	MUST_PASS(ProcessScheduler::The().Leak().TheCurrent());

	auto ref_process = ProcessScheduler::The().Leak().TheCurrent();

	T* pointer = (T*)ref_process.Leak().New(sizeof(T));
	return pointer;
}

//! @brief TLS delete implementation.
template <typename T>
inline Kernel::Bool tls_delete_ptr(T* ptr)
{
	if (!ptr)
		return false;

	using namespace Kernel;

	MUST_PASS(ProcessScheduler::The().Leak().TheCurrent());

	auto ref_process = ProcessScheduler::The().Leak().TheCurrent();
	return ref_process.Leak().Delete(ptr, sizeof(T));
}

/// @brief Allocate a C++ class, and then call the constructor of it.
/// @tparam T
/// @tparam ...Args
/// @param ...args
/// @return
template <typename T, typename... Args>
T* tls_new_class(Args&&... args)
{
	T* ptr = tls_new_ptr<T>();

	using namespace Kernel;

	if (ptr)
	{
		*ptr = T(forward(args)...);
		return ptr;
	}

	return nullptr;
}

/// @brief Delete a C++ class (call constructor first.)
/// @tparam T
/// @param ptr
/// @return
template <typename T>
inline Kernel::Bool tls_delete_class(T* ptr)
{
	if (!ptr)
		return false;

	ptr->~T();
	return tls_delete_ptr(ptr);
}
