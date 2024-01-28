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
    EfiTextString output_string;
    UInt64 test_string;
    UInt64 query_mode;
    UInt64 set_mode;
    UInt64 set_attribute;
    UInt64 clear_screen;
    UInt64 set_cursor_position;
    UInt64 enable_cursor;
    UInt64 mode;
} EfiSimpleTextOutputProtocol;

typedef struct EfiSystemTable
{
    EfiTableHeader hdr;
    Int16 *firmwareVendor;
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
