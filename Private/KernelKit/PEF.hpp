/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __PEF__
#define __PEF__

#include <CompilerKit/CompilerKit.hpp>
#include <KernelKit/Loader.hpp>
#include <NewKit/Defines.hpp>

#define kPefMagic "PEF"
#define kPefMagicFat "FEP"

#define kPefMagicLen 3

#define kPefVersion 1
#define kPefNameLen 64

// @brief Preferred Executable Format, a format designed for any computer.

namespace HCore {
enum {
  kPefArchIntel86S,
  kPefArchAMD64,
  kPefArchRISCV,
  kPefArch64x0, /* 64x000. */
  kPefArch32x0,
  kPefArchInvalid = 0xFF,
};

enum {
  kPefKindExec = 1,         /* .exe */
  kPefKindSharedObject = 2, /* .lib */
  kPefKindObject = 4,       /* .obj */
  kPefKindDebug = 5,        /* .debug */
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
} __attribute__((packed)) PEFContainer;

/* First PEFCommandHeader starts after PEFContainer */
/* Last container is __exec_end */

/* PEF executable section and commands. */

typedef struct PEFCommandHeader final {
  Char Name[kPefNameLen]; /* container name */
  UInt32 Flags;           /* container flags */
  UInt16 Kind;            /* container kind */
  UIntPtr Offset;         /* content offset */
  SizeT Size;             /* content Size */
} __attribute__((packed)) PEFCommandHeader;

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
