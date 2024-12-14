/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

//! @file ThreadLocalStorage.inl
//! @brief Allocate resources from the process's heap storage.

#ifndef INC_PROCESS_SCHEDULER_H
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

//! @brief Delete process pointer.
//! @param obj The pointer to delete.
template <typename T>
inline Kernel::Bool tls_delete_ptr(T* obj) noexcept
{
	using namespace Kernel;

	if (!obj)
		return No;

	auto ref_process = UserProcessScheduler::The().GetCurrentProcess();
	MUST_PASS(ref_process);

	ErrorOr<T*> obj_wrapped{obj};

	return ref_process.Leak().Delete(obj_wrapped, sizeof(T));
}

//! @brief Delete process pointer.
//! @param obj The pointer to delete.
template <typename T>
inline Kernel::Bool tls_delete_ptr(Kernel::ErrorOr<T> obj) noexcept
{
	return tls_delete_ptr(obj.Leak());
}

//! @brief Delete process pointer.
//! @param obj The pointer to delete.
template <typename T>
inline Kernel::Bool tls_delete_ptr(Kernel::ErrorOr<T*> obj) noexcept
{
	return tls_delete_ptr(obj->Leak());
}

/// @brief Allocate a C++ class, and then call the constructor of it.
/// @tparam T class type.
/// @tparam ...Args varg class type.
/// @param args arguments list.
/// @return Class instance.
template <typename T, typename... Args>
T* tls_new_class(Args&&... args)
{
	using namespace Kernel;

	T* obj = tls_new_ptr<T>();

	if (obj)
	{
		*obj = T(forward(args)...);
		return obj;
	}

	return nullptr;
}

/// @brief Delete a C++ class (call constructor first.)
/// @tparam T
/// @param obj
/// @return
template <typename T>
inline Kernel::Bool tls_delete_class(T* obj)
{
	using namespace Kernel;

	if (!obj)
		return No;

	obj->~T();
	return tls_delete_ptr(obj);
}
