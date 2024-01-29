/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

using namespace hCore;

typedef struct EfiTableHeader
{
    UInt64 signature;
    UInt32 revision;
    UInt32 headerSize;
    UInt32 crc32;
    UInt32 reserved;
} EfiTableHeader;

struct EfiSimpleTextOutputProtocol;

typedef UInt64 (*EfiTextString)(struct EfiSimpleTextOutputProtocol *self, const wchar_t *string);

typedef struct EfiSimpleTextOutputProtocol
{
    UInt64 reset;
    EfiTextString outputString;
    UInt64 testString;
    UInt64 queryMode;
    UInt64 setMode;
    UInt64 setAttribute;
    UInt64 clearScreen;
    UInt64 setCursorPosition;
    UInt64 enableCursor;
    UInt64 mode;
} EfiSimpleTextOutputProtocol;

typedef struct EfiSystemTable
{
    EfiTableHeader hdr;
    wchar_t *firmwareVendor;
    UInt32 firmwareRevision;
    VoidPtr consoleInHandle;
    UInt64 conIn;
    VoidPtr consoleOutHandle;
    EfiSimpleTextOutputProtocol *conOut;
    VoidPtr standardErrorHandle;
    UInt64 stdErr;
    UInt64 runtimeServices;
    UInt64 bootServices;
    UInt64 numberOfTableEntries;
    UInt64 configurationTable;
} EfiSystemTable;

#define EfiMain efi_main
