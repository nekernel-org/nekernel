/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: DDK DLL Base Header.

------------------------------------------- */

#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
#define DK_EXTERN extern "C"
#define nil		  nullptr
#define DK_FINAL  final
#else
#define DK_EXTERN extern
#define nil		  ((void*)0)
#define DK_FINAL
#endif // defined(__cplusplus)

#ifndef __NEWOSKRNL__
#error !!! including header in user mode !!!
#endif // __NEWOSKRNL__

struct DDK_STATUS_STRUCT;
struct DDK_PROPERTY_RECORD;

struct DDK_PROPERTY_RECORD DK_FINAL
{
	char* p_name;
	void* p_object;
	void* p_xpcom_object;
};

/// \brief DDK status structure (__at_enable, __at_disable...)
struct DDK_STATUS_STRUCT DK_FINAL
{
	int32_t s_action_id;
	int32_t s_issuer_id;
	int32_t s_group_id;
	void*	s_object;
};

/// @brief Call Kernel (interrupt 0x33)
/// @param KernelRpcName
/// @param cnt number of elements in **dat**
/// @param dat data ptr
/// @param sz sz of whole data ptr.
/// @return result of call
DK_EXTERN void* KernelCall(const char* KernelRpcName, int32_t cnt, void* dat, size_t sz);

/// @brief add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DK_EXTERN void KernelAddSyscall(const int slot, void (*slotFn)(void* a0));

/// @brief allocate heap ptr.
/// @param sz size of ptr.
/// @return the pointer allocated or **nil**.
DK_EXTERN void* KernelAlloc(size_t sz);

/// @brief free heap ptr.
/// @param pointer to free
DK_EXTERN void KernelFree(void*);

/// @brief Get a Kernel property.
/// @param slot property id (always 0)
/// @param name the property's name.
/// @return property's object.
DK_EXTERN void* KernelGetProperty(const int slot, const char* name);

/// @brief Set a Kernel property.
/// @param slot property id (always 0)
/// @param name the property's name.
/// @param ddk_pr pointer to a  property's DDK_PROPERTY_RECORD.
/// @return property's object.
DK_EXTERN void* KernelSetProperty(const int slot, const struct DDK_PROPERTY_RECORD* ddk_pr);

/// @brief The highest API version of the DDK.
DK_EXTERN int32_t c_api_version_highest;

/// @brief The lowest API version of the DDK.
DK_EXTERN int32_t c_api_version_least;

/// @brief c_api_version_least+c_api_version_highest combined version.
DK_EXTERN int32_t c_api_version;
