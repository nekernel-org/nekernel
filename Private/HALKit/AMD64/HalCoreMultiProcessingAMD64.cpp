/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Drivers/ACPI/ACPIManager.hxx>
#include <HALKit/AMD64/Processor.hpp>

///////////////////////////////////////////////////////////////////////////////////////

//! NOTE: fGSI stands 'Field Global System Interrupt'

namespace HCore::HAL {
constexpr Int32 kThreadAPIC = 0;
constexpr Int32 kThreadLAPIC = 1;
constexpr Int32 kThreadIOAPIC = 2;
constexpr Int32 kThreadAPIC64 = 3;
constexpr Int32 kThreadBoot = 4;

/*
 *
 * this is used to store info about the current running thread
 * we use this struct to determine if we can use it, or mark it as used or on
 * sleep.
 *
 */

struct ProcessorInfoAMD64 final {
  Int32 ThreadType;
  UIntPtr JumpAddress;

  struct {
    UInt32 Code;
    UInt32 Data;
    UInt32 BSS;
  } Selector;
};

static voidPtr kApicMadt = nullptr;
static const char* kApicSignature = "APIC";

struct Madt final {
  char fMag[4];
  Int32 fLength;
  char fRev;

  struct MadtAddress final {
    UInt32 fPhysicalAddress;
    UInt32 fFlags;  // 1 = Dual Legacy PICs installed

    Char fType;
    Char fRecLen;  // record length
  };
};

struct MadtProcessorLocalApic final {
  Char fProcessorId;
  Char fApicId;
  UInt32 fFlags;
};

struct MadtIOApic final {
  Char fApicId;
  Char fReserved;
  UInt32 fAddress;
  UInt32 fSystemInterruptBase;
};

struct MadtInterruptSource final {
  Char fBusSource;
  Char fIrqSource;
  UInt32 fGSI;
  UInt16 fFlags;
};

struct MadtInterruptNmi final {
  Char fNmiSource;
  Char fReserved;
  UInt16 fFlags;
  UInt32 fGSI;
};

struct MadtLocalApicAddressOverride final {
  UInt16 fResvered;
  UIntPtr fAddress;
};

///////////////////////////////////////////////////////////////////////////////////////

static Madt kApicMadtList[256];

Madt* system_find_core(Madt* madt) {
  madt = madt + sizeof(Madt);

  if (rt_string_cmp(madt->fMag, kApicSignature,
                    rt_string_len(kApicSignature)) == 0)
    return madt;

  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void system_get_cores(voidPtr rsdPtr) {
  auto acpi = ACPIManager(rsdPtr);
  kApicMadt = acpi.Find(kApicSignature).Leak().Leak();

  MUST_PASS(kApicMadt);  // MADT must exist.

  SizeT counter = 0UL;
  Madt* offset = system_find_core((Madt*)kApicMadt);
  //! now find core addresses.
  while (offset != nullptr) {
    // calls rt_copy_memory in NewC++
    kApicMadtList[counter] = *offset;
    offset = system_find_core(offset);

    ++counter;
  }
}
}  // namespace HCore::HAL

///////////////////////////////////////////////////////////////////////////////////////
