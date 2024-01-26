/*
 *	========================================================
 *
 *	hCore 	Date Added: 20/02/2023
 * 	Copyright XPX Corp, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_ACPI_H
#define _INC_ACPI_H

#include <HALKit/AMD64/ACPI/ACPI.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace hCore
{
    class ACPIManager
    {
    public:
        ACPIManager(voidPtr rsdptr);

    public:
        ~ACPIManager() = default;

        ACPIManager &operator=(const ACPIManager &) = default;

        ACPIManager(const ACPIManager &) = default;

    public:
        void Shutdown(); // shutdown
        void Reset();    // soft-reboot

        ErrorOr <voidPtr> Find(const char *signature);

        bool Checksum(const char *checksum, SSizeT len); // watch for collides!

    public:
        ErrorOr <voidPtr> operator[](const char *signature) {
            return this->Find(signature);
        }

    private:
        VoidPtr m_Rsdp;   // pointer to root descriptor.
        SSizeT m_Entries; // number of entries, -1 tells that no invalid entries were found.

    };
} // namespace hCore

#endif // !_INC_ACPI_H
