/* -------------------------------------------

    Copyright Mahrouss Logic

    File: MSDOS.hpp
    Purpose: MS-DOS header for HCore.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __MSDOS_EXEC__
#define __MSDOS_EXEC__

#include <NewKit/Defines.hpp>

#include "PE.hpp"

typedef HCore::UInt32 DosWord;
typedef HCore::Long DosLong;

typedef struct _DosHeader {
  HCore::UInt8 eMagic[2];
  DosWord eMagLen;
  DosWord ePagesCount;
  DosWord eCrlc;
  DosWord eCParHdr;
  DosWord eMinAlloc;
  DosWord eMaxAlloc;
  DosWord eStackSeg;
  DosWord eStackPtr;
  DosWord eChksum;
  DosWord eIp;
  DosWord eCs;
  DosWord eLfarlc;
  DosWord eOvno;
  DosWord eRes[4];
  DosWord eOemid;
  DosWord eOeminfo;
  DosWord eRes2[10];
  DosLong eLfanew;
} DosHeader, *DosHeaderPtr;

namespace HCore {
/// @brief Find the PE header inside the the blob.
inline auto rt_find_exec_header(DosHeaderPtr ptrDos) -> VoidPtr {
  if (!ptrDos) return nullptr;
  if (ptrDos->eMagic[0] != kMagMz0) return nullptr;
  if (ptrDos->eMagic[1] != kMagMz1) return nullptr;

  return (VoidPtr)(&ptrDos->eLfanew + 1);
}
}  // namespace HCore

#endif /* ifndef __MSDOS_EXEC__ */
