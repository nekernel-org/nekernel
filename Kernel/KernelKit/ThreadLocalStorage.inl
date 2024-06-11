/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

//! @brief Allocates a pointer from the process's tls.

#ifndef __PROCESS_MANAGER__
#include <KernelKit/ProcessScheduler.hxx>
#endif

template <typename T>
inline T* tls_new_ptr(void)
{
	using namespace NewOS;

	MUST_PASS(ProcessScheduler::The().Leak().GetCurrent());

	auto ref_process = ProcessScheduler::The().Leak().GetCurrent();

	T* pointer = (T*)ref_process.Leak().New(sizeof(T));
	return pointer;
}

//! @brief TLS delete implementation.
template <typename T>
inline bool tls_delete_ptr(T* ptr)
{
	if (!ptr)
		return false;

	using namespace NewOS;

	MUST_PASS(ProcessScheduler::The().Leak().GetCurrent());

	auto ref_process = ProcessScheduler::The().Leak().GetCurrent();
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

	if (ptr)
	{
		*ptr = T(NewOS::forward(args)...);
		return ptr;
	}

	return nullptr;
}

/// @brief Delete a C++ class (call constructor first.)
/// @tparam T
/// @param ptr
/// @return
template <typename T>
inline bool tls_delete_class(T* ptr)
{
	ptr->~T();
	return tls_delete_ptr(ptr);
}