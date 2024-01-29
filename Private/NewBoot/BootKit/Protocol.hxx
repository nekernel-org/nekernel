/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <EFIKit/EFI.hxx>

namespace HEL
{
using namespace hCore;

struct HandoverHeader final
{
    Int32 magic;
    Int32 type;
    Int32 arch;
    // WIP
};
} // namespace HEL

#define kHandoverMagic 0xBAD55

#define kBaseHandoverStruct 0x10000000
#define kHandoverStructSz sizeof(HEL::HandoverHeader)
