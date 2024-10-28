/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

//! @file ThreadLocalStorage.inl
//! @brief Allocate resources from the process's heap storage.

#ifndef INC_PROCESS_SCHEDULER_HXX
#include <KernelKit/UserProcessScheduler.h>
#endif

template <typename T>
inline T* tls_new_ptr(void) noexcept
{
	using namespace Kernel;

	auto ref_process = UserProcessScheduler::The().GetCurrentProcess();
	MUST_PASS(ref_process);

	auto pointer = ref_process.Leak().New(sizeof(T));

	if (pointer.Error())
		return nullptr;

	return reinterpret_cast<T*>(pointer.Leak().Leak());
}

//! @brief TLS delete implementation.
template <typename T>
inline Kernel::Bool tls_delete_ptr(T* ptr) noexcept
{
	if (!ptr)
		return No;

	using namespace Kernel;

	auto ref_process = UserProcessScheduler::The().GetCurrentProcess();
	MUST_PASS(ref_process);

	return ref_process.Leak().Delete(ptr, sizeof(T));
}

/// @brief Allocate a C++ class, and then call the constructor of it.
/// @tparam T class type.
/// @tparam ...Args varg class type.
/// @param ...args arguments list.
/// @return Class instance.
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
