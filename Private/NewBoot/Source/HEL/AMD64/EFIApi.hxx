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
struct HandoverHeader final
{
    int magic;
    int type;
    int arch;
    // WIP
};
} // namespace HEL
