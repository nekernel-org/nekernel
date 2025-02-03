/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: MSDOS.h
	Purpose: MS-DOS header for Kernel.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __MSDOS_EXEC__
#define __MSDOS_EXEC__

#include <KernelKit/PE.h>
#include <NewKit/Defines.h>

// Last Rev
// Sat Feb 24 CET 2024

#define kMagMz0 'M'
#define kMagMz1 'Z'

typedef Kernel::UInt32 DosWord;
typedef Kernel::Long   DosLong;

typedef struct _DosHeader
{
	Kernel::UInt8 eMagic[2];
	DosWord		  eMagLen;
	DosWord		  ePagesCount;
	DosWord		  eCrlc;
	DosWord		  eCParHdr;
	DosWord		  eMinAlloc;
	DosWord		  eMaxAlloc;
	DosWord		  eStackSeg;
	DosWord		  eStackPtr;
	DosWord		  eChksum;
	DosWord		  eIp;
	DosWord		  eCs;
	DosWord		  eLfarlc;
	DosWord		  eOvno;
	DosWord		  eRes[4];
	DosWord		  eOemid;
	DosWord		  eOeminfo;
	DosWord		  eRes2[10];
	DosLong		  eLfanew;
} DosHeader, *DosHeaderPtr;

#endif /* ifndef __MSDOS_EXEC__ */
