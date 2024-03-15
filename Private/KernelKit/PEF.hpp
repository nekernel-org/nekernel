/* -------------------------------------------

    Copyright Mahrouss Logic

    File: PEF.hpp
    Purpose: Preferred Executable Format for HCore.

    Revision History:

    ?/?/23: Added file (amlel)

------------------------------------------- */

#ifndef __PEF__
#define __PEF__

#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/LoaderInterface.hpp>
#include <NewKit/Defines.hpp>

#define kPefMagic "PEF"
#define kPefMagicFat "FEP"

#define kPefMagicLen 3

#define kPefVersion 1
#define kPefNameLen 64

/// @brief Preferred Executable Format, a format designed
/// for RISC/CISC Von-neumann processor types.

/// The PEF also uses the x64 PE calling convention and ABI.
/// It's just that the container are different.

namespace HCore {
enum {
  kPefArchIntel86S,
  kPefArchAMD64,
  kPefArchRISCV,
  kPefArch64x0, /* 64x0. ISA */
  kPefArch32x0, /* 32x0. ISA */
  kPefArchCount = (kPefArch32x0 - kPefArchIntel86S) + 1,
  kPefArchInvalid = 0xFF,
};

enum {
  kPefKindExec = 1,         /* .exe */
  kPefKindSharedObject = 2, /* .lib */
  kPefKindObject = 4,       /* .obj */
  kPefKindDebug = 5,        /* .debug */
  kPefKindDriver = 6,
  kPefKindCount,
};

typedef struct PEFContainer final {
  Char Magic[kPefMagicLen];
  UInt32 Linker;
  UInt32 Version;
  UInt32 Kind;
  UInt32 Abi;
  UInt32 Cpu;
  UInt32 SubCpu; /* Cpu specific information */
  UIntPtr Start;
  SizeT HdrSz; /* Size of header */
  SizeT Count; /* container header count */
} PACKED PEFContainer;

/* First PEFCommandHeader starts after PEFContainer */
/* Last container is __exec_end */

/* PEF executable section and commands. */

typedef struct PEFCommandHeader final {
  Char Name[kPefNameLen]; /* container name */
  UInt32 Flags;           /* container flags */
  UInt16 Kind;            /* container kind */
  UIntPtr Offset;         /* content offset */
  SizeT Size;             /* content Size */
} PACKED PEFCommandHeader;

enum {
  kPefCode = 0xC,
  kPefData = 0xD,
  kPefZero = 0xE,
  kPefLinkerID = 0x1,
};
}  // namespace HCore

#define kPefExt ".exe"
#define kPefDylibExt ".dll"
#define kPefLibExt ".lib"
#define kPefObjectExt ".obj"
#define kPefDebugExt ".pdb"

// HCore System Binary Interface.
#define kPefAbi (0xDEAD2)

#define kPefStart "__start"

#endif /* ifndef __PEF__ */
