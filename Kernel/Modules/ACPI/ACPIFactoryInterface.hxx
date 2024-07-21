/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef __ACPI_MANAGER__
#define __ACPI_MANAGER__

#include <Modules/ACPI/ACPI.hxx>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hxx>

namespace Kernel
{
	class ACPIFactoryInterface;

	typedef ACPIFactoryInterface PowerFactoryInterface;

	class ACPIFactoryInterface final
	{
	public:
		explicit ACPIFactoryInterface(voidPtr rsdPtr);
		~ACPIFactoryInterface() = default;

		ACPIFactoryInterface& operator=(const ACPIFactoryInterface&) = default;
		ACPIFactoryInterface(const ACPIFactoryInterface&)			 = default;

	public:
		void Shutdown(); // shutdown
		void Reboot();	 // soft-reboot

	public:
		/// @brief Descriptor find factory.
		/// @param signature The signature of the descriptor table (MADT, ACPI...)
		/// @return the blob inside an ErrorOr object.
		ErrorOr<voidPtr> Find(const char* signature);

		/// @brief Checksum factory.
		/// @param checksum the data to checksum
		/// @param len it's size
		/// @return if it succeed
		bool Checksum(const char* checksum, SSizeT len); // watch for collides!

	public:
		ErrorOr<voidPtr> operator[](const char* signature)
		{
			return this->Find(signature);
		}

	private:
		VoidPtr fRsdp;	  // pointer to root descriptor.
		SSizeT	fEntries; // number of entries, -1 tells that no invalid entries were
						  // found.
	};
} // namespace Kernel

#endif // !__ACPI_MANAGER__
