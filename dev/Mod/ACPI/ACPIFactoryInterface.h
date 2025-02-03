/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __MOD_ACPI_H__
#define __MOD_ACPI_H__

#include <KernelKit/DebugOutput.h>
#include <Mod/ACPI/ACPI.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	class ACPIFactoryInterface;

	typedef ACPIFactoryInterface PowerFactoryInterface;

	class ACPIFactoryInterface final
	{
	public:
		explicit ACPIFactoryInterface(voidPtr rsp_ptr);
		~ACPIFactoryInterface() = default;

		ACPIFactoryInterface& operator=(const ACPIFactoryInterface&) = default;
		ACPIFactoryInterface(const ACPIFactoryInterface&)			 = default;

	public:
		Void Shutdown(); // shutdown
		Void Reboot();	 // soft-reboot

	public:
		/// @brief Descriptor find factory.
		/// @param signature The signature of the descriptor table (MADT, ACPI...)
		/// @return the blob inside an ErrorOr object.
		ErrorOr<voidPtr> Find(const Char* signature);

		/// @brief Checksum factory.
		/// @param checksum the data to checksum
		/// @param len it's size
		/// @return if it succeed
		bool Checksum(const Char* checksum, SSizeT len); // watch for collides!

	public:
		ErrorOr<voidPtr> operator[](const Char* signature)
		{
			return this->Find(signature);
		}

	private:
		VoidPtr fRsdp{nullptr}; // pointer to root descriptor.
		SSizeT	fEntries{0UL};	// number of entries, -1 tells that no invalid entries were
								// found.
	};
} // namespace Kernel

#endif // !__MOD_ACPI_H__
