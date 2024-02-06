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

#endif /* ifndef __MSDOS_EXEC__ */
